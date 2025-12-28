#include <catch2/catch_all.hpp>
#include "Server.h"
#include "Client.h"

#include <thread>
#include <string>

TEST_CASE("Server default handler echoes when no handler is set", "[network]") {
    Server server(9120);
    server.start();

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    Client client("127.0.0.1", 9120);
    REQUIRE(client.connect());

    std::string msg = "DefaultEcho_" + std::to_string(std::rand());
    std::vector<char> data(msg.begin(), msg.end());
    REQUIRE(client.sendData(data));

    // Server closes after echo, so receiveData should return the full echoed bytes.
    auto response = client.receiveData();
    REQUIRE(std::string(response.begin(), response.end()) == msg);

    client.disconnect();
    server.stop();
}


