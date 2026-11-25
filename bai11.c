#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    int k;
    if (scanf("%d", &k) != 1) return 1;

    int p[2];
    if (pipe(p) < 0) return 1;

    pid_t* pids = (pid_t*)malloc(sizeof(pid_t) * k);
    if (!pids) return 1;

    int created = 0;
    for (int i = 1; i <= k; i++) {
        pid_t pid = fork();
        if (pid < 0) break;

        if (pid == 0) {
            close(p[0]);
            long long v = (long long)i * i * i;
            if (write(p[1], &v, sizeof(v)) != (ssize_t)sizeof(v)) _exit(1);
            close(p[1]);
            _exit(0);
        }
        pids[created++] = pid;
    }

    close(p[1]);

    long long sum = 0, x;
    int received = 0;
    while (received < created &&
           read(p[0], &x, sizeof(x)) == (ssize_t)sizeof(x)) {
        sum += x;
        received++;
    }
    close(p[0]);

    for (int i = 0; i < created; i++) {
        waitpid(pids[i], NULL, 0);
    }
    free(pids);

    if (created != k || received != k) return 1;

    printf("%lld\n", sum);
    return 0;
}
