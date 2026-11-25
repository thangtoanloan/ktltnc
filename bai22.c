#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int B, n;
long long* x;      /* dữ liệu đầu vào */
long long buf[64]; /* hàng đợi vòng, chỉ dùng B phần tử đầu */
int head = 0, tail = 0, cnt = 0;
long long total = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void* producer(void* arg) {
    (void)arg;
    for (int i = 0; i < n; i++) {
        pthread_mutex_lock(&m);
        while (cnt == B) pthread_cond_wait(&not_full, &m);
        buf[tail] = x[i];
        tail = (tail + 1) % B;
        cnt++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

void* consumer(void* arg) {
    (void)arg;
    for (int i = 1; i <= n; i++) {
        pthread_mutex_lock(&m);
        while (cnt == 0) pthread_cond_wait(&not_empty, &m);
        long long v = buf[head];
        head = (head + 1) % B;
        cnt--;
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&m);
        total += (long long)i * v; /* chỉ consumer dùng total */
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d", &B, &n) != 2) return 1;
    x = (long long*)malloc(sizeof(long long) * n);
    if (!x) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &x[i]) != 1) {
            free(x);
            return 1;
        }
    }

    pthread_t p, c;
    if (pthread_create(&p, NULL, producer, NULL) != 0) {
        free(x);
        return 1;
    }
    if (pthread_create(&c, NULL, consumer, NULL) != 0) {
        free(x);
        return 1;
    }

    pthread_join(p, NULL);
    pthread_join(c, NULL);

    free(x);
    printf("%lld\n", total);
    return 0;
}
