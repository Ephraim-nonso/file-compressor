#include "Client.h"
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "platform/socket_init.h"

/**
 * Client constructor - Initialize member variables
 * This is already implemented for you.
 */
Client::Client(const std::string& host, int port) 
    : host(host), port(port), clientSocket(-1), connected(false) {}

/**
 * Client destructor - Clean up resources
 * This is already implemented for you.
 */
Client::~Client() {
    disconnect();
}

/**
 * TODO: Implement TCP socket connection to server
 * 
 * This method should establish a TCP connection to the server specified by host and port.
 * 
 * Important considerations:
 * - Check if already connected (return true if so)
 * - Create a TCP socket using socket(AF_INET, SOCK_STREAM, 0)
 * - Set up sockaddr_in structure with server address and port
 * - Use inet_pton() to convert IP address string to binary form
 * - Call ::connect() to establish connection
 * - Handle errors gracefully: close socket and return false on failure
 * - Update connected flag and return true on success
 * 
 * Hint: Use htons() to convert port to network byte order
 * Hint: Check return values of socket(), inet_pton(), and connect()
 * 
 * @return true if connection successful, false otherwise
 */
bool Client::connect() {
    if (connected) {
        return true;
    }

    ensure_socket_init();

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        return false;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) != 1) {
        ::close(clientSocket);
        clientSocket = -1;
        return false;
    }

    if (::connect(clientSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        ::close(clientSocket);
        clientSocket = -1;
        return false;
    }

    connected = true;
    return true;
}

/**
 * TODO: Implement socket disconnection
 * 
 * This method should close the socket and clean up the connection.
 * 
 * Important considerations:
 * - Check if connected before attempting to close
 * - Close the socket using close()
 * - Reset clientSocket to -1
 * - Set connected flag to false
 * 
 * @return void
 */
void Client::disconnect() {
    if (!connected && clientSocket < 0) {
        return;
    }

    if (clientSocket >= 0) {
        (void)shutdown(clientSocket, SHUT_RDWR);
        ::close(clientSocket);
    }
    clientSocket = -1;
    connected = false;
}

/**
 * TODO: Implement sending data over the socket
 * 
 * This method should send the provided data through the connected socket.
 * 
 * Important considerations:
 * - Check if connected before attempting to send
 * - Use send() system call to transmit data
 * - Verify that all bytes were sent successfully
 * - Return true only if all data was sent, false otherwise
 * 
 * Hint: Compare bytes sent with data.size()
 * 
 * @param data The data to send
 * @return true if all data sent successfully, false otherwise
 */
bool Client::sendData(const std::vector<char>& data) {
    if (!connected || clientSocket < 0) {
        return false;
    }
    if (data.empty()) {
        return true;
    }

    size_t totalSent = 0;
    while (totalSent < data.size()) {
        ssize_t sent = ::send(
            clientSocket,
            data.data() + totalSent,
            data.size() - totalSent,
            0
        );
        if (sent < 0 && errno == EINTR) {
            continue;
        }
        if (sent <= 0) {
            return false;
        }
        totalSent += static_cast<size_t>(sent);
    }
    return true;
}

/**
 * TODO: Implement receiving data from the socket
 * 
 * This method should receive data from the connected socket.
 * 
 * Important considerations:
 * - Check if connected before attempting to receive
 * - Create a buffer of maxSize to store received data
 * - Use read() or recv() to receive data from socket
 * - Handle cases where bytes <= 0 (error or connection closed)
 * - Resize the buffer to actual number of bytes received
 * - Return empty vector on error
 * 
 * @param maxSize Maximum number of bytes to receive (default 4096)
 * @return Vector containing received data, or empty vector on error
 */
std::vector<char> Client::receiveData(size_t maxSize) {
    std::vector<char> out;
    if (!connected || clientSocket < 0 || maxSize == 0) {
        return out;
    }

    // TCP is a byte stream, not a message protocol:
    // - we may need multiple reads to assemble the full response
    // - we treat peer-close (recv == 0) as "end of message" for our simple echo use-case
    out.reserve(std::min<size_t>(maxSize, 4096));
    while (out.size() < maxSize) {
        char buf[4096];
        size_t want = std::min(sizeof(buf), maxSize - out.size());
        ssize_t n = ::recv(clientSocket, buf, want, 0);
        if (n > 0) {
            out.insert(out.end(), buf, buf + n);
            continue;
        }
        if (n == 0) {
            break; // connection closed
        }
        // n < 0: retry on signal interruption; otherwise treat as error.
        if (errno == EINTR) {
            continue;
        }
        return {};
    }
    return out;
}

bool Client::shutdownWrite() {
    if (!connected || clientSocket < 0) {
        return false;
    }
    return (::shutdown(clientSocket, SHUT_WR) == 0);
}
