#include <catch2/catch_all.hpp>
#include "Server.h"
#include "Client.h"
#include <thread>
#include <string>
#include <unistd.h>
#include <sys/socket.h>

TEST_CASE("Server handles multiple clients sequentially", "[network]") {
    // Create and start server with echo handler
    Server server(9101);
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
    
    // Test multiple clients
    int N = 3;
    for (int i = 0; i < N; i++) {
        Client client("127.0.0.1", 9101);
        REQUIRE(client.connect());
        
        std::string msg = "Client" + std::to_string(i);
        std::vector<char> data(msg.begin(), msg.end());
        REQUIRE(client.sendData(data));
        
        auto response = client.receiveData();
        REQUIRE(std::string(response.begin(), response.end()) == msg);
        
        client.disconnect();
    }
    
    server.stop();
}
