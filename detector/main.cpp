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


namespace fs = std::filesystem;
namespace ch = std::chrono;
const unsigned int SUS_EVENT_NUMB = 2;


std::unordered_map<unsigned, std::string> access_path;
std::unordered_map<std::string, std::pair<unsigned, ch::time_point<ch::system_clock>>> access_file;
std::unordered_map<std::string, ch::time_point<ch::system_clock>> susWrite;
int eventsCount = 0;


void terminate_executable(pid_t pid) {

}

void handle_event(int fan_fd) {
    fanotify_event_metadata *metadata;
    fanotify_response response;
    fanotify_event_metadata buf[200];
    ssize_t len;
    char path[PATH_MAX];
    ssize_t path_len;
    char procfd_path[PATH_MAX];


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

                /* Retrieve and print pathname of the accessed file. */
                snprintf(procfd_path, sizeof(procfd_path),
                         "/proc/self/fd/%d", metadata->fd);


                if (metadata->mask & FAN_ACCESS_PERM) {
                    printf("FAN_ACCESS_PERM: ");
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write(fan_fd, &response, sizeof(response));


//                    if (!access_path.contains(metadata->pid)) {
//                        std::string oldPath = access_path[metadata->pid];
//                        auto res = std::mismatch(oldPath.begin(), oldPath.end(),
//                                                 path_fs.string().begin());
//
//                        if (res.first != oldPath.end()) {
//                            access_path.insert(metadata->pid, path_fs.parent_path().string());
//                        }
//                    }
//                    auto pair = std::pair<unsigned, ch::time_point<ch::system_clock>>(metadata->pid,
//                                                                                      ch::system_clock::now());
//                    access_file.insert(path_fs.string(), pair);
                }

                if (metadata->mask & FAN_CLOSE_WRITE) {
//                    auto currentTime = ch::system_clock::now();
//
//                    if (access_file[path_fs.string()].first == metadata->pid) {
//                        auto timeDiff = ch::duration<double, std::milli>(
//                                 currentTime - access_file[path_fs.string()].second).count();
//
//                        if (timeDiff < 500) {
//                            if (susWrite.contains(access_path[metadata->pid])
//                            && ch::duration<double, std::milli>(
//                                    currentTime - susWrite[access_path[metadata->pid]]).count() < 500) {
//                                eventsCount++;
//                                if (eventsCount == SUS_EVENT_NUMB) {
//                                    std::cout << "Suspicious process: " << metadata->pid << "\n";
//                                    // terminate process
//                                }
//                            }

                    printf("FAN_CLOSE_WRITE: ");
//                            susWrite.insert(access_path[metadata->pid], ch::system_clock::now());
//                        }
//
//                    }
                }

                path_len = readlink(procfd_path, path, sizeof(path) - 1);
                if (path_len == -1) {
                    std::cerr << "Failed to read link " << procfd_path << "\n";
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
                fs::path path_fs = path;

                printf("File %s", path);
                printf(" PID %d", metadata->pid);
                printf("\n");

                /* Close the file descriptor of the event. */
                close(metadata->fd);

            }
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " [path name]\n";
        exit(EXIT_FAILURE);
    }

    int fan_fd = fanotify_init(FAN_CLOEXEC | FAN_CLASS_PRE_CONTENT | FAN_NONBLOCK,
                               O_RDONLY | O_LARGEFILE);

    if (fan_fd == -1) {
        std::cerr << "Failed to init fanotify watch queue\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Check 1\n";

    if (fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                      FAN_ACCESS_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                      argv[1]) == -1) {
        std::cerr << "Failed to mark file or directory\n";
        exit(EXIT_FAILURE);
    }
    std::cout << "Check 2\n";

    pollfd fds{fan_fd, POLLIN};
    int counter = 0;
    while (true) {
        int pollNum = poll(&fds, 1, -1);
        if (pollNum == -1) {
            std::cerr << std::strerror(errno);
            exit(EXIT_FAILURE);
        }
        if (fds.revents & POLLIN) {
            handle_event(fds.fd);
        }
    }

    return 0;
}
