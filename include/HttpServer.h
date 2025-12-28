#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "HttpTypes.h"

#include <atomic>
#include <functional>
#include <thread>

/**
 * @brief Tiny HTTP/1.1 server built on TCP sockets (no external deps).
 *
 * Design goals:
 * - Modular: the server only parses HTTP and delegates to a handler.
 * - Binary-safe: request/response bodies are raw bytes.
 * - Simple: supports a single request per connection (sufficient for API usage).
 */
class HttpServer {
public:
    using Handler = std::function<HttpResponse(const HttpRequest&)>;

    explicit HttpServer(int port);
    ~HttpServer();

    void setHandler(Handler handler);

    void start();
    void stop();

private:
    int port;
    int serverSocket;
    std::atomic<bool> running;
    std::thread serverThread;
    Handler handler;

    void acceptLoop();
    void handleClient(int clientSocket);
};

#endif


