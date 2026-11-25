#include <pthread.h>
#include <stdio.h>

int n, threshold, max_depth;
int a[100000];

typedef struct {
    int lo, hi; /* đoạn [lo, hi], bao gồm */
    int depth;
} Task;

void swap_int(int* x, int* y) {
    int t = *x;
    *x = *y;
    *y = t;
}

/* Hoare partition với pivot ở giữa; trả về (i, j) qua con trỏ */
void partition(int lo, int hi, int* pi, int* pj) {
    int pivot = a[lo + (hi - lo) / 2];
    int i = lo, j = hi;
    while (i <= j) {
        while (a[i] < pivot) i++;
        while (a[j] > pivot) j--;
        if (i <= j) {
            swap_int(&a[i], &a[j]);
            i++;
            j--;
        }
    }
    *pi = i;
    *pj = j;
}

void quicksort_seq(int lo, int hi) {
    while (lo < hi) {
        int i, j;
        partition(lo, hi, &i, &j);
        /* đệ quy đoạn ngắn hơn, lặp đoạn dài hơn để giới hạn độ sâu stack */
        if (j - lo < hi - i) {
            quicksort_seq(lo, j);
            lo = i;
        } else {
            quicksort_seq(i, hi);
            hi = j;
        }
    }
}

void* psort(void* arg) {
    Task* t = (Task*)arg;
    int lo = t->lo, hi = t->hi, depth = t->depth;

    if (lo >= hi) return NULL;

    if (hi - lo + 1 <= threshold || depth >= max_depth) {
        quicksort_seq(lo, hi); /* đoạn nhỏ hoặc quá sâu: tuần tự */
        return NULL;
    }

    int i, j;
    partition(lo, hi, &i, &j);

    Task left = {lo, j, depth + 1};
    Task right = {i, hi, depth + 1};

    pthread_t th;
    int spawned = (j > lo) && (pthread_create(&th, NULL, psort, &left) == 0);

    if (j > lo && !spawned) psort(&left); /* tạo luồng lỗi thì chạy tại chỗ */
    psort(&right);                        /* nhánh phải chạy ở luồng hiện tại */

    if (spawned) pthread_join(th, NULL);
    return NULL;
}

int main(void) {
    if (scanf("%d %d %d", &n, &threshold, &max_depth) != 3) return 1;
    for (int i = 0; i < n; i++) {
        if (scanf("%d", &a[i]) != 1) return 1;
    }

    Task root = {0, n - 1, 0};
    psort(&root);

    for (int i = 0; i < n; i++) {
        if (i > 0) printf(" ");
        printf("%d", a[i]);
    }
    printf("\n");
    return 0;
}
