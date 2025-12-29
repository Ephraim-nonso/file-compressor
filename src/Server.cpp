#include "Server.h"
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <cerrno>
#include <stdexcept>

#include "platform/socket_init.h"

namespace {
void defaultEchoHandler(int clientSock) {
    char buffer[4096];
    int bytes = static_cast<int>(::read(clientSock, buffer, sizeof(buffer)));
    if (bytes > 0) {
        (void)::send(clientSock, buffer, bytes, 0);
    }
    ::close(clientSock);
}
} // namespace

/**
 * Server constructor - Initialize member variables
 * This is already implemented for you.
 */
Server::Server(int port) : port(port), serverSocket(-1), running(false) {}

/**
 * Server destructor - Clean up resources
 * This is already implemented for you.
 */
Server::~Server() {
    stop();
}

/**
 * TODO: Implement server startup and socket binding
 * 
 * This method should start the server by creating a socket, binding it to a port,
 * and starting to listen for connections.
 * 
 * Important considerations:
 * - Check if already running (return early if so)
 * - Create a TCP socket using socket(AF_INET, SOCK_STREAM, 0)
 * - Set SO_REUSEADDR socket option to allow quick restart
 * - Bind socket to INADDR_ANY on the specified port
 * - Start listening with a reasonable backlog (e.g., 8)
 * - Set running flag to true
 * - Start acceptLoop in a separate thread (use std::thread)
 * - Throw std::runtime_error with descriptive message on failure
 * 
 * Hint: Use htons() for port conversion
 * Hint: Check return values and throw exceptions on errors
 * 
 * @throws std::runtime_error if socket creation, binding, or listening fails
 */
void Server::start() {
    if (running) {
        return;
    }

    ensure_socket_init();

    serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        throw std::runtime_error(std::string("Server::start: socket() failed: ") + std::strerror(errno));
    }

    int opt = 1;
#ifdef _WIN32
    const char* optPtr = reinterpret_cast<const char*>(&opt);
    const int optLen = static_cast<int>(sizeof(opt));
#else
    const void* optPtr = &opt;
    const socklen_t optLen = static_cast<socklen_t>(sizeof(opt));
#endif
    if (::setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, optPtr, optLen) < 0) {
        ::close(serverSocket);
        serverSocket = -1;
        throw std::runtime_error(std::string("Server::start: setsockopt(SO_REUSEADDR) failed: ") + std::strerror(errno));
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (::bind(serverSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(serverSocket);
        serverSocket = -1;
        throw std::runtime_error(std::string("Server::start: bind() failed: ") + std::strerror(errno));
    }

    if (::listen(serverSocket, 8) < 0) {
        ::close(serverSocket);
        serverSocket = -1;
        throw std::runtime_error(std::string("Server::start: listen() failed: ") + std::strerror(errno));
    }

    running = true;
    serverThread = std::thread(&Server::acceptLoop, this);
}

/**
 * TODO: Implement server shutdown
 * 
 * This method should gracefully stop the server and clean up resources.
 * 
 * Important considerations:
 * - Check if server is running (return early if not)
 * - Set running flag to false to signal threads to stop
 * - Shutdown and close the server socket
 * - Wait for serverThread to finish (use join() if joinable)
 * - Reset serverSocket to -1
 * 
 * Hint: Use shutdown(serverSocket, SHUT_RDWR) before close()
 * 
 * @return void
 */
void Server::stop() {
    if (!running) {
        return;
    }

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

/**
 * TODO: Implement client connection handler
 * 
 * This method handles communication with a single client.
 * 
 * Important considerations:
 * - If a custom clientHandler is set, use it
 * - Otherwise, implement default echo behavior:
 *   - Read data from the client socket
 *   - Send the same data back (echo)
 *   - Close the client socket when done
 * - Handle errors gracefully
 * 
 * Hint: Use read() to receive data and send() to transmit
 * Hint: Always close the client socket when done
 * 
 * @param clientSocket The socket descriptor for the connected client
 */
void Server::handleClient(int clientSocket) {
    if (clientHandler) {
        // Custom handler owns the socket lifecycle (tests close it themselves).
        clientHandler(clientSocket);
        return;
    }
    defaultEchoHandler(clientSocket);
}

/**
 * Set a custom handler function for client connections
 * This is already implemented for you.
 */
void Server::setHandler(std::function<void(int)> handler) {
    clientHandler = handler;
}

/**
 * TODO: Implement accept loop for incoming connections
 * 
 * This is the main server loop that accepts new client connections.
 * 
 * Important considerations:
 * - Loop while running flag is true
 * - Use accept() to wait for and accept new client connections
 * - Handle accept() errors appropriately (check running flag)
 * - For each accepted connection, create a new detached thread to handle the client
 * - Use std::thread with handleClient method
 * - Detach the thread so it runs independently
 * 
 * Hint: Check if running is still true after accept() fails
 * Hint: Use std::thread(&Server::handleClient, this, clientSocket).detach()
 * 
 * @return void (runs in separate thread until server stops)
 */
void Server::acceptLoop() {
    while (running) {
        int clientSock = ::accept(serverSocket, nullptr, nullptr);
        if (clientSock < 0) {
            if (!running) {
                break;
            }
            if (errno == EINTR) {
                continue;
            }
            // Transient error; continue accepting.
            continue;
        }

        // Copy handler so detached threads don't access `this` after stop/destruction.
        auto handlerCopy = clientHandler;
        std::thread([clientSock, handlerCopy]() mutable {
            if (handlerCopy) {
                handlerCopy(clientSock);
            } else {
                defaultEchoHandler(clientSock);
            }
        }).detach();
    }
}
