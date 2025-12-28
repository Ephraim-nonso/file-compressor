#include <catch2/catch_all.hpp>
#include "CompressionAlgorithm.h"
#include "IdentityCompression.h"

TEST_CASE("IdentityCompression behaves correctly via base-class pointer", "[oop][identity]") {
    CompressionAlgorithm* algo = new IdentityCompression();

    std::vector<char> input = {'\0', 'A', 'B', static_cast<char>(255), '\0'};
    auto compressed = algo->compress(input);
    auto decompressed = algo->decompress(compressed);

    REQUIRE(decompressed == input);
    delete algo;
}


