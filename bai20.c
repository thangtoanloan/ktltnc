#include <pthread.h>
#include <stdio.h>

int n;
int next = 1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void* worker(void* arg) {
    int parity = *(int*)arg;

    pthread_mutex_lock(&m);
    while (next <= n) {
        if (next % 2 == parity) {
            if (next > 1) printf(" ");
            printf("%d", next);
            next++;
            pthread_cond_broadcast(&cv);
        } else {
            pthread_cond_wait(&cv, &m);
        }
    }
    pthread_cond_broadcast(&cv);
    pthread_mutex_unlock(&m);
    return NULL;
}

int main(void) {
    if (scanf("%d", &n) != 1) return 1;

    pthread_t t1, t2;
    int odd = 1, even = 0;

    if (pthread_create(&t1, NULL, worker, &odd) != 0) return 1;
    if (pthread_create(&t2, NULL, worker, &even) != 0) {
        /* luồng lẻ sẽ in xong 1 rồi chờ mãi, nên không thể join an toàn */
        return 1;
    }

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\n");
    return 0;
}
