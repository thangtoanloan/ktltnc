#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int n, k;
    if (scanf("%d %d", &n, &k) != 2) return 1;

    long long *A = (long long *)malloc(sizeof(long long) * n);
    long long *B = (long long *)malloc(sizeof(long long) * n);
    if (!A || !B) { free(A); free(B); return 1; }

    for (int i = 0; i < n; i++)
        if (scanf("%lld", &A[i]) != 1) { free(A); free(B); return 1; }
    for (int i = 0; i < n; i++)
        if (scanf("%lld", &B[i]) != 1) { free(A); free(B); return 1; }

    int procs = (n < k) ? n : k;          /* min(n, k) tiến trình con */
    int (*fd)[2] = malloc(sizeof(int[2]) * procs);
    pid_t *pids = (pid_t *)malloc(sizeof(pid_t) * procs);
    if (!fd || !pids) { free(A); free(B); free(fd); free(pids); return 1; }

    int created = 0;
    int ok = 1;
    for (int c = 0; c < procs; c++) {
        /* Đoạn [L, R) chia đều: phần dư rải cho các con đầu */
        int base = n / procs, rem = n % procs;
        int L = c * base + (c < rem ? c : rem);
        int R = L + base + (c < rem ? 1 : 0);

        if (pipe(fd[c]) < 0) { ok = 0; break; }

        pid_t pid = fork();
        if (pid < 0) {
            close(fd[c][0]); close(fd[c][1]);
            ok = 0;
            break;
        }

        if (pid == 0) {
            /* Con c: đóng mọi pipe không dùng */
            for (int j = 0; j < c; j++) {
                close(fd[j][0]);          /* đầu đọc của các con trước */
            }
            close(fd[c][0]);

            long long s = 0;
            for (int i = L; i < R; i++) s += A[i] * B[i];

            if (write(fd[c][1], &s, sizeof(s)) != (ssize_t)sizeof(s)) _exit(1);
            close(fd[c][1]);
            _exit(0);
        }

        /* Cha: đóng đầu ghi của pipe này, giữ đầu đọc */
        close(fd[c][1]);
        pids[created++] = pid;
    }

    long long total = 0;
    for (int c = 0; c < created; c++) {
        long long s;
        if (read(fd[c][0], &s, sizeof(s)) == (ssize_t)sizeof(s)) total += s;
        else ok = 0;
        close(fd[c][0]);
    }

    for (int c = 0; c < created; c++) waitpid(pids[c], NULL, 0);

    free(A); free(B); free(fd); free(pids);

    if (!ok || created != procs) return 1;
    printf("%lld\n", total);
    return 0;
}
