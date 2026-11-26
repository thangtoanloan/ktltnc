#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

char path[256];
int target;

typedef struct {
    long long start, len; /* đoạn [start, start + len) */
    long long count;
    int err;
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    t->count = 0;
    t->err = 0;
    if (t->len == 0) return NULL;

    int fd = open(path, O_RDONLY); /* fd riêng cho từng luồng */
    if (fd < 0) {
        t->err = 1;
        return NULL;
    }

    if (lseek(fd, (off_t)t->start, SEEK_SET) < 0) {
        t->err = 1;
        close(fd);
        return NULL;
    }

    unsigned char buf[65536];
    long long remain = t->len;
    long long local = 0; /* đếm cục bộ, không cần mutex */

    while (remain > 0) {
        size_t want =
            remain < (long long)sizeof(buf) ? (size_t)remain : sizeof(buf);
        ssize_t r = read(fd, buf, want);
        if (r < 0) {
            t->err = 1;
            break;
        }
        if (r == 0) break;
        for (ssize_t i = 0; i < r; i++) {
            if (buf[i] == (unsigned char)target) local++;
        }
        remain -= r;
    }

    close(fd);
    t->count = local;
    return NULL;
}

int main(void) {
    int T;
    if (scanf("%255s %d %d", path, &target, &T) != 3) return 1;

    struct stat st;
    if (stat(path, &st) != 0) {
        printf("-1\n");
        return 0;
    }
    long long size = (long long)st.st_size;
    if (size == 0) {
        printf("0\n");
        return 0;
    }

    int P = (size < T) ? (int)size : T; /* tránh đoạn rỗng */
    long long base = size / P, rem = size % P;

    pthread_t th[8];
    Task tasks[8];
    int created = 0;
    long long cur = 0;

    for (int i = 0; i < P; i++) {
        long long sz = base + (i < rem ? 1 : 0);
        tasks[i].start = cur;
        tasks[i].len = sz;
        cur += sz;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);

    if (created != P) {
        printf("-1\n");
        return 0;
    }

    long long total = 0;
    for (int i = 0; i < P; i++) {
        if (tasks[i].err) {
            printf("-1\n");
            return 0;
        }
        total += tasks[i].count;
    }

    printf("%lld\n", total);
    return 0;
}
