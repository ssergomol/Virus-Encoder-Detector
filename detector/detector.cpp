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
//    std::remove(exePath);
    std::cout << "\n\nRemoved suspicious file: " << exePath << "\n\n";
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
    std::cout << "\n\nRemoved suspicious file: " << exePath << "\n\n";
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
                if (path_len == -1) {
                    std::cerr << "Failed to read link " << procPath << "\n";
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                path_fs = path;

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
                if (metadata->mask & FAN_CLOSE_WRITE) {
                    auto currentTime = ch::system_clock::now();

                    if (access_file[path_fs.string()].first == metadata->pid) {
                        auto timeDiff = ch::duration<double, std::milli>(
                                currentTime - access_file[path_fs.string()].second).count();

                        if (timeDiff < 500) {
                            if (susWrite.contains(access_path[metadata->pid])
                                && ch::duration<double, std::milli>(
                                    currentTime - susWrite[access_path[metadata->pid]]).count() < 500) {
                                eventsCount++;
                                if (eventsCount == SUS_EVENT_NUMB) {
                                    std::cout << "Suspicious process: " << metadata->pid << "\n";
                                    terminate_executable(metadata->pid);
                                }
                            }

                            printf("FAN_CLOSE_WRITE: ");
                            susWrite[access_path[metadata->pid]] = ch::system_clock::now();
                        }

                    }
                }

                std::cout << "File " << path << " PID " << metadata->pid << "\n";

                // Close the file descriptor of the event
                close(metadata->fd);

            }
            metadata = FAN_EVENT_NEXT(metadata, len);
        }
    }
}

int Detector::startDecoder(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [path name]\n";
        exit(EXIT_FAILURE);
    }

    // Init watch queue
    int fan_fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT | FAN_NONBLOCK,
                               O_RDONLY | O_LARGEFILE);

    if (fan_fd == -1) {
        std::cerr << "Failed to init fanotify watch queue\n";
        store.close();
        exit(EXIT_FAILURE);
    }

    // Add dir to watch queue
    if (fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                      FAN_ACCESS_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                      argv[1]) == -1) {
        std::cerr << "Failed to mark file or directory\n";
        store.close();
        exit(EXIT_FAILURE);
    }


    pollfd fds{fan_fd, POLLIN};
    int counter = 0;

    // Wait until event occurs
    while (true) {
        int pollNum = poll(&fds, 1, -1);
        if (pollNum == -1) {
            std::cerr << std::strerror(errno);
            store.close();
            exit(EXIT_FAILURE);
        }
        if (fds.revents & POLLIN) {
            handle_event(fds.fd);
        }
    }

    store.close();
    return 0;
}
