#include <catch2/catch_all.hpp>
#include "RLECompression.h"

TEST_CASE("RLE handles single-character input", "[edge]") {
    RLECompression rle;
    std::vector<char> input = {'Z'};
    auto comp = rle.compress(input);
    auto decomp = rle.decompress(comp);
    REQUIRE(decomp == input);
}

TEST_CASE("RLE handles all same characters", "[edge]") {
    RLECompression rle;
    std::vector<char> input(100, 'X');
    auto comp = rle.compress(input);
    // Compression should significantly reduce size for repeated chars
    REQUIRE(comp.size() < input.size());
    auto decomp = rle.decompress(comp);
    REQUIRE(decomp == input);
}

TEST_CASE("RLE handles special characters and symbols", "[edge]") {
    RLECompression rle;
    std::vector<char> input = {'!', '!', '@', '#', '#', '#', '$', '$'};
    auto comp = rle.compress(input);
    auto decomp = rle.decompress(comp);
    REQUIRE(decomp == input);
}
