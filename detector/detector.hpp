#ifndef VIRUS_ENCODER_DETECTOR_DETECTOR_HPP
#define VIRUS_ENCODER_DETECTOR_DETECTOR_HPP
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sys/poll.h>
#include <unistd.h>
#include <unordered_map>
#include <sys/fanotify.h>
#include <chrono>
#include <ctime>
#include <utility>
#include <filesystem>
#include <cstring>
#include <string>
#include <csignal>
#include <sqlite3.h>
#include "../database/db.hpp"
#include "../database/file_repo.hpp"


namespace fs = std::filesystem;
namespace ch = std::chrono;

class Detector {
    Storage *DB;

    const unsigned int SUS_EVENT_NUMB = 2;
    // access_path map for each process tracks the parent subdirectory where
    // this process changes some files. It updates only in case if changing file is not
    // in folder which is a child for chosen parent directory
    std::unordered_map<int, std::string> access_path;

    // access_file map tracks the modified time and the process which made this modification
    // for each modifying file
    std::unordered_map <std::string,
    std::pair<int, ch::time_point < ch::system_clock>>> access_file;

    // susWrite keeps track of last suspicious behavior of certain files
    std::unordered_map <std::string, ch::time_point<ch::system_clock>> susWrite;

    int eventsCount = 0;

public:
    Detector(Storage *database) : DB(database) {}

    // Terminate executation of suspicious file and remove the executable
    void terminate_executable(int pid);

    void addToDatabase(int pid);

    /*
     * The file is considered to be suspicious if the following conditions
     * are met:
     *  - The file was read by the same process less than 0.5 seconds ago.
     *  - The same process has written to this subtree of directories
     *  less than 0.5 seconds ago.
     *  - If such suspicious behaviour occurred SUS_EVENT_NUMB times for the same
     *  process, then the process is considered to be encoder virus.
     *  In such case process will be killed and the executable deleted.
     */
    void handle_event(int fan_fd);

    int startDecoder(int argc, char **argv);
};

#endif //VIRUS_ENCODER_DETECTOR_DETECTOR_HPP
