#include <pthread.h>
#include <stdio.h>

int M, K, N, T;
long long A[100][100], B[100][100], C[100][100];

typedef struct {
    int lo, hi; /* dải hàng [lo, hi), nửa mở */
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    for (int i = t->lo; i < t->hi; i++) {
        for (int k = 0; k < K; k++) {
            long long aik = A[i][k];
            for (int j = 0; j < N; j++) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d %d", &M, &K, &N, &T) != 4) return 1;
    for (int i = 0; i < M; i++)
        for (int k = 0; k < K; k++)
            if (scanf("%lld", &A[i][k]) != 1) return 1;
    for (int k = 0; k < K; k++)
        for (int j = 0; j < N; j++)
            if (scanf("%lld", &B[k][j]) != 1) return 1;

    int P = (M < T) ? M : T; /* min(M, T) luồng */
    int base = M / P, rem = M % P;
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

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (j > 0) printf(" ");
            printf("%lld", C[i][j]);
        }
        printf("\n");
    }
    return 0;
}
