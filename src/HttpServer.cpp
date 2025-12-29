#include "HttpServer.h"

#include <algorithm>
#include <cerrno>
#include <cctype>
#include <cstring>
#include <stdexcept>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "platform/socket_init.h"

namespace {
std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

// Reads until delimiter "\r\n\r\n" is found or limit is exceeded.
std::string readHeaders(int sock, size_t maxBytes) {
    std::string buf;
    buf.reserve(2048);
    while (buf.size() < maxBytes) {
        char tmp[1024];
        ssize_t n = ::recv(sock, tmp, sizeof(tmp), 0);
        if (n == 0) break;
        if (n < 0) {
            if (errno == EINTR) continue;
            throw std::runtime_error(std::string("recv() failed: ") + std::strerror(errno));
        }
        buf.append(tmp, tmp + n);
        if (buf.find("\r\n\r\n") != std::string::npos) {
            return buf;
        }
    }
    throw std::runtime_error("request headers too large or incomplete");
}

HttpRequest parseRequest(const std::string& raw, std::vector<char>& leftoverBody) {
    HttpRequest req;
    const size_t headerEnd = raw.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        throw std::runtime_error("malformed HTTP request");
    }

    std::string headerPart = raw.substr(0, headerEnd);
    std::string after = raw.substr(headerEnd + 4);
    leftoverBody.assign(after.begin(), after.end());

    // Parse request line
    const size_t firstLineEnd = headerPart.find("\r\n");
    const std::string requestLine = (firstLineEnd == std::string::npos) ? headerPart : headerPart.substr(0, firstLineEnd);
    {
        // "METHOD PATH VERSION"
        size_t a = requestLine.find(' ');
        size_t b = (a == std::string::npos) ? std::string::npos : requestLine.find(' ', a + 1);
        if (a == std::string::npos || b == std::string::npos) {
            throw std::runtime_error("malformed request line");
        }
        req.method = requestLine.substr(0, a);
        req.path = requestLine.substr(a + 1, b - (a + 1));
        req.version = requestLine.substr(b + 1);
    }

    // Parse headers
    size_t pos = (firstLineEnd == std::string::npos) ? headerPart.size() : (firstLineEnd + 2);
    while (pos < headerPart.size()) {
        size_t lineEnd = headerPart.find("\r\n", pos);
        if (lineEnd == std::string::npos) lineEnd = headerPart.size();
        std::string line = headerPart.substr(pos, lineEnd - pos);
        pos = lineEnd + 2;

        if (line.empty()) continue;
        size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string key = toLower(line.substr(0, colon));
        std::string value = line.substr(colon + 1);
        // trim leading spaces
        while (!value.empty() && (value[0] == ' ' || value[0] == '\t')) value.erase(value.begin());
        req.headers[key] = value;
    }

    return req;
}

size_t parseContentLength(const HttpRequest& req) {
    auto it = req.headers.find("content-length");
    if (it == req.headers.end()) return 0;
    try {
        return static_cast<size_t>(std::stoul(it->second));
    } catch (...) {
        throw std::runtime_error("invalid Content-Length");
    }
}

void sendAll(int sock, const char* data, size_t len) {
    size_t off = 0;
    while (off < len) {
        ssize_t n = ::send(sock, data + off, len - off, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) throw std::runtime_error("send() failed");
        off += static_cast<size_t>(n);
    }
}
} // namespace

HttpServer::HttpServer(int port)
    : port(port), serverSocket(-1), running(false) {}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::setHandler(Handler h) {
    handler = std::move(h);
}

void HttpServer::start() {
    if (running) return;

    ensure_socket_init();

    serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error(std::string("HttpServer::start: socket() failed: ") + std::strerror(errno));
    }

    int opt = 1;
    (void)::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(serverSocket);
        serverSocket = -1;
        throw std::runtime_error(std::string("HttpServer::start: bind() failed: ") + std::strerror(errno));
    }

    if (::listen(serverSocket, 16) < 0) {
        ::close(serverSocket);
        serverSocket = -1;
        throw std::runtime_error(std::string("HttpServer::start: listen() failed: ") + std::strerror(errno));
    }

    running = true;
    serverThread = std::thread(&HttpServer::acceptLoop, this);
}

void HttpServer::stop() {
    if (!running) return;
    running = false;

    if (serverSocket >= 0) {
        (void)::shutdown(serverSocket, SHUT_RDWR);
        ::close(serverSocket);
        serverSocket = -1;
    }
    if (serverThread.joinable()) {
        serverThread.join();
    }
}

void HttpServer::acceptLoop() {
    while (running) {
        int clientSock = ::accept(serverSocket, nullptr, nullptr);
        if (clientSock < 0) {
            if (!running) break;
            if (errno == EINTR) continue;
            continue;
        }

        auto handlerCopy = handler;
        std::thread([this, clientSock, handlerCopy]() mutable {
            // We keep parsing + response generation here; handler is pure function.
            (void)handlerCopy; // silence unused warning in some builds if handler is empty
            handleClient(clientSock);
        }).detach();
    }
}

void HttpServer::handleClient(int clientSock) {
    try {
        // 1) read & parse headers
        std::string raw = readHeaders(clientSock, 64 * 1024);
        std::vector<char> leftover;
        HttpRequest req = parseRequest(raw, leftover);

        // 2) read body according to Content-Length
        const size_t contentLength = parseContentLength(req);
        req.body = std::move(leftover);
        while (req.body.size() < contentLength) {
            char buf[4096];
            size_t want = std::min(sizeof(buf), contentLength - req.body.size());
            ssize_t n = ::recv(clientSock, buf, want, 0);
            if (n == 0) break;
            if (n < 0) {
                if (errno == EINTR) continue;
                throw std::runtime_error(std::string("recv() failed: ") + std::strerror(errno));
            }
            req.body.insert(req.body.end(), buf, buf + n);
        }
        if (req.body.size() != contentLength) {
            throw std::runtime_error("incomplete body");
        }

        // 3) produce response
        HttpResponse res;
        if (handler) {
            res = handler(req);
        } else {
            res.statusCode = 500;
            res.statusText = "Internal Server Error";
            res.headers["Content-Type"] = "text/plain; charset=utf-8";
            const std::string msg = "No handler configured.\n";
            res.body.assign(msg.begin(), msg.end());
        }

        // Ensure Content-Length is correct for binary payloads.
        res.headers["Content-Length"] = std::to_string(res.body.size());
        if (res.headers.find("Connection") == res.headers.end() &&
            res.headers.find("connection") == res.headers.end()) {
            res.headers["Connection"] = "close";
        }

        // 4) write response
        std::string header = "HTTP/1.1 " + std::to_string(res.statusCode) + " " + res.statusText + "\r\n";
        for (const auto& kv : res.headers) {
            header += kv.first + ": " + kv.second + "\r\n";
        }
        header += "\r\n";

        sendAll(clientSock, header.data(), header.size());
        if (!res.body.empty()) {
            sendAll(clientSock, res.body.data(), res.body.size());
        }
    } catch (const std::exception& e) {
        // Best-effort 400 response if parsing fails.
        const std::string body = std::string("Bad Request: ") + e.what() + "\n";
        const std::string resp =
            "HTTP/1.1 400 Bad Request\r\n"
            "Content-Type: text/plain; charset=utf-8\r\n"
            "Content-Length: " + std::to_string(body.size()) + "\r\n"
            "Connection: close\r\n"
            "\r\n" + body;
        (void)::send(clientSock, resp.data(), resp.size(), 0);
    }

    ::close(clientSock);
}


