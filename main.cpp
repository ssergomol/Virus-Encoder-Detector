//#include "detector/detector.hpp"
#include "database/db.hpp"
#include <loguru.hpp>

#include "database/file_repo.hpp"
#include "database/models/file.hpp"
//#include "detector/detector.hpp"
#include "encoder/encoder.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

int main(int argc, char** argv) {
    YAML::Node basenode = YAML::LoadFile("config.yaml");
    // Will also detect verbosity level on command line as -v.
    loguru::init(argc, argv);

    if (basenode["logger"]["all_logs"] && basenode["logger"]["latest_readable"]) {
        std::string all_logs_path = basenode["logger"]["all_logs"].as<std::string>();
        std::string last_readable_path = basenode["logger"]["latest_readable"].as<std::string>();
        // Put every log message in "everything.log":
        loguru::add_file(all_logs_path.c_str(),
                loguru::Append, loguru::Verbosity_MAX);

        // Only log INFO, WARNING, ERROR and FATAL to "latest_readable.log":
        loguru::add_file(last_readable_path.c_str(),
                loguru::Truncate, loguru::Verbosity_INFO);
    } else {
        LOG_F(FATAL, "The logger isn't configured correct in the config.yaml");
        return EXIT_FAILURE;
    }

    // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = 1;

    Storage *database = new Storage();
    if (basenode["database"]["URL"]) {
        database->connect(basenode["database"]["URL"].as<std::string>());
    } else {
        LOG_F(FATAL, "The database URL for database wasn't provided in config");
        return EXIT_FAILURE;
    }

    if (basenode["database"]["init_script"]) {
        database->initDB(basenode["database"]["init_script"].as<std::string>());
    } else {
        LOG_F(FATAL, "The init script for database wasn't provided in config");
        return EXIT_FAILURE;
    }

    std::vector<char> content{'1', '2', '3'};
    File file("/home/ok", content, 3, ::getpid());
    database->File()->insertFile(file);
    
    Detector detector(database);
    detector.startDecoder(argc, argv);
    database->close();
    free(database);
}
