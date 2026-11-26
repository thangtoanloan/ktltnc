#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

volatile sig_atomic_t got_sig = 0;

void on_usr1(int sig) {
    (void)sig;
    got_sig = 1;
}

int main(void) {
    char src[256], dst[256];
    if (scanf("%255s %255s", src, dst) != 2) return 1;

    /* đăng ký handler TRƯỚC khi fork */
    struct sigaction sa;
    sa.sa_handler = on_usr1;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) < 0) return 1;

    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        int fi = open(src, O_RDONLY);
        if (fi < 0) _exit(1);                       /* không gửi tín hiệu */

        int fo = open(dst, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fo < 0) { close(fi); _exit(1); }

        unsigned char in[4096], out[4096];
        ssize_t r;
        while ((r = read(fi, in, sizeof(in))) > 0) {
            size_t m = 0;
            for (ssize_t i = 0; i < r; i++) {
                if ((in[i] >= 32 && in[i] <= 126) || in[i] == '\n')
                    out[m++] = in[i];
            }
            size_t off = 0;
            while (off < m) {
                ssize_t w = write(fo, out + off, m - off);
                if (w <= 0) { close(fi); close(fo); _exit(1); }
                off += (size_t)w;
            }
        }
        close(fi);
        close(fo);

        kill(getppid(), SIGUSR1);                   /* báo hiệu hoàn tất */
        _exit(0);
    }

    int status;
    if (waitpid(pid, &status, 0) < 0) return 1;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        printf("-1 -1\n");
        return 0;
    }

    struct stat st;
    if (stat(dst, &st) != 0) {
        printf("-1 -1\n");
        return 0;
    }

    printf("%d %lld\n", (int)got_sig, (long long)st.st_size);
    return 0;
}
