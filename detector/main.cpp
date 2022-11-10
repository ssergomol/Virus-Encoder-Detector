#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sys/poll.h>
#include <sys/fanotify.h>


int handle_event() {

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
    while(true) {
        if (fds.revents & POLLIN) {
            handle_event();
        }
    }





    return 0;
}
