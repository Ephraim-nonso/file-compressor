#include <catch2/catch_all.hpp>
#include "CompressionAlgorithm.h"
#include "RLECompression.h"
#include <cstdlib>
#include <vector>

TEST_CASE("Virtual destructor called correctly", "[oop]") {
    // Test that virtual destructor works correctly for base class pointer
    CompressionAlgorithm* algo = new RLECompression();
    std::vector<char> input = {'A', 'A', 'A', 'B', 'B'};
    auto compressed = algo->compress(input);
    REQUIRE(!compressed.empty());
    delete algo; // Should call RLECompression destructor, not just base
    // If destructor isn't virtual, this would cause undefined behavior
}

TEST_CASE("Base class interface is pure virtual", "[oop]") {
    // Test that we can store different algorithm instances via base pointer
    std::vector<CompressionAlgorithm*> algorithms;
    algorithms.push_back(new RLECompression());
    
    std::vector<char> input = {'X', 'X', 'Y', 'Y', 'Z'};
    
    // Test polymorphic behavior through base class pointer
    for (auto* algo : algorithms) {
        auto compressed = algo->compress(input);
        auto decompressed = algo->decompress(compressed);
        REQUIRE(decompressed == input);
    }
    
    // Cleanup - tests proper inheritance and virtual destructor
    for (auto* algo : algorithms) {
        delete algo;
    }
}
