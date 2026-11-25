#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const long long *a;
    int n;
} Task;

static void *worker(void *arg) {
    Task *t = (Task *)arg;
    long long *res = (long long *)malloc(sizeof(long long));
    if (!res) return NULL;
    long long sum = 0;
    for (int i = 0; i < t->n; i++) {
        sum += t->a[i];
    }
    *res = sum;
    return res;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    long long *a = (long long *)malloc(sizeof(long long) * n);
    if (!a) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) {
            free(a);
            return 1;
        }
    }

    Task t = {a, n};
    pthread_t th;
    if (pthread_create(&th, NULL, worker, &t) != 0) {
        free(a);
        return 1;
    }

    void *ret = NULL;
    if (pthread_join(th, &ret) != 0 || ret == NULL) {
        free(a);
        return 1;
    }

    long long total = *(long long *)ret;
    free(ret);
    free(a);

    printf("%lld\n", total);
    return 0;
}
