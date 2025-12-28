#include <catch2/catch_all.hpp>
#include "AdaptiveCompression.h"

#include <stdexcept>

TEST_CASE("AdaptiveCompression rejects unknown algorithm tags", "[adaptive][error]") {
    AdaptiveCompression algo;

    std::vector<char> bogus = {'?', 'A', 'B', 'C'};
    REQUIRE_THROWS_AS(algo.decompress(bogus), std::runtime_error);
}


