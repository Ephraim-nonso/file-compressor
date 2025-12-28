#include <catch2/catch_all.hpp>
#include "Server.h"
#include "Client.h"

#include <thread>
#include <string>
#include <vector>

#include <unistd.h>
#include <sys/socket.h>

TEST_CASE("Client/server roundtrip works for payloads larger than 4KB", "[network][large]") {
    // Custom echo handler that reads until EOF and echoes everything back.
    // This avoids relying on a single read() call (TCP is a stream).
    Server server(9121);
    server.setHandler([](int clientSock) {
        std::vector<char> all;
        char buf[2048];
        while (true) {
            int n = read(clientSock, buf, sizeof(buf));
            if (n > 0) {
                all.insert(all.end(), buf, buf + n);
                continue;
            }
            break; // n == 0 (peer closed) or error
        }
        if (!all.empty()) {
            (void)send(clientSock, all.data(), all.size(), 0);
        }
        close(clientSock);
    });
    server.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Client client("127.0.0.1", 9121);
    REQUIRE(client.connect());

    // 10KB message ensures multiple recv() iterations on many platforms.
    std::string msg(10 * 1024, 'X');
    // add some variation
    msg[0] = 'A';
    msg[4096] = 'B';
    msg.back() = 'Z';

    std::vector<char> data(msg.begin(), msg.end());
    REQUIRE(client.sendData(data));

    // Signal end-of-message without fully closing the socket so we can still receive.
    REQUIRE(client.shutdownWrite());

    auto response = client.receiveData(64 * 1024);
    REQUIRE(response.size() == data.size());
    REQUIRE(std::string(response.begin(), response.end()) == msg);

    client.disconnect();
    server.stop();
}


