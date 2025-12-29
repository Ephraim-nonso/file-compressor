#include "HttpServer.h"
#include "CompressionApi.h"

#include <atomic>
#include <cstdlib>
#include <csignal>
#include <chrono>
#include <iostream>
#include <string> 
#include <thread>

namespace {
std::atomic<bool> g_stop{false};

void handleSignal(int) {
    g_stop = true;
}
} // namespace

int main(int argc, char** argv) {
    int port = 8081;
    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (...) {
            std::cerr << "Usage: http_server [port]\n";
            return 2;
        }
    } else {
        // Railway (and many other PaaS) provides the listen port via PORT.
        if (const char* envPort = std::getenv("PORT")) {
            try {
                port = std::stoi(envPort);
            } catch (...) {
                std::cerr << "Invalid PORT env var: '" << envPort << "'. Expected integer.\n";
                return 2;
            }
        }
    }

    CompressionApi api;
    HttpServer server(port);
    server.setHandler([&api](const HttpRequest& req) { return api.handle(req); });

    // Container-friendly lifecycle: don't depend on stdin being attached.
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    try {
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Failed to start HTTP server on port " << port << ": " << e.what() << "\n";
        std::cerr << "Tip: if you already have the docker api running, stop it or pick another port.\n";
        return 1;
    }

    std::cout << "HTTP API listening on port " << port << "\n";
    std::cout << "Endpoints:\n";
    std::cout << "  POST /compress   (binary body)\n";
    std::cout << "  POST /decompress (binary body)\n";
    std::cout << "Stopping: send SIGINT (Ctrl+C) or SIGTERM (docker stop)\n";

    while (!g_stop.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    server.stop();
    return 0;
}


