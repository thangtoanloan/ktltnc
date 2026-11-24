#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    char file_path[256];
    if (scanf("%255s", file_path) != 1) {
        return 0;
    }

    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    unsigned char buf[4096];
    int total_bytes = 0, newline_count = 0, digit_count = 0;
    ssize_t n;

    while ((n = read(fd, buf, sizeof(buf))) > 0) {
        total_bytes += (int)n;
        for (ssize_t i = 0; i < n; i++) {
            if (buf[i] == '\n') {
                newline_count++;
            } else if (buf[i] >= '0' && buf[i] <= '9') {
                digit_count++;
            }
        }
    }

    close(fd);
    if (n < 0) {
        printf("-1\n");
        return 0;
    }

    printf("%d %d %d\n", total_bytes, newline_count, digit_count);
    return 0;
}
