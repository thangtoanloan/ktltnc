#include <pthread.h>
#include <stdio.h>

int n, T;
long long target;
long long a[100000];

int found = 0;
int found_idx = -1;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int lo, hi; /* đoạn [lo, hi), nửa mở */
} Task;

void* worker(void* arg) {
    Task* t = (Task*)arg;
    for (int i = t->lo; i < t->hi; i++) {
        /* kiểm tra dừng sớm định kỳ (mỗi 256 phần tử) */
        if (((i - t->lo) & 255) == 0) {
            pthread_mutex_lock(&m);
            int f = found;
            pthread_mutex_unlock(&m);
            if (f) return NULL;
        }
        if (a[i] == target) {
            pthread_mutex_lock(&m);
            found = 1;
            found_idx = i;
            pthread_mutex_unlock(&m);
            return NULL;
        }
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %lld %d", &n, &target, &T) != 3) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) return 1;
    }

    int P = (n < T) ? n : T; /* min(n, T) luồng */
    int base = n / P, rem = n % P;
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

    printf("%d\n", found_idx);
    return 0;
}
