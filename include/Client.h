#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <vector>

/**
 * @brief Simple TCP client wrapper.
 *
 * Encapsulates socket lifecycle (RAII-ish) and basic send/receive helpers.
 */
class Client {
public:
    Client(const std::string& host, int port);
    ~Client();

    bool connect();
    void disconnect();

    bool sendData(const std::vector<char>& data);
    std::vector<char> receiveData(size_t maxSize = 4096);

    /**
     * @brief Half-close the connection for writing (send FIN) while keeping it open for reading.
     *
     * Useful for simple request/response patterns where the server reads until EOF.
     * @return true on success, false otherwise.
     */
    bool shutdownWrite();

private:
    std::string host;
    int port;
    int clientSocket;
    bool connected;
};

#endif


