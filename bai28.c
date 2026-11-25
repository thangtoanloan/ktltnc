#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int n, T;
int *px;
long long local_hist[16][256];   /* mỗi luồng một hàng riêng */

typedef struct {
    int tid;
    int lo, hi;   /* đoạn [lo, hi), nửa mở */
} Task;

void *worker(void *arg) {
    Task *t = (Task *)arg;
    long long *h = local_hist[t->tid];     /* mảng cục bộ của luồng */
    for (int i = t->lo; i < t->hi; i++) {
        h[px[i]]++;
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d", &n, &T) != 2) return 1;
    px = (int *)malloc(sizeof(int) * n);
    if (!px) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &px[i]) != 1) { free(px); return 1; }
    }

    int P = (n < T) ? n : T;     /* min(n, T) luồng */
    int base = n / P, rem = n % P;
    pthread_t th[16];
    Task tasks[16];
    int created = 0;
    int cur = 0;

    for (int i = 0; i < P; i++) {
        int sz = base + (i < rem ? 1 : 0);
        tasks[i].tid = i;
        tasks[i].lo = cur;
        tasks[i].hi = cur + sz;
        cur += sz;
        if (pthread_create(&th[i], NULL, worker, &tasks[i]) != 0) break;
        created++;
    }

    for (int i = 0; i < created; i++) pthread_join(th[i], NULL);
    if (created != P) { free(px); return 1; }

    /* Reduce: gộp các histogram cục bộ */
    long long hist[256] = {0};
    for (int i = 0; i < P; i++)
        for (int v = 0; v < 256; v++)
            hist[v] += local_hist[i][v];

    int distinct = 0, mode = 0;
    long long best = -1;
    for (int v = 0; v < 256; v++) {
        if (hist[v] > 0) distinct++;
        if (hist[v] > best) {          /* dấu > nên khi hòa giữ giá trị nhỏ hơn */
            best = hist[v];
            mode = v;
        }
    }

    free(px);
    printf("%d %d %lld\n", distinct, mode, best);
    return 0;
}
