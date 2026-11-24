#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    char src_path[256], dst_path[256];
    long long buf_size;
    if (scanf("%255s %255s %lld", src_path, dst_path, &buf_size) != 3) {
        return 0;
    }

    if (buf_size <= 0) {
        printf("-1\n");
        return 0;
    }

    int src_fd = open(src_path, O_RDONLY);
    if (src_fd < 0) {
        printf("-1\n");
        return 0;
    }

    off_t file_size = lseek(src_fd, 0, SEEK_END);
    if (file_size == (off_t)-1) {
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

    if (file_size == 0) {
        close(src_fd);
        close(dst_fd);
        printf("0\n");
        return 0;
    }

    unsigned char *buf = (unsigned char *)malloc((size_t)buf_size);
    if (!buf) {
        close(src_fd);
        close(dst_fd);
        printf("-1\n");
        return 0;
    }

    off_t pos = file_size;
    long long total_written = 0;

    while (pos > 0) {
        size_t chunk_size = (pos < (off_t)buf_size) ? (size_t)pos : (size_t)buf_size;
        pos -= chunk_size;

        if (lseek(src_fd, pos, SEEK_SET) == (off_t)-1) {
            free(buf);
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }

        ssize_t n = read(src_fd, buf, chunk_size);
        if (n != (ssize_t)chunk_size) {
            free(buf);
            close(src_fd);
            close(dst_fd);
            printf("-1\n");
            return 0;
        }

        // Đảo ngược mảng byte trong bộ đệm
        for (ssize_t i = 0, j = n - 1; i < j; i++, j--) {
            unsigned char temp = buf[i];
            buf[i] = buf[j];
            buf[j] = temp;
        }

        // Ghi bộ đệm đã đảo ngược vào tệp đích (xử lý short writes)
        ssize_t written = 0;
        while (written < n) {
            ssize_t w = write(dst_fd, buf + written, (size_t)(n - written));
            if (w <= 0) {
                free(buf);
                close(src_fd);
                close(dst_fd);
                printf("-1\n");
                return 0;
            }
            written += w;
        }

        total_written += written;
    }

    free(buf);
    close(src_fd);
    close(dst_fd);

    printf("%lld\n", total_written);
    return 0;
}
