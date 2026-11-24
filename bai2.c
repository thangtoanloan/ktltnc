#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

int main(void) {
    char src_path[256], dst_path[256];
    if (scanf("%255s %255s", src_path, dst_path) != 2) {
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (dst_fd < 0) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    unsigned char buf[4096];
    ssize_t n;
    while ((n = read(src_fd, buf, sizeof(buf))) > 0) {
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(dst_fd, buf + written, n - written);
            if (w <= 0) {
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            written += w;
        }
    }

    if (n < 0) {
        close(src_fd);
        close(dst_fd);
        printf("-1\n");
        return 0;
    }

    struct stat st;
    if (fstat(dst_fd, &st) < 0) {
        close(src_fd);
        close(dst_fd);
        printf("-1\n");
        return 0;
    }

    close(src_fd);
    close(dst_fd);

    printf("%lld\n", (long long)st.st_size);
    return 0;
}
