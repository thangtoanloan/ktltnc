#include <stdio.h>

long long p[2001];
long long dp[2001];

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    for (int i = 1; i <= n; i++) {
        if (scanf("%lld", &p[i]) != 1) return 1;
    }

    dp[0] = 0;
    for (int len = 1; len <= n; len++) {
        long long best = 0;
        for (int i = 1; i <= len; i++) {
            long long cand = p[i] + dp[len - i];
            if (cand > best) best = cand;
        }
        dp[len] = best;
    }

    printf("%lld\n", dp[n]);
    return 0;
}
