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
    for (int j = 1; j <= n; j++) {
        for (int i = 1; i <= j; i++) {
            long long m = p[i] + dp[j - i];
            if (m > dp[j]) dp[j] = m;
        }
    }
    printf("%lld\n", dp[n]);
    return 0;
}
