#include <stdio.h>
#include <stdlib.h>

typedef struct {
    long long s, f, p;
} Job;

Job job[10001];
long long dp[10001];

int cmp(const void* a, const void* b) {
    const Job *x = (const Job*)a, *y = (const Job*)b;
    if (x->f < y->f) return -1;
    if (x->f > y->f) return 1;
    return 0;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    for (int i = 1; i <= n; i++) {
        if (scanf("%lld %lld %lld", &job[i].s, &job[i].f, &job[i].p) != 3)
            return 1;
    }

    qsort(job + 1, n, sizeof(Job), cmp);

    dp[0] = 0;
    for (int i = 1; i <= n; i++) {
        /* tìm k lớn nhất trong [0, i-1] sao cho job[k].f <= job[i].s (k = 0:
         * không có) */
        int lo = 0, hi = i - 1, k = 0;
        while (lo <= hi) {
            int mid = (lo + hi) / 2;
            if (mid == 0 || job[mid].f <= job[i].s) {
                k = mid;
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
        long long take = dp[k] + job[i].p;
        dp[i] = dp[i - 1] > take ? dp[i - 1] : take;
    }

    printf("%lld\n", dp[n]);
    return 0;
}
