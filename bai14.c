#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static volatile sig_atomic_t got_usr1 = 0;  /* dùng ở cha */
static volatile sig_atomic_t got_usr2 = 0;  /* dùng ở con */

static void on_usr1(int sig) { (void)sig; got_usr1 = 1; }
static void on_usr2(int sig) { (void)sig; got_usr2 = 1; }

int main(void) {
    int m;
    if (scanf("%d", &m) != 1) return 1;

    /* Chặn SIGUSR1, SIGUSR2 trước khi fork để không mất tín hiệu */
    sigset_t block, old;
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);
    sigaddset(&block, SIGUSR2);
    if (sigprocmask(SIG_BLOCK, &block, &old) < 0) return 1;

    /* Cha: handler SIGUSR1. Con: handler SIGUSR2 (đặt trong con) */
    struct sigaction sa;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    pid_t parent = getpid();
    pid_t pid = fork();
    if (pid < 0) return 1;

    if (pid == 0) {
        /* Con: gửi SIGUSR1, chờ SIGUSR2 xác nhận */
        sa.sa_handler = on_usr2;
        sigaction(SIGUSR2, &sa, NULL);

        for (int i = 0; i < m; i++) {
            got_usr2 = 0;
            kill(parent, SIGUSR1);
            while (!got_usr2) sigsuspend(&old);   /* mở chặn và ngủ nguyên tử */
        }
        _exit(0);
    }

    /* Cha: nhận SIGUSR1, trả lời SIGUSR2 */
    sa.sa_handler = on_usr1;
    sigaction(SIGUSR1, &sa, NULL);

    int count = 0;
    for (int i = 0; i < m; i++) {
        while (!got_usr1) sigsuspend(&old);
        got_usr1 = 0;
        count++;
        kill(pid, SIGUSR2);
    }

    waitpid(pid, NULL, 0);
    printf("%d\n", count);
    return 0;
}
