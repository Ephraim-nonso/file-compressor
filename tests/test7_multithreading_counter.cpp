#include <catch2/catch_all.hpp>
#include "Server.h"
#include "Client.h"
#include <thread>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <sys/socket.h>

TEST_CASE("Server handles concurrent clients with atomic counter", "[thread]") {
    // Shared atomic counter to track client handling
    std::atomic<int> counter{0};
    
    // Create and start server with counter-incrementing handler
    Server server(9102);
    server.setHandler([&counter](int clientSock) {
        counter++;  // Increment counter atomically
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
    
    // Launch multiple concurrent clients
    int numClients = 5;
    std::vector<std::thread> clientThreads;
    std::atomic<int> successCount{0};
    
    for (int i = 0; i < numClients; i++) {
        clientThreads.emplace_back([i, &successCount]() {
            Client client("127.0.0.1", 9102);
            if (client.connect()) {
                std::string msg = "Client" + std::to_string(i);
                std::vector<char> data(msg.begin(), msg.end());
                if (client.sendData(data)) {
                    auto response = client.receiveData();
                    if (std::string(response.begin(), response.end()) == msg) {
                        successCount++;
                    }
                }
                client.disconnect();
            }
        });
    }
    
    // Wait for all clients to complete
    for (auto& t : clientThreads) {
        t.join();
    }
    
    // Verify all clients were handled successfully
    REQUIRE(successCount == numClients);
    REQUIRE(counter == numClients);
    
    server.stop();
}
