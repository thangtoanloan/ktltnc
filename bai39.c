#include <stdio.h>

long long p[2001], r[2001];
int s[2001];

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    for (int i = 1; i <= n; i++) {
        if (scanf("%lld", &p[i]) != 1) return 1;
    }

    r[0] = 0;
    for (int j = 1; j <= n; j++) {
        long long best = -1;
        for (int i = 1; i <= j; i++) {
            long long q = p[i] + r[j - i];
            if (q > best) { /* nghiêm ngặt: hòa thì giữ i nhỏ hơn */
                best = q;
                s[j] = i;
            }
        }
        r[j] = best;
    }

    printf("%lld\n", r[n]);

    int rem = n, first = 1;
    while (rem > 0) {
        if (!first) printf(" ");
        printf("%d", s[rem]);
        first = 0;
        rem -= s[rem];
    }
    printf("\n");
    return 0;
}
