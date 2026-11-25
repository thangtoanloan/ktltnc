#include <stdio.h>

#define INF 1000000000

int c[100];
int dp[100001];

int main(void) {
    int n, S;
    if (scanf("%d %d", &n, &S) != 2) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &c[i]) != 1) return 1;
    }

    dp[0] = 0;
    for (int s = 1; s <= S; s++) {
        dp[s] = INF;
        for (int i = 0; i < n; i++) {
            if (c[i] <= s && dp[s - c[i]] != INF && dp[s - c[i]] + 1 < dp[s]) {
                dp[s] = dp[s - c[i]] + 1;
            }
        }
    }

    printf("%d\n", dp[S] == INF ? -1 : dp[S]);
    return 0;
}
