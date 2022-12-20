#include <iostream>
#include "encoder.hpp"

int main(int argc, char **argv) {
    fs::path file = argv[1];
    Encoder encoder;
    encoder.encodeFile(file);
}