#include <unistd.h>
#include <stdlib.h>


int main() {
    const char *message = "Hello, World!\n";
    ssize_t bytes_written = write(STDOUT_FILENO, message, 14);
    
    if (bytes_written == -1) {
        return 1;  // Error occurred
    }

    exit(0);
    
    return 0;
}
