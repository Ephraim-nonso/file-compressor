#include <catch2/catch_all.hpp>
#include "RLECompression.h"
TEST_CASE("RLE handles empty input", "[rle]") {
    RLECompression rle;
    std::vector<char> input;
    auto compressed = rle.compress(input);
    auto decompressed = rle.decompress(compressed);
    REQUIRE(decompressed.empty());
    // Also test with non-empty input to verify stub doesn't accidentally work
    std::vector<char> nonEmpty = {'A'};
    auto compressedNonEmpty = rle.compress(nonEmpty);
    REQUIRE(!compressedNonEmpty.empty()); // Compressed data should not be empty for non-empty input
}
