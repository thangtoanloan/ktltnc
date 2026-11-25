#include <stdio.h>

#define MOD 1000000007LL

int g[501][501];
long long dp[501][501];

int main(void) {
    int M, N;
    if (scanf("%d %d", &M, &N) != 2) return 1;
    for (int i = 1; i <= M; i++)
        for (int j = 1; j <= N; j++)
            if (scanf("%d", &g[i][j]) != 1) return 1;

    if (g[1][1] == 1 || g[M][N] == 1) {
        printf("0\n");
        return 0;
    }

    dp[1][1] = 1;
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            if (i == 1 && j == 1) continue;
            if (g[i][j] == 1) {
                dp[i][j] = 0;
            } else {
                dp[i][j] = (dp[i - 1][j] + dp[i][j - 1]) % MOD;
            }
        }
    }

    printf("%lld\n", dp[M][N]);
    return 0;
}
