#include <catch2/catch_all.hpp>
#include "RLECompression.h"
TEST_CASE("RLE handles large input", "[scalability]") {
    RLECompression rle;
    std::vector<char> input(50000, 'A');
    auto compressed = rle.compress(input);
    auto decompressed = rle.decompress(compressed);
    REQUIRE(decompressed == input);
}
