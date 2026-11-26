#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

long long dp[10001];

int main(void) {
    char src[256], dst[256];
    if (scanf("%255s %255s", src, dst) != 2) return 1;

    int fd = open(src, O_RDONLY);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    /* đọc toàn bộ tệp vào bộ đệm tăng dần */
    size_t cap = 1 << 16, len = 0;
    char *buf = (char *)malloc(cap + 1);
    if (!buf) { close(fd); return 1; }
    ssize_t r;
    while ((r = read(fd, buf + len, cap - len)) > 0) {
        len += (size_t)r;
        if (len == cap) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap + 1);
            if (!tmp) { free(buf); close(fd); return 1; }
            buf = tmp;
        }
    }
    close(fd);
    buf[len] = '\0';

    /* tự phân tích số nguyên (không âm theo ràng buộc) */
    size_t pos = 0;
    #define NEXT(var)                                                     \
        do {                                                              \
            while (pos < len && (buf[pos] < '0' || buf[pos] > '9')) pos++; \
            long long x_ = 0;                                             \
            while (pos < len && buf[pos] >= '0' && buf[pos] <= '9')       \
                x_ = x_ * 10 + (buf[pos++] - '0');                        \
            var = x_;                                                     \
        } while (0)

    long long n, W;
    NEXT(n);
    NEXT(W);

    for (long long i = 0; i < n; i++) {
        long long w, v;
        NEXT(w);
        NEXT(v);
        for (long long j = W; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) dp[j] = dp[j - w] + v;
        }
    }
    free(buf);

    long long ans = dp[W];

    /* ghi "<ans>\n" vào dst_path bằng write() */
    char out[32];
    int olen = snprintf(out, sizeof(out), "%lld\n", ans);

    int fo = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fo >= 0) {
        ssize_t off = 0;
        while (off < olen) {
            ssize_t w = write(fo, out + off, (size_t)(olen - off));
            if (w <= 0) break;
            off += w;
        }
        close(fo);
    }

    printf("%lld\n", ans);
    return 0;
}
