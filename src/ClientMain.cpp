#include "Client.h"
#include <iostream>
#include <string>

int main() {
    Client client("127.0.0.1", 8080);
    
    if (!client.connect()) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }
    
    std::string msg = "AAAAABBBBCCCCD";
    std::vector<char> data(msg.begin(), msg.end());
    
    if (!client.sendData(data)) {
        std::cerr << "Failed to send data\n";
        return 1;
    }
    
    auto response = client.receiveData();
    std::cout << "Received " << response.size() << " bytes\n";
    
    client.disconnect();
    return 0;
}
