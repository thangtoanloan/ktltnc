#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;

    long long *a = (long long *)malloc(sizeof(long long) * n);
    if (!a) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) { free(a); return 1; }
    }

    int p[2];
    if (pipe(p) < 0) { free(a); return 1; }

    pid_t pid = fork();
    if (pid < 0) { free(a); return 1; }

    if (pid == 0) {
        close(p[1]);
        long long sum = 0, x;
        while (read(p[0], &x, sizeof(x)) == (ssize_t)sizeof(x)) {
            sum += x;
        }
        close(p[0]);
        printf("%lld\n", sum);
        fflush(stdout);
        _exit(0);
    }

    close(p[0]);
    for (int i = 0; i < n; i++) {
        if (write(p[1], &a[i], sizeof(long long)) != (ssize_t)sizeof(long long)) break;
    }
    close(p[1]);

    int status;
    waitpid(pid, &status, 0);
    free(a);
    return 0;
}
