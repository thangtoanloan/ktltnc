#include <pthread.h>
#include <stdio.h>

int N, BT, T;
long long A[200][200], B[200][200], C[200][200];

typedef struct {
    int lo, hi; /* dải hàng [lo, hi), nửa mở */
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    for (int kk = 0; kk < N; kk += BT) {
        int kmax = (kk + BT < N) ? kk + BT : N;
        for (int jj = 0; jj < N; jj += BT) {
            int jmax = (jj + BT < N) ? jj + BT : N;
            for (int i = t->lo; i < t->hi; i++) {
                for (int k = kk; k < kmax; k++) {
                    long long aik = A[i][k];
                    for (int j = jj; j < jmax; j++) {
                        C[i][j] += aik * B[k][j];
                    }
                }
            }
        }
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d", &N, &BT, &T) != 3) return 1;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (scanf("%lld", &A[i][j]) != 1) return 1;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (scanf("%lld", &B[i][j]) != 1) return 1;

    int P = (N < T) ? N : T; /* min(N, T) luồng */
    int base = N / P, rem = N % P;
    pthread_t th[16];
    Task tasks[16];
    int created = 0;
    int cur = 0;

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

    for (int i = 0; i < N; i++) {
        long long s = 0;
        for (int j = 0; j < N; j++) s += C[i][j];
        if (i > 0) printf(" ");
        printf("%lld", s);
    }
    printf("\n");
    return 0;
}
