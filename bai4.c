#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    char src_path[256], dst_path[256];
    long long offset, max_bytes;
    if (scanf("%255s %255s %lld %lld", src_path, dst_path, &offset, &max_bytes) != 4) {
        return 0;
    }

    if (offset < 0 || max_bytes < 0) {
        printf("-1\n");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    if (lseek(src_fd, (off_t)offset, SEEK_SET) == (off_t)-1) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    int dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        close(src_fd);
        printf("-1\n");
        return 0;
    }

    unsigned char buf[4096];
    long long remaining = max_bytes;
    long long copied = 0;

    while (remaining > 0) {
        size_t to_read = (remaining < (long long)sizeof(buf)) ? (size_t)remaining : sizeof(buf);
        ssize_t n = read(src_fd, buf, to_read);
        if (n < 0) {
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }
        if (n == 0) {
            break;
        }

        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(dst_fd, buf + written, (size_t)(n - written));
            if (w <= 0) {
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            written += w;
        }

        copied += n;
        remaining -= n;
    }

    close(src_fd);
    close(dst_fd);

    printf("%lld\n", copied);
    return 0;
}
