#include <catch2/catch_all.hpp>
#include "RLECompression.h"
#include <thread>
#include <vector>
#include <cstdlib>
TEST_CASE("Multiple threads compress different data", "[thread]") {
    RLECompression rle;
    std::vector<std::vector<char>> inputs(3, std::vector<char>(200));
    for (auto& v : inputs) std::generate(v.begin(), v.end(), [](){ return 'A' + rand()%4; });
    std::vector<std::vector<char>> outputs(3);
    auto worker = [&](int i) { outputs[i] = rle.compress(inputs[i]); };
    std::thread t1(worker,0), t2(worker,1), t3(worker,2);
    t1.join(); t2.join(); t3.join();
    for (int i=0;i<3;i++) {
        auto roundtrip = rle.decompress(outputs[i]);
        REQUIRE(roundtrip == inputs[i]);
    }
}
