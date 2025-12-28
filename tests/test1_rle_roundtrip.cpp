#include <catch2/catch_all.hpp>
#include "RLECompression.h"
#include <cstdlib>
TEST_CASE("RLE compress-decompress roundtrip with random input", "[rle]") {
    RLECompression rle;
    std::string input;
    for (int i=0;i<100;i++) input.push_back('A' + rand()%3);
    std::vector<char> data(input.begin(), input.end());
    auto compressed = rle.compress(data);
    auto decompressed = rle.decompress(compressed);
    REQUIRE(decompressed == data);
}
