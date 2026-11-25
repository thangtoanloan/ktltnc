#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int k;
    if (scanf("%d", &k) != 1) return 1;

    int c[16];
    for (int i = 0; i < k; i++) {
        if (scanf("%d", &c[i]) != 1) return 1;
    }

    pid_t pids[16];
    int created = 0;

    for (int i = 0; i < k; i++) {
        pid_t pid = fork();
        if (pid < 0) break;

        if (pid == 0) {
            if (c[i] >= 0) {
                _exit(c[i] % 200);
            } else {
                kill(getpid(), SIGKILL);
                _exit(1);   /* không bao giờ tới đây */
            }
        }
        pids[created++] = pid;   /* chỉ cha chạy tới đây */
    }

    int normal = 0, killed = 0;
    long long sum_exit = 0;

    for (int i = 0; i < created; i++) {
        int status;
        if (waitpid(pids[i], &status, 0) < 0) continue;

        if (WIFEXITED(status)) {
            normal++;
            sum_exit += WEXITSTATUS(status);
        } else if (WIFSIGNALED(status) && WTERMSIG(status) == SIGKILL) {
            killed++;
        }
    }

    if (created != k) return 1;

    printf("%d %lld %d\n", normal, sum_exit, killed);
    return 0;
}
