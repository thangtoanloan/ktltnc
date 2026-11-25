#include <pthread.h>
#include <stdio.h>

int n, T;
long long A[100000], B[100000];
long long partial[16];

typedef struct {
    int tid;
    int lo, hi;   /* đoạn [lo, hi), nửa mở */
} Task;

void *worker(void *arg) {
    Task *t = (Task *)arg;
    long long local = 0;
    for (int i = t->lo; i < t->hi; i++) {
        local += A[i] * B[i];
    }
    partial[t->tid] = local;
    return NULL;
}

int main(void) {
    if (scanf("%d %d", &n, &T) != 2) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &A[i]) != 1) return 1;
    }
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &B[i]) != 1) return 1;
    }

    int P = (n < T) ? n : T;     /* min(n, T) luồng */
    int base = n / P, rem = n % P;
    pthread_t th[16];
    Task tasks[16];
    int created = 0;
    int cur = 0;

    for (int i = 0; i < P; i++) {
        int sz = base + (i < rem ? 1 : 0);
        tasks[i].tid = i;
        tasks[i].lo = cur;
        tasks[i].hi = cur + sz;
        cur += sz;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);
    if (created != P) return 1;

    long long total = 0;
    for (int i = 0; i < P; i++) total += partial[i];

    printf("%lld\n", total);
    return 0;
}
