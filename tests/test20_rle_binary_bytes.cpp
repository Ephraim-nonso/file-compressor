#include <catch2/catch_all.hpp>
#include "RLECompression.h"

#include <vector>

TEST_CASE("RLE roundtrips binary bytes including 0x00 and 0xFF", "[rle][binary]") {
    RLECompression rle;

    // Construct a deterministic binary payload with repeats.
    std::vector<char> input;
    input.reserve(1024);

    // 0x00 repeated
    input.insert(input.end(), 50, static_cast<char>(0x00));
    // 0xFF repeated
    input.insert(input.end(), 60, static_cast<char>(0xFF));
    // A ramp pattern (mostly non-compressible)
    for (int i = 0; i < 256; i++) {
        input.push_back(static_cast<char>(i));
    }
    // Some more repeats
    input.insert(input.end(), 80, static_cast<char>('Z'));

    auto compressed = rle.compress(input);
    auto decompressed = rle.decompress(compressed);
    REQUIRE(decompressed == input);
}


