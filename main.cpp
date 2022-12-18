//#include "detector/detector.hpp"
#include "database/db.hpp"
#include <loguru.hpp>

#include "database/file_repo.hpp"
#include "database/models/file.hpp"
#include "detector/detector.hpp"
#include "encoder/encoder.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>

int main(int argc, char** argv) {
    // Will also detect verbosity level on command line as -v.
    loguru::init(argc, argv);
    // Put every log message in "everything.log":
    loguru::add_file("logs/detector_all.log", loguru::Append, loguru::Verbosity_MAX);
    // Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
    loguru::add_file("logs/detector_latest_readable.log", loguru::Truncate, loguru::Verbosity_INFO);
    // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = 1;

    Storage *database = new Storage();
    database->connect();
    database->initDB("database/init_db.sql");
    std::vector<char> content{'1', '2', '3'};
    File file("/home/ok", content, 3, ::getpid());
    database->File()->insertFile(file);

    Detector detector(database);
    detector.startDecoder(argc, argv);
    database->close();
    free(database);
}
