#include <stdio.h>

long long dp[10001];

int main(void) {
    int n, W;
    if (scanf("%d %d", &n, &W) != 2) return 1;

    for (int i = 0; i < n; i++) {
        int w;
        long long v;
        if (scanf("%d %lld", &w, &v) != 2) return 1;
        for (int j = W; j >= w; j--) {
            if (dp[j - w] + v > dp[j]) dp[j] = dp[j - w] + v;
        }
    }

    printf("%lld\n", dp[W]);
    return 0;
}
