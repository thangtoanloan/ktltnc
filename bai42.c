#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    char path[256];
    if (scanf("%255s", path) != 1) return 1;

    int p[2];
    if (pipe(p) < 0) return 1;

    pid_t pid = fork();
    if (pid < 0) {
        close(p[0]);
        close(p[1]);
        return 1;
    }

    if (pid == 0) {
        close(p[0]); /* con chỉ ghi */
        long long sum = 0;

        int fd = open(path, O_RDONLY);
        if (fd < 0) {
            sum = -1;
        } else {
            char buf[4096];
            ssize_t r;
            int in_num = 0, neg = 0;
            long long cur = 0;

            while ((r = read(fd, buf, sizeof(buf))) > 0) {
                for (ssize_t i = 0; i < r; i++) {
                    char c = buf[i];
                    if (c >= '0' && c <= '9') {
                        cur = cur * 10 + (c - '0');
                        in_num = 1;
                    } else if (c == '-' && !in_num) {
                        neg = 1;
                    } else {
                        if (in_num && !neg && cur > 0) sum += cur;
                        in_num = 0;
                        neg = 0;
                        cur = 0;
                        if (c == '-') neg = 1;
                    }
                }
            }
            if (in_num && !neg && cur > 0)
                sum += cur; /* số cuối tệp không có ký tự phân cách */
            close(fd);
        }

        if (write(p[1], &sum, sizeof(sum)) != (ssize_t)sizeof(sum)) _exit(1);
        close(p[1]);
        _exit(0);
    }

    close(p[1]); /* cha chỉ đọc */
    long long result = 0;
    ssize_t r = read(p[0], &result, sizeof(result));
    close(p[0]);
    waitpid(pid, NULL, 0);

    if (r != (ssize_t)sizeof(result)) return 1;
    printf("%lld\n", result);
    return 0;
}
