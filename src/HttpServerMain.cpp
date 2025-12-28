#include "HttpServer.h"
#include "CompressionApi.h"

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    int port = 8081;
    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Usage: http_server [port]\n";
            return 2;
        }
    }

    CompressionApi api;
    HttpServer server(port);
    server.setHandler([&api](const HttpRequest& req) { return api.handle(req); });

    std::cout << "HTTP API listening on port " << port << "\n";
    std::cout << "Endpoints:\n";
    std::cout << "  POST /compress   (binary body)\n";
    std::cout << "  POST /decompress (binary body)\n";
    std::cout << "Press Enter to stop...\n";

    server.start();

    std::string line;
    std::getline(std::cin, line);

    server.stop();
    return 0;
}


