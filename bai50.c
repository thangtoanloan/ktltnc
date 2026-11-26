#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

int Q, T;
int nq[101];
long long *price[101];     /* price[q][1..nq[q]] */
long long ans[101];

typedef struct {
    int lo, hi;   /* dải bài [lo, hi), nửa mở */
} Task;

void *worker(void *arg) {
    Task *t = (Task *)arg;
    for (int q = t->lo; q < t->hi; q++) {
        int n = nq[q];
        long long *dp = (long long *)calloc(n + 1, sizeof(long long));  /* DP cục bộ */
        if (!dp) { ans[q] = 0; continue; }
        for (int len = 1; len <= n; len++) {
            long long best = 0;
            for (int i = 1; i <= len; i++) {
                long long cand = price[q][i] + dp[len - i];
                if (cand > best) best = cand;
            }
            dp[len] = best;
        }
        ans[q] = dp[n];
        free(dp);
    }
    return NULL;
}

int main(void) {
    char src[256], dst[256];
    if (scanf("%255s %255s %d", src, dst, &T) != 3) return 1;

    int fd = open(src, O_RDONLY);
    if (fd < 0) {
        printf("-1\n");
        return 0;
    }

    /* đọc toàn bộ tệp */
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

    /* tự phân tích số nguyên không âm */
    size_t pos = 0;
    #define NEXT(var)                                                      \
        do {                                                               \
            while (pos < len && (buf[pos] < '0' || buf[pos] > '9')) pos++; \
            long long x_ = 0;                                              \
            while (pos < len && buf[pos] >= '0' && buf[pos] <= '9')        \
                x_ = x_ * 10 + (buf[pos++] - '0');                         \
            var = x_;                                                      \
        } while (0)

    long long tmpv;
    NEXT(tmpv);
    Q = (int)tmpv;
    for (int q = 0; q < Q; q++) {
        NEXT(tmpv);
        nq[q] = (int)tmpv;
        price[q] = (long long *)malloc(sizeof(long long) * (nq[q] + 1));
        if (!price[q]) return 1;
        for (int i = 1; i <= nq[q]; i++) NEXT(price[q][i]);
    }
    free(buf);

    int P = (Q < T) ? Q : T;     /* min(Q, T) luồng */
    int base = Q / P, rem = Q % P;
    pthread_t th[8];
    Task tasks[8];
    int created = 0, cur = 0;

    for (int i = 0; i < P; i++) {
        int sz = base + (i < rem ? 1 : 0);
        tasks[i].lo = cur;
        tasks[i].hi = cur + sz;
        cur += sz;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) break;
        created++;
    }
    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);
    if (created != P) return 1;

    /* ghi Q dòng vào dst_path bằng write() */
    char out[16384];
    size_t olen = 0;
    long long total = 0;
    for (int q = 0; q < Q; q++) {
        olen += (size_t)snprintf(out + olen, sizeof(out) - olen, "%lld\n", ans[q]);
        total += ans[q];
    }

    int fo = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fo >= 0) {
        size_t off = 0;
        while (off < olen) {
            ssize_t w = write(fo, out + off, olen - off);
            if (w <= 0) break;
            off += (size_t)w;
        }
        close(fo);
    }

    for (int q = 0; q < Q; q++) free(price[q]);
    printf("%lld\n", total);
    return 0;
}
