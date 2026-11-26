#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define MAXD 2   /* số mức sinh luồng tối đa */

char X[505], Y[505];
int memo[501][501];
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    int m, n, depth;
    int result;
} Task;

int lcs(int m, int n, int depth);

void *worker(void *arg) {
    Task *t = (Task *)arg;
    t->result = lcs(t->m, t->n, t->depth);
    return NULL;
}

int lcs(int m, int n, int depth) {
    if (m == 0 || n == 0) return 0;

    pthread_mutex_lock(&mtx);
    int v = memo[m][n];
    pthread_mutex_unlock(&mtx);
    if (v != -1) return v;

    int res;
    if (X[m - 1] == Y[n - 1]) {
        res = 1 + lcs(m - 1, n - 1, depth);
    } else if (depth < MAXD) {
        Task t1 = { m - 1, n, depth + 1, 0 };
        Task t2 = { m, n - 1, depth + 1, 0 };
        pthread_t th1, th2;
        int c1 = (pthread_create(&th1, NULL, worker, &t1) == 0);
        int c2 = (pthread_create(&th2, NULL, worker, &t2) == 0);
        if (!c1) t1.result = lcs(m - 1, n, depth + 1);   /* dự phòng nếu tạo luồng lỗi */
        if (!c2) t2.result = lcs(m, n - 1, depth + 1);
        if (c1) pthread_join(th1, NULL);                 /* không giữ khóa khi join */
        if (c2) pthread_join(th2, NULL);
        res = t1.result > t2.result ? t1.result : t2.result;
    } else {
        int a = lcs(m - 1, n, depth);
        int b = lcs(m, n - 1, depth);
        res = a > b ? a : b;
    }

    pthread_mutex_lock(&mtx);
    memo[m][n] = res;
    pthread_mutex_unlock(&mtx);
    return res;
}

int main(void) {
    if (scanf("%504s %504s", X, Y) != 2) return 1;
    int m = strlen(X), n = strlen(Y);

    for (int i = 0; i <= m; i++)
        for (int j = 0; j <= n; j++)
            memo[i][j] = -1;

    printf("%d\n", lcs(m, n, 0));
    return 0;
}
