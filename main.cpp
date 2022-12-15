#include "detector/detector.cpp"
#include "database/db.hpp"

int main(int argc, char** argv) {
    Storage database = new Storage();
    database.connect();
    database.initDB("../database/init_db.sql");

    Detector detector(database);
    detector.startDecoder(argc, argv);
    free(database);
}