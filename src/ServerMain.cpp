#include "Server.h"
#include "RLECompression.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <sys/socket.h>

int main() {
    Server server(8080);
    
    // Set up compression handler
    server.setHandler([](int clientSock) {
        RLECompression algo;
        char buffer[4096];
        int bytes = read(clientSock, buffer, sizeof(buffer));
        if (bytes <= 0) { close(clientSock); return; }
        std::vector<char> data(buffer, buffer + bytes);
        auto compressed = algo.compress(data);
        send(clientSock, compressed.data(), compressed.size(), 0);
        close(clientSock);
    });
    
    std::cout << "Server listening on port 8080\n";
    server.start();
    
    // Keep main thread alive
    std::string input;
    std::cout << "Press Enter to stop server...\n";
    std::getline(std::cin, input);
    
    server.stop();
    return 0;
}
