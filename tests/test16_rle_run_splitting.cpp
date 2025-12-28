#include <catch2/catch_all.hpp>
#include "RLECompression.h"

TEST_CASE("RLE splits runs longer than 255 correctly", "[rle][edge]") {
    RLECompression rle;

    // 300 of the same byte must be encoded as at least two runs: 255 + 45.
    std::vector<char> input(300, 'A');
    auto compressed = rle.compress(input);

    // Each run becomes 2 bytes; expect exactly 4 bytes here.
    REQUIRE(compressed.size() == 4);
    REQUIRE(compressed[0] == 'A');
    REQUIRE(static_cast<unsigned char>(compressed[1]) == 255);
    REQUIRE(compressed[2] == 'A');
    REQUIRE(static_cast<unsigned char>(compressed[3]) == 45);

    auto decompressed = rle.decompress(compressed);
    REQUIRE(decompressed == input);
}


