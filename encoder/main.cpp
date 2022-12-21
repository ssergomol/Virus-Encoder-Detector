#include <iostream>
#include "encoder.hpp"
namespace fs = std::filesystem;

int main(int argc, char **argv) {
    fs::path file = argv[1];
    Encoder encoder;
//    Encoder::encodeFile(fs::absolute(file));
    encoder.start_encoder(argc, argv);
}