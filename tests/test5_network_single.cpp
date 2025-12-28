#include <catch2/catch_all.hpp>
#include "Server.h"
#include "Client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <sys/socket.h>

TEST_CASE("Client connects and receives echo", "[network]") {
    // Create and start server with echo handler
    Server server(9100);
    server.setHandler([](int clientSock) {
        char buffer[1024];
        int bytes = read(clientSock, buffer, sizeof(buffer));
        if (bytes > 0) {
            send(clientSock, buffer, bytes, 0);
        }
        close(clientSock);
    });
    server.start();
    
    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Create client and test connection
    Client client("127.0.0.1", 9100);
    REQUIRE(client.connect());
    
    // Send and receive data
    std::string msg = "Hello_" + std::to_string(rand());
    std::vector<char> data(msg.begin(), msg.end());
    REQUIRE(client.sendData(data));
    
    auto response = client.receiveData();
    REQUIRE(std::string(response.begin(), response.end()) == msg);
    
    client.disconnect();
    server.stop();
}
