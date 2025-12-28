#include <catch2/catch_all.hpp>
#include "FileHandler.h"
#include <cstdio>
#include <cstdlib>
TEST_CASE("FileHandler writes and reads back random data", "[file]") {
    std::string filename = "tmp_" + std::to_string(rand()) + ".bin";
    std::vector<char> data(100);
    std::generate(data.begin(), data.end(), [](){ return rand()%256; });
    FileHandler::writeFile(filename, data);
    auto result = FileHandler::readFile(filename);
    REQUIRE(result == data);
    std::remove(filename.c_str());
}
