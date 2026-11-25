#include <stdio.h>
#include <string.h>

char X[1005], Y[1005], res[1005];
int dp[1001][1001];

int main(void) {
    if (scanf("%1004s %1004s", X, Y) != 2) return 1;
    int m = strlen(X), n = strlen(Y);

    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (X[i - 1] == Y[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
            } else {
                dp[i][j] =
                    dp[i - 1][j] > dp[i][j - 1] ? dp[i - 1][j] : dp[i][j - 1];
            }
        }
    }

    int len = dp[m][n];
    int k = len;
    res[k] = '\0';

    int i = m, j = n;
    while (i > 0 && j > 0) {
        if (X[i - 1] == Y[j - 1]) {
            res[--k] = X[i - 1];
            i--;
            j--;
        } else if (dp[i - 1][j] > dp[i][j - 1]) {
            i--;
        } else {
            j--;
        }
    }

    printf("%d\n", len);
    if (len == 0)
        printf("EMPTY\n");
    else
        printf("%s\n", res);
    return 0;
}
