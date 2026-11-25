#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    long long a, b;
    if (scanf("%lld %lld", &a, &b) != 2) return 0;

    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        long long sum = 0;
        for (long long i = a; i <= b; i++) {
            sum = (sum + i) % 200;
        }
        _exit((int)sum);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) return 1;

    if (WIFEXITED(status)) {
        printf("%d\n", WEXITSTATUS(status));
    }
    return 0;
}
