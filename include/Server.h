#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <thread>
#include <atomic>

/**
 * @brief Simple TCP server with per-connection handler threads.
 *
 * - `start()` launches an accept loop in a background thread.
 * - Each accepted client connection is handled in its own detached thread.
 */
class Server {
public:
    explicit Server(int port);
    ~Server();

    void start();
    void stop();

    void handleClient(int clientSocket);
    void setHandler(std::function<void(int)> handler);

private:
    int port;
    int serverSocket;
    std::atomic<bool> running;
    std::thread serverThread;
    std::function<void(int)> clientHandler;

    void acceptLoop();
};

#endif


