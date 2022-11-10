#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sys/poll.h>
#include <unistd.h>
#include <sys/fanotify.h>

namespace fs = std::filesystem;

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

                if (metadata->mask & FAN_ACCESS_PERM) {
                    printf("FAN_ACCESS_PERM: ");
                    response.fd = metadata->fd;
                    response.response = FAN_ALLOW;
                    write(fan_fd, &response, sizeof(response));
                }

                if (metadata->mask & FAN_CLOSE_WRITE) {
                    printf("FAN_CLOSE_WRITE: ");
                }

                /* Retrieve and print pathname of the accessed file. */
                snprintf(procfd_path, sizeof(procfd_path),
                         "/proc/self/fd/%d", metadata->fd);
                path_len = readlink(procfd_path, path,
                                    sizeof(path) - 1);

                if (path_len == -1) {
                    std::cerr << "Failed to read link " << procfd_path << "\n";
                    exit(EXIT_FAILURE);
                }

                path[path_len] = '\0';
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

    if (fanotify_mark(fan_fd, FAN_MARK_ADD | FAN_MARK_MOUNT,
                      FAN_ACCESS_PERM | FAN_CLOSE_WRITE, AT_FDCWD,
                      argv[1]) == -1) {
        std::cerr << "Failed to mark file or directory\n";
        exit(EXIT_FAILURE);
    }

    pollfd fds{fan_fd, POLLIN};
    while (true) {
        if (fds.revents & POLLIN) {
            handle_event(fds.fd);
        }
    }

    return 0;
}
