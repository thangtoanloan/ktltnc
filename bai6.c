#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
    char file_path[256];
    long long offset;
    static char new_data[100005];

    if (scanf("%255s %lld %100000s", file_path, &offset, new_data) != 3) {
        return 0;
    }

    if (offset < 0) {
        printf("-1\n");
        return 0;
    }

    int fd = open(file_path, O_WRONLY);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    if (lseek(fd, (off_t)offset, SEEK_SET) == (off_t)-1) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    size_t len = strlen(new_data);
    size_t written = 0;
    while (written < len) {
        ssize_t w = write(fd, new_data + written, len - written);
        if (w <= 0) {
            close(fd);
            printf("-1\n");
            return 0;
        }
        written += (size_t)w;
    }

    struct stat st;
    if (fstat(fd, &st) < 0) {
        close(fd);
        printf("-1\n");
        return 0;
    }

    close(fd);
    printf("%lld\n", (long long)st.st_size);
    return 0;
}
