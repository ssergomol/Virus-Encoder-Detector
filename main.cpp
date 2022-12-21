#include "detector/detector.hpp"
#include "database/db.hpp"
#include <loguru.hpp>

#include "database/file_repo.hpp"
#include "database/models/file.hpp"
#include "database/black_list_repo.hpp"
#include "database/white_list_repo.hpp"
//#include "detector/detector.hpp"
#include "encoder/encoder.hpp"

#include <iostream>
#include <vector>
#include <unistd.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/yaml.h>

namespace fs = std::filesystem;


int main(int argc, char** argv) {
    sqlite3_shutdown();
    sqlite3_config(SQLITE_CONFIG_SERIALIZED);
    sqlite3_initialize();

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
        LOG_F(FATAL, "The logger isn't configured correctly in the config.yaml");
        return EXIT_FAILURE;
    }

    // Only show most relevant things on stderr:
    loguru::g_stderr_verbosity = 1;

//    Storage *database = new Storage();
//    if (basenode["database"]["URL"]) {
//        database->connect(basenode["database"]["URL"].as<std::string>());
//    } else {
//        LOG_F(FATAL, "The database URL for database wasn't provided in config");
//        return EXIT_FAILURE;
//    }
//
//    if (basenode["database"]["init_script"]) {
//        database->initDB(basenode["database"]["init_script"].as<std::string>());
//    } else {
//        LOG_F(FATAL, "The init script for database wasn't provided in config");
//        return EXIT_FAILURE;
//    }

//    std::vector<char> content{'1', '2', '3'};
//    std::string filePath = "/home/ok";
//    File file(filePath, content, 3, ::getpid());
//    database->File()->insertFile(file);
//    if (database->File()->contains(filePath)) {
//        LOG_F(INFO, "File %s is added to the database as modified", filePath.c_str());
//    } else {
//        LOG_F(INFO, "File %s is already in database", filePath.c_str());
//    }

//    database->WhiteList()->addExe("/usr/test");
//    database->BlackList()->addExe("/usr/test/for/black/list");

//    if (database->WhiteList()->contains("/usr/test")) {
//        std::cout << "/usr/test is in the white list\n";
//    } else {
//        std::cout << "/usr/test is not in the white list\n";
//    }
//
//    if (database->BlackList()->contains("/usr/test/for/black/list")) {
//        std::cout << "/usr/test/for/black/list is in the black list\n";
//    } else {
//        std::cout << "/usr/test/for/black/list is not in the black list\n";
//    }

//    Detector detector(database);
//    detector.terminate_executable(10);

//    std::string path = "/home/ssergomol/Repos/Virus-Encoder-Detector/encoder/encoder";
//    const char* path = "/home/ssergomol/Repos/Virus-Encoder-Detector/encoder/encoder";;
//    File file(path,  ::getpid());
//    database->File()->insertFile(file);

//
//    Storage* DB = new Storage();
//    DB->connect("detector.db");
//    DB->initDB("database/init_db.sql");
//    DB->File()->insertFile(File("/home/ssergomol/hw", 10));
//    DB->File()->recoverFiles(10);
//    DB->File()->recoverFiles(10);
//    fs::path filePath = "../test/file.txt";
//    Encoder::encodeFile(filePath);
    Detector detector;
    detector.startDecoder(argc, argv);
//    database->close();
//    free(database);
}
