#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int left, right; /* right là chỉ số cuối, bao gồm */
    const long long* a;
    long long local_max;
} Task;

static void* worker(void* arg) {
    Task* t = (Task*)arg;
    long long mx = t->a[t->left];
    for (int i = t->left + 1; i <= t->right; i++) {
        if (t->a[i] > mx) mx = t->a[i];
    }
    t->local_max = mx;
    return NULL;
}

int main(void) {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 1;

    long long* a = (long long*)malloc(sizeof(long long) * n);
    if (!a) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) {
            free(a);
            return 1;
        }
    }

    pthread_t th[16];
    Task t[16];

    int P = (n < k) ? n : k; /* min(n, k) luồng */
    int base = n / P;
    int rem = n % P;
    int cur = 0;
    int created = 0;

    for (int i = 0; i < P; i++) {
        int len = base + (i < rem ? 1 : 0);

        t[i].a = a;
        t[i].left = cur;
        t[i].right = cur + len - 1;
        t[i].local_max = 0;
        cur += len;

        if (pthread_create(&th[i], NULL, worker, &t[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);

    if (created != P) {
        free(a);
        return 1;
    }

    long long best = t[0].local_max;
    for (int i = 1; i < P; i++) {
        if (t[i].local_max > best) best = t[i].local_max;
    }

    free(a);
    printf("%lld\n", best);
    return 0;
}
