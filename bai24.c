#include <pthread.h>
#include <stdio.h>

int M, T, Q;
long long bal[64];
pthread_mutex_t acc_mtx[64];

int U[5000], V[5000];
long long AM[5000];

int valid_count = 0;
pthread_mutex_t cnt_mtx = PTHREAD_MUTEX_INITIALIZER;

void* worker(void* arg) {
    int t = *(int*)arg;

    for (int j = t; j < Q; j += T) {
        int u = U[j], v = V[j];
        long long amt = AM[j];

        /* giao dịch không hợp lệ thì bỏ qua */
        if (u < 0 || u >= M || v < 0 || v >= M || u == v || amt <= 0) continue;

        int lo = u < v ? u : v;
        int hi = u < v ? v : u;

        pthread_mutex_lock(&acc_mtx[lo]); /* khóa min trước */
        pthread_mutex_lock(&acc_mtx[hi]); /* rồi max */

        bal[u] -= amt;
        bal[v] += amt;

        pthread_mutex_unlock(&acc_mtx[hi]);
        pthread_mutex_unlock(&acc_mtx[lo]);

        pthread_mutex_lock(&cnt_mtx);
        valid_count++;
        pthread_mutex_unlock(&cnt_mtx);
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d", &M, &T, &Q) != 3) return 1;
    for (int i = 0; i < M; i++) {
        if (scanf("%lld", &bal[i]) != 1) return 1;
    }
    for (int j = 0; j < Q; j++) {
        if (scanf("%d %d %lld", &U[j], &V[j], &AM[j]) != 3) return 1;
    }

    for (int i = 0; i < M; i++) pthread_mutex_init(&acc_mtx[i], NULL);

    pthread_t th[8];
    int id[8];
    int created = 0;
    for (int i = 0; i < T; i++) {
        id[i] = i;
        if (pthread_create(&th[i], NULL, worker, &id[i]) != 0) break;
        created++;
    }
    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);
    if (created != T) return 1;

    printf("%d\n", valid_count);
    for (int i = 0; i < M; i++) {
        if (i > 0) printf(" ");
        printf("%lld", bal[i]);
    }
    printf("\n");
    return 0;
}
