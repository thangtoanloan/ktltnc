#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    /* Đọc toàn bộ stdin đến EOF (tối đa 65536 byte) */
    size_t cap = 65536 + 1, len = 0;
    char *buf = (char *)malloc(cap);
    if (!buf) return 1;

    ssize_t r;
    while ((r = read(STDIN_FILENO, buf + len, cap - len)) > 0) {
        len += (size_t)r;
        if (len == cap) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap);
            if (!tmp) { free(buf); return 1; }
            buf = tmp;
        }
    }

    int in_pipe[2], out_pipe[2];
    if (pipe(in_pipe) < 0) { free(buf); return 1; }
    if (pipe(out_pipe) < 0) {
        close(in_pipe[0]); close(in_pipe[1]);
        free(buf);
        return 1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        close(in_pipe[0]); close(in_pipe[1]);
        close(out_pipe[0]); close(out_pipe[1]);
        free(buf);
        return 1;
    }

    if (pid == 0) {
        /* Con: STDIN <- in_pipe[0], STDOUT -> out_pipe[1] */
        dup2(in_pipe[0], STDIN_FILENO);
        dup2(out_pipe[1], STDOUT_FILENO);
        close(in_pipe[0]);
        close(in_pipe[1]);
        close(out_pipe[0]);
        close(out_pipe[1]);
        free(buf);

        execlp("wc", "wc", "-w", (char *)NULL);
        _exit(127);   /* chỉ chạy tới đây nếu exec lỗi */
    }

    /* Cha: ghi vào in_pipe[1], đọc từ out_pipe[0] */
    close(in_pipe[0]);
    close(out_pipe[1]);

    size_t off = 0;
    while (off < len) {
        ssize_t w = write(in_pipe[1], buf + off, len - off);
        if (w <= 0) break;
        off += (size_t)w;
    }
    close(in_pipe[1]);   /* để wc nhận EOF, kể cả khi đầu vào rỗng */

    char out[64];
    size_t olen = 0;
    while (olen < sizeof(out) - 1 &&
           (r = read(out_pipe[0], out + olen, sizeof(out) - 1 - olen)) > 0) {
        olen += (size_t)r;
    }
    out[olen] = '\0';
    close(out_pipe[0]);

    waitpid(pid, NULL, 0);
    free(buf);

    long long count = atoll(out);   /* atoll bỏ qua khoảng trắng và '\n' */
    printf("%lld\n", count);
    return 0;
}
