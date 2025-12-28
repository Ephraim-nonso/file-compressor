#include <catch2/catch_all.hpp>
#include "AdaptiveCompression.h"

TEST_CASE("AdaptiveCompression falls back to identity when RLE expands data", "[adaptive]") {
    AdaptiveCompression algo;

    // Alternating bytes => RLE would encode as (byte,count=1) pairs (expands ~2x),
    // so adaptive should store raw payload with a 1-byte header.
    std::vector<char> input;
    input.reserve(100);
    for (int i = 0; i < 100; i++) {
        input.push_back((i % 2) == 0 ? 'A' : 'B');
    }

    auto compressed = algo.compress(input);
    REQUIRE(!compressed.empty());
    REQUIRE(compressed[0] == 'I');
    REQUIRE(compressed.size() == input.size() + 1);

    auto decompressed = algo.decompress(compressed);
    REQUIRE(decompressed == input);
}

TEST_CASE("AdaptiveCompression uses RLE when it helps", "[adaptive]") {
    AdaptiveCompression algo;

    std::vector<char> input(100, 'X'); // highly compressible via RLE
    auto compressed = algo.compress(input);
    REQUIRE(!compressed.empty());
    REQUIRE(compressed[0] == 'R');

    auto decompressed = algo.decompress(compressed);
    REQUIRE(decompressed == input);
}


