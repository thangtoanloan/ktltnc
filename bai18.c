#include <pthread.h>
#include <stdio.h>

static long long counter = 0;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int id, inc, dec;
} Task;

static void *worker(void *arg) {
    Task *t = (Task *)arg;
    if (t->id % 2 == 1) {
        for (int i = 0; i < t->inc; i++) {
            pthread_mutex_lock(&m);
            counter++;
            pthread_mutex_unlock(&m);
        }
    } else {
        for (int i = 0; i < t->dec; i++) {
            pthread_mutex_lock(&m);
            counter--;
            pthread_mutex_unlock(&m);
        }
    }
    return NULL;
}

int main(void) {
    int k, inc, dec;
    if (scanf("%d %d %d", &k, &inc, &dec) != 3) return 1;

    pthread_t th[16];
    Task t[16];
    int created = 0;
    for (int i = 0; i < k; i++) {
        t[i].id = i + 1;
        t[i].inc = inc;
        t[i].dec = dec;
        if (pthread_create(&th[i], NULL, worker, &t[i]) != 0) break;
        created++;
    }
    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);

    if (created != k) return 1;

    printf("%lld\n", counter);
    return 0;
}
