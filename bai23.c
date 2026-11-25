#include <pthread.h>
#include <stdio.h>

int P, C, B, n;
long long a[10000];
long long buf[32];
int head = 0, tail = 0, cnt = 0;
int active;                 /* số Producer còn đang chạy */
long long even_sum = 0;

pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t not_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t not_empty = PTHREAD_COND_INITIALIZER;

void *producer(void *arg) {
    int id = *(int *)arg;
    int base = n / P, rem = n % P;
    int L = id * base + (id < rem ? id : rem);
    int R = L + base + (id < rem ? 1 : 0);   /* đoạn [L, R) */

    for (int i = L; i < R; i++) {
        pthread_mutex_lock(&m);
        while (cnt == B) pthread_cond_wait(&not_full, &m);
        buf[tail] = a[i];
        tail = (tail + 1) % B;
        cnt++;
        pthread_cond_signal(&not_empty);
        pthread_mutex_unlock(&m);
    }

    pthread_mutex_lock(&m);
    active--;
    if (active == 0) pthread_cond_broadcast(&not_empty);  /* đánh thức Consumer để thoát */
    pthread_mutex_unlock(&m);
    return NULL;
}

void *consumer(void *arg) {
    (void)arg;
    while (1) {
        pthread_mutex_lock(&m);
        while (cnt == 0 && active > 0) pthread_cond_wait(&not_empty, &m);
        if (cnt == 0 && active == 0) {       /* hết dữ liệu và hết Producer */
            pthread_mutex_unlock(&m);
            break;
        }
        long long v = buf[head];
        head = (head + 1) % B;
        cnt--;
        if (v % 2 == 0) even_sum += v;       /* đang giữ mutex nên đồng bộ */
        pthread_cond_signal(&not_full);
        pthread_mutex_unlock(&m);
    }
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d %d", &P, &C, &B, &n) != 4) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%lld", &a[i]) != 1) return 1;
    }
    active = P;

    pthread_t pt[8], ct[8];
    int id[8];

    for (int i = 0; i < C; i++) pthread_create(&ct[i], NULL, consumer, NULL);
    for (int i = 0; i < P; i++) {
        id[i] = i;
        pthread_create(&pt[i], NULL, producer, &id[i]);
    }

    for (int i = 0; i < P; i++) pthread_join(pt[i], NULL);
    for (int i = 0; i < C; i++) pthread_join(ct[i], NULL);

    printf("%lld\n", even_sum);
    return 0;
}
