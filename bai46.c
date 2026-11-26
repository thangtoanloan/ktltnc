#include <pthread.h>
#include <stdio.h>

int n, W, T;
int wt[201];
long long val[201];

long long buf0[10001], buf1[10001];
long long *dp_prev = buf0, *dp_cur = buf1;

typedef struct {
    int lo, hi;   /* dải trọng lượng [lo, hi), nửa mở */
    int w;
    long long v;
} Task;

void *worker(void *arg) {
    Task *t = (Task *)arg;
    for (int j = t->lo; j < t->hi; j++) {
        long long best = dp_prev[j];                 /* không chọn vật i */
        if (j >= t->w && dp_prev[j - t->w] + t->v > best)
            best = dp_prev[j - t->w] + t->v;         /* chọn vật i */
        dp_cur[j] = best;
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d", &n, &W, &T) != 3) return 1;
    for (int i = 1; i <= n; i++) {
        if (scanf("%d %lld", &wt[i], &val[i]) != 2) return 1;
    }

    int total = W + 1;                    /* số trạng thái w = 0..W */
    int P = (total < T) ? total : T;
    int base = total / P, rem = total % P;

    for (int i = 1; i <= n; i++) {
        pthread_t th[8];
        Task tasks[8];
        int created = 0, cur = 0;

        for (int c = 0; c < P; c++) {
            int sz = base + (c < rem ? 1 : 0);
            tasks[c].lo = cur;
            tasks[c].hi = cur + sz;
            tasks[c].w = wt[i];
            tasks[c].v = val[i];
            cur += sz;
            if (pthread_create(&th[c], NULL, worker, &tasks[c]) != 0) break;
            created++;
        }

        for (int c = 0; c < created; c++) pthread_join(th[c], NULL);
        if (created != P) return 1;

        /* hàng i hoàn tất, hoán đổi để chuẩn bị vật tiếp theo */
        long long *tmp = dp_prev;
        dp_prev = dp_cur;
        dp_cur = tmp;
    }

    printf("%lld\n", dp_prev[W]);
    return 0;
}
