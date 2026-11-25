#include <pthread.h>
#include <stdio.h>

int k;
long long a[16], b[16];

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;
int count = 0;
int generation = 0;

void barrier_wait(void) {
    pthread_mutex_lock(&m);
    int gen = generation;
    count++;
    if (count == k) {
        /* luồng đến cuối cùng: reset và giải phóng tất cả */
        count = 0;
        generation++;
        pthread_cond_broadcast(&cv);
    } else {
        while (gen == generation) {
            pthread_cond_wait(&cv, &m);
        }
    }
    pthread_mutex_unlock(&m);
}

void *worker(void *arg) {
    int i = *(int *)arg;

    /* Pha 1 */
    a[i] = a[i] * a[i];

    barrier_wait();

    /* Pha 2 */
    b[i] = a[i] + a[(i + 1) % k];
    return NULL;
}

int main(void) {
    if (scanf("%d", &k) != 1) return 1;
    for (int i = 0; i < k; i++) {
        if (scanf("%lld", &a[i]) != 1) return 1;
    }

    pthread_t th[16];
    int id[16];
    int created = 0;

    for (int i = 0; i < k; i++) {
        id[i] = i;
        if (pthread_create(&th[i], NULL, worker, &id[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);

    if (created != k) return 1;

    for (int i = 0; i < k; i++) {
        if (i > 0) printf(" ");
        printf("%lld", b[i]);
    }
    printf("\n");
    return 0;
}
