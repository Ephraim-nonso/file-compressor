#include <catch2/catch_all.hpp>
#include "RLECompression.h"
#include <stdexcept>

TEST_CASE("RLE decompress rejects malformed input", "[rle][error]") {
    RLECompression rle;

    SECTION("Odd-length input (missing count)") {
        std::vector<char> bad = {'A'}; // should be ['A', count]
        REQUIRE_THROWS_AS(rle.decompress(bad), std::runtime_error);
    }

    SECTION("Zero count is invalid") {
        std::vector<char> bad = {'A', 0};
        REQUIRE_THROWS_AS(rle.decompress(bad), std::runtime_error);
    }
}


