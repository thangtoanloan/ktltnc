#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int is_prime(long long n) {
    if (n <= 1) return 0;
    if (n < 4) return 1;
    if (n % 2 == 0) return 0;
    for (long long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;
    long long* a = (long long*)malloc(sizeof(long long) * n);
    if (!a) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) {
            free(a);
            return 1;
        }
    }

    int p2c[2], c2p[2];
    if (pipe(p2c) < 0) {
        free(a);
        return 1;
    }
    if (pipe(c2p) < 0) {
        close(p2c[0]);
        close(p2c[1]);
        free(a);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(p2c[0]);
        close(p2c[1]);
        close(c2p[0]);
        close(c2p[1]);
        free(a);
        return 1;
    }

    if (pid == 0) {
        close(p2c[1]);
        close(c2p[0]);
        long long sum = 0, x;
        while (read(p2c[0], &x, sizeof(x)) == (ssize_t)sizeof(x)) {
            if (is_prime(x)) sum += x;
        }
        close(p2c[0]);
        if (write(c2p[1], &sum, sizeof(sum)) != (ssize_t)sizeof(sum)) {
            _exit(1);
        }
        close(c2p[1]);
        _exit(0);
    }

    close(p2c[0]);
    close(c2p[1]);
    for (int i = 0; i < n; i++) {
        if (write(p2c[1], &a[i], sizeof(long long)) !=
            (ssize_t)sizeof(long long)) {
            break;
        }
    }
    close(p2c[1]);

    long long result = 0;
    ssize_t r = read(c2p[0], &result, sizeof(result));
    close(c2p[0]);
    waitpid(pid, NULL, 0);
    free(a);

    if (r != (ssize_t)sizeof(result)) return 1;
    printf("%lld\n", result);
    return 0;
}
