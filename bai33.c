#include <stdio.h>

long long cost[100001];
long long dp[100001];

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    for (int i = 1; i <= n; i++) {
        if (scanf("%lld", &cost[i]) != 1) return 1;
    }

    dp[0] = 0;
    dp[1] = cost[1];
    for (int i = 2; i <= n; i++) {
        long long best = dp[i - 1] < dp[i - 2] ? dp[i - 1] : dp[i - 2];
        dp[i] = cost[i] + best;
    }

    printf("%lld\n", dp[n]);
    return 0;
}
