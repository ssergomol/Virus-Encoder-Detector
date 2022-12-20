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
#include "detector.hpp"
#include "../database/black_list_repo.hpp"
#include "../database/white_list_repo.hpp"
#include <loguru.hpp>

namespace fs = std::filesystem;
namespace ch = std::chrono;

// Terminate executation of suspicious file and remove the executable
void Detector::terminate_executable(int pid) {
    char exePath[PATH_MAX];
    std::string linkToExe;

    linkToExe = "/proc/" + std::to_string(pid) + "/exe";
    ssize_t len = readlink(linkToExe.c_str(), exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
    }
    kill(pid, SIGKILL);

    // Put executable into the black list
//    if (!this->DB->BlackList()->contains(std::string(exePath))) {
//        this->DB->BlackList()->addExe(std::string(exePath));
//    }

    LOG_F(INFO, "Binary file %s was detected as suspicious and put into the whitelist", exePath);
    LOG_F(INFO, "Suspicious process %d is killed", pid);
}

void Detector::addToDatabase(int pid) {
    char exePath[PATH_MAX];
    std::string linkToExe;

    linkToExe = "/proc/" + std::to_string(pid) + "/exe";
    ssize_t len = readlink(linkToExe.c_str(), exePath, sizeof(exePath) - 1);
    if (len != -1) {
        exePath[len] = '\0';
    }

    File file((std::string(exePath)));
    DB->File()->insertFile(file);
    LOG_F(INFO, "File %s is added to the database as modified", exePath);
}

void Detector::handle_event(int fan_fd) {
    fanotify_event_metadata *metadata;
    fanotify_response response;
    fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    ssize_t path_len;
    char procPath[PATH_MAX];
    fs::path path_fs;

    while (true) {
        len = read(fan_fd, buf, sizeof(buf));
        CHECK_F(!(len == -1 && errno != EAGAIN), "Failed to read file: %s", strerror(errno));
        if (len == -1 && errno != EAGAIN) {
            std::cerr << "Failed to read file\n";
            exit(EXIT_FAILURE);
        }

        if (len <= 0) {
            break;
        }

        metadata = buf;

        while (FAN_EVENT_OK(metadata, len)) {
            if (metadata->fd >= 0) {

                // Retrieve pathname of the accessed file
                snprintf(procPath, sizeof(procPath),
                         "/proc/self/fd/%d", metadata->fd);

                path_len = readlink(procPath, path, sizeof(path) - 1);
                CHECK_F(path_len != -1, "Failed to read link %s: %s", procPath, strerror(errno));
                if (path_len == -1) {
                    std::cerr << "Failed to read link " << procPath << "\n";
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                path_fs = path;

                LOG_F(INFO, "File %s is accessed by process %d\n", path, metadata->pid);
                // If accessed file in the white list, then skip
//                if (this->DB->WhiteList()->contains(std::string(path))) {
//                    continue;
//                }
//                std::cout << path << " not in white list, continue\n";


                // Send response if some process intends to read the file
                if (metadata->mask & FAN_ACCESS_PERM) {
                    printf("FAN_ACCESS_PERM: ");
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;

                    write(fan_fd, &response, sizeof(response));


                    if (!access_path.contains(metadata->pid)) {
                        std::string oldPath = access_path[metadata->pid];
                        auto res = std::mismatch(oldPath.begin(), oldPath.end(),
                                                 path_fs.string().begin());

                        if (res.first != oldPath.end()) {
                            access_path[metadata->pid] = path_fs.parent_path().string();
                        }
                    }
                    auto pair = std::pair < unsigned, ch::time_point<ch::system_clock>>
                    (metadata->pid,
                            ch::system_clock::now());
                    access_file[path_fs.string()] = pair;
                }

                // Send response if some process intends to write to the file
                // and add modified file to database
                if (metadata->mask & FAN_CLOSE_WRITE) {
                    auto currentTime = ch::system_clock::now();
//                    this->addToDatabase(metadata->pid);

                    if (access_file[path_fs.string()].first == metadata->pid) {
                        auto timeDiff = ch::duration<double, std::milli>(
                                currentTime - access_file[path_fs.string()].second).count();

                        if (timeDiff < 500) {
                            if (susWrite.contains(access_path[metadata->pid])
                                && ch::duration<double, std::milli>(
                                    currentTime - susWrite[access_path[metadata->pid]]).count() < 500) {
                                eventsCount++;
                                if (eventsCount == SUS_EVENT_NUMB) {
                                    terminate_executable(metadata->pid);
                                }
                            }
                            printf("FAN_CLOSE_WRITE: ");
                            susWrite[access_path[metadata->pid]] = ch::system_clock::now();
                        }

                    }
                }

                // Close the file descriptor of the event
                close(metadata->fd);

            }
            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }
}

int Detector::startDecoder(int argc, char **argv) {
    CHECK_F(argc == 2, "Usage: %s [path name]", argv[0]);

    // Init watch queue
    int fan_fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT | FAN_NONBLOCK,
                               O_RDONLY | O_LARGEFILE);

    if (fan_fd == -1) {
        LOG_F(FATAL, "Failed to init fanotify watch queue: %s", strerror(errno));
        DB->close();
        return EXIT_FAILURE;
    }

    // Add dir to watch queue
    if (fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                      FAN_ACCESS_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                      argv[1]) == -1) {

        LOG_F(FATAL, "Failed to mark file or directory: %s", strerror(errno));
        DB->close();
        return EXIT_FAILURE;
    }


    pollfd fds{fan_fd, POLLIN};
    int counter = 0;

    // Wait until event occurs
    while (true) {
        int pollNum = poll(&fds, 1, -1);
        if (pollNum == -1) {

            LOG_F(FATAL, strerror(errno));
            DB->close();
            return EXIT_FAILURE;
        }
        if (fds.revents & POLLIN) {
            handle_event(fds.fd);
        }
    }

    DB->close();
    return EXIT_SUCCESS;
}
