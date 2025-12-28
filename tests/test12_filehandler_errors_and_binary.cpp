#include <catch2/catch_all.hpp>
#include "FileHandler.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

TEST_CASE("FileHandler throws on missing file", "[file][error]") {
    std::string filename = "definitely_missing_" + std::to_string(std::rand()) + ".bin";
    REQUIRE_THROWS_AS(FileHandler::readFile(filename), std::runtime_error);
}

TEST_CASE("FileHandler roundtrips binary data including null bytes", "[file][binary]") {
    std::string filename = "tmp_bin_" + std::to_string(std::rand()) + ".bin";

    std::vector<char> data = {
        0, 1, 2, 3,
        0, 0, 0,
        static_cast<char>(255),
        'A', '\0', 'B', '\0', 'C'
    };

    FileHandler::writeFile(filename, data);
    auto result = FileHandler::readFile(filename);
    REQUIRE(result == data);

    std::remove(filename.c_str());
}


