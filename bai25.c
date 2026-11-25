#include <pthread.h>
#include <stdio.h>

long long L, R, d;
int T;
long long counts[16];

typedef struct {
    int tid;
    long long lo, hi; /* đoạn [lo, hi], bao gồm */
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    long long local = 0; /* biến đếm cục bộ */
    for (long long x = t->lo; x <= t->hi; x++) {
        if (x % 2 != 0 && x % d == 0) local++;
    }
    counts[t->tid] = local; /* ghi đúng một lần */
    return NULL;
}

int main(void) {
    if (scanf("%lld %lld %lld %d", &L, &R, &d, &T) != 4) return 1;

    long long len = R - L + 1;
    long long base = len / T, rem = len % T;

    pthread_t th[16];
    Task tasks[16];
    int created = 0;
    long long cur = L;

    for (int i = 0; i < T; i++) {
        long long sz = base + (i < rem ? 1 : 0);
        tasks[i].tid = i;
        tasks[i].lo = cur;
        tasks[i].hi =
            cur + sz - 1; /* sz = 0 thì hi < lo, vòng lặp không chạy */
        cur += sz;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);
    if (created != T) return 1;

    long long total = 0;
    for (int i = 0; i < T; i++) total += counts[i];

    printf("%lld\n", total);
    return 0;
}
