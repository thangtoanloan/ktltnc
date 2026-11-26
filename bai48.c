#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int k, T;
char names[32][256];
long long file_sum[32];

typedef struct {
    int lo, hi;   /* dải chỉ số tệp [lo, hi), nửa mở */
} Task;

void *worker(void *arg) {
    Task *t = (Task *)arg;
    unsigned char buf[65536];

    for (int idx = t->lo; idx < t->hi; idx++) {
        int fd = open(names[idx], O_RDONLY);
        if (fd < 0) {
            file_sum[idx] = -1;
            continue;
        }

        long long sum = 0;
        ssize_t r;
        while ((r = read(fd, buf, sizeof(buf))) > 0) {
            for (ssize_t i = 0; i < r; i++) sum += buf[i];   /* unsigned char: 0..255 */
        }
        close(fd);

        file_sum[idx] = (r < 0) ? -1 : sum;                  /* lỗi đọc thì coi như không đọc được */
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d", &k, &T) != 2) return 1;
    for (int i = 0; i < k; i++) {
        if (scanf("%255s", names[i]) != 1) return 1;
    }

    int P = (k < T) ? k : T;     /* min(k, T) luồng */
    int base = k / P, rem = k % P;
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

    for (int i = 0; i < k; i++) {
        if (i > 0) printf(" ");
        printf("%lld", file_sum[i]);
    }
    printf("\n");
    return 0;
}
