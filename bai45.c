#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    long long mn;
    long long mx;
} Pair;

int main(void) {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 1;

    long long *a = (long long *)malloc(sizeof(long long) * n);
    if (!a) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) { free(a); return 1; }
    }

    int c = (n < k) ? n : k;               /* min(n, k) tiến trình con */
    int fd[8][2];
    pid_t pids[8];
    int base = n / c, rem = n % c;
    int created = 0, ok = 1;
    int cur = 0;

    for (int i = 0; i < c; i++) {
        int sz = base + (i < rem ? 1 : 0);
        int L = cur, R = cur + sz;         /* đoạn [L, R) */
        cur += sz;

        if (pipe(fd[i]) < 0) { ok = 0; break; }

        pid_t pid = fork();
        if (pid < 0) {
            close(fd[i][0]);
            close(fd[i][1]);
            ok = 0;
            break;
        }

        if (pid == 0) {
            /* Con: đóng đầu đọc của mọi pipe đã mở (kể cả của chính mình) */
            for (int j = 0; j <= i; j++) close(fd[j][0]);

            Pair p;
            p.mn = a[L];
            p.mx = a[L];
            for (int x = L + 1; x < R; x++) {
                if (a[x] < p.mn) p.mn = a[x];
                if (a[x] > p.mx) p.mx = a[x];
            }

            if (write(fd[i][1], &p, sizeof(p)) != (ssize_t)sizeof(p)) _exit(1);
            close(fd[i][1]);
            _exit(0);
        }

        close(fd[i][1]);                   /* cha chỉ đọc */
        pids[created++] = pid;
    }

    long long gmin = 0, gmax = 0;
    int have = 0;
    for (int i = 0; i < created; i++) {
        Pair p;
        if (read(fd[i][0], &p, sizeof(p)) == (ssize_t)sizeof(p)) {
            if (!have) { gmin = p.mn; gmax = p.mx; have = 1; }
            else {
                if (p.mn < gmin) gmin = p.mn;
                if (p.mx > gmax) gmax = p.mx;
            }
        } else {
            ok = 0;
        }
        close(fd[i][0]);
    }

    for (int i = 0; i < created; i++) waitpid(pids[i], NULL, 0);
    free(a);

    if (!ok || !have || created != c) return 1;
    printf("%lld %lld\n", gmin, gmax);
    return 0;
}
