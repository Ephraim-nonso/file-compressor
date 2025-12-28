#include <catch2/catch_all.hpp>
#include "Server.h"

#include <thread>

TEST_CASE("Server start/stop are safe to call multiple times", "[network][lifecycle]") {
    Server server(9122);

    server.start();
    // start again should be a no-op, not a crash
    server.start();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));

    server.stop();
    // stop again should be a no-op
    server.stop();
}


