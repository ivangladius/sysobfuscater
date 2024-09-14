#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("target.c", O_RDONLY);
    if (fd == -1) return 1;

    char buf[1024];
    ssize_t bytes;
    while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
        write(STDOUT_FILENO, buf, bytes);
    }

    close(fd);
    return 0;
}




