#include <pthread.h>
#include <stdio.h>

long long memo[100];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

long long fib(int k) {
    pthread_mutex_lock(&m);
    long long v = memo[k];
    pthread_mutex_unlock(&m);
    if (v != -1) return v;

    /* không giữ khóa khi đệ quy */
    long long r = fib(k - 1) + fib(k - 2);

    pthread_mutex_lock(&m);
    memo[k] = r;
    pthread_mutex_unlock(&m);
    return r;
}

void* worker(void* arg) {
    int k = *(int*)arg;
    long long* res = (long long*)arg; /* tái dùng ô nhớ để trả kết quả */
    *res = fib(k);
    return NULL;
}

int main(void) {
    int n;
    if (scanf("%d", &n) != 1) return 1;

    for (int i = 0; i < 100; i++) memo[i] = -1;
    memo[0] = 0;
    memo[1] = 1;

    if (n < 2) {
        printf("%d\n", n);
        return 0;
    }

    /* mỗi luồng có ô riêng: vào là chỉ số k, ra là kết quả */
    long long a1 = n - 1, a2 = n - 2;
    pthread_t t1, t2;

    if (pthread_create(&t1, NULL, worker, &a1) != 0) return 1;
    if (pthread_create(&t2, NULL, worker, &a2) != 0) return 1;

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("%lld\n", a1 + a2);
    return 0;
}
