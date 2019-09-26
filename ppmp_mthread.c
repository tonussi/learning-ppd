#include <time.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>

#define NUMBER_OF_PROCESSORS sysconf(_SC_NPROCESSORS_ONLN)

enum SortingAlgorithm {QuickSort = 1, BubbleSort = 0};

static void *thread_start_quick(void *arg);
static void *thread_start_bubble(void *arg);
int cmpfunc (const void * a, const void * b);
void print_matrix(int **mat, int m, int n);
void bubblesort(int *mat, int n);
void quicksort(int *mat, int n);
void swap(int *x, int *y);

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)


int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

int **global_var_mat;

struct thread_info
{                                   /* Used as argument to thread_start_xyz() */
    pthread_t thread_id;            /* ID returned by pthread_create() */
    int thread_num;                 /* Application-defined thread # */
    int thread_matrix_row_position; /* Which position this thread will work on sorting */
    int number_of_elements;         /* Number of elements */
    double processing_time;         /* How many seconds it took for calculations */
    int startIndex;                 /* Starting index where thread will get tasks */
    int endIndex;                   /* Ending index where thread will stop getting tasks */
};

static void *thread_start_quick(void *arg)
{
    clock_t t;
    struct thread_info *tinfo = arg;

    for (size_t i = tinfo->startIndex; i < tinfo->endIndex; i++)
    {
        t = clock();

        quicksort(global_var_mat[i], tinfo->number_of_elements);
        // qsort(global_var_mat[i], tinfo->number_of_elements, sizeof(int), cmpfunc);

        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC;
        tinfo->processing_time += time_taken;
    }

    pthread_exit(EXIT_SUCCESS);
}

static void *thread_start_bubble(void *arg)
{
    clock_t t;
    struct thread_info *tinfo = arg;

    for (size_t i = tinfo->startIndex; i < tinfo->endIndex; i++)
    {
        t = clock();

        bubblesort(global_var_mat[i], tinfo->number_of_elements);

        t = clock() - t;
        double time_taken = ((double)t) / CLOCKS_PER_SEC;
        tinfo->processing_time += time_taken;
    }

    pthread_exit(EXIT_SUCCESS);
}

void quicksort(int *mat, int n)
{
    if (n < 2)
        return;

    int pivot = mat[n / 2];

    int i, j;
    for (i = 0, j = n - 1;; i++, j--)
    {
        while (mat[i] < pivot)
            i++;
        while (mat[j] > pivot)
            j--;

        if (i >= j)
            break;

        int temp = mat[i];
        mat[i] = mat[j];
        mat[j] = temp;
    }

    quicksort(mat, i);
    quicksort(mat + i, n - i);
}

void bubblesort(int *mat, int n)
{
    for (int step = 0; step < n - 1; ++step)
    {
        for (int i = 0; i < n - step - 1; ++i)
        {
            if (mat[i] > mat[i + 1])
            {
                swap(&mat[i], &mat[i + 1]);
            }
        }
    }
}

void swap(int *x, int *y)
{
    int t;
    t = *x;
    *x = *y;
    *y = t;
}

void print_matrix(int **mat, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%d ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(int argc, char const *argv[])
{
    int s, tnum, num_threads;
    struct thread_info *tinfo;

    int m = 40, n = 40;

    int processingPackSize = (m + NUMBER_OF_PROCESSORS - 1) / NUMBER_OF_PROCESSORS;
    if (processingPackSize <= 0)
    {
        processingPackSize = 1;
    }

    global_var_mat = (int **)malloc(m * sizeof(int *));
    for (int i = 0; i < m; i++)
    {
        global_var_mat[i] = (int *)malloc(n * sizeof(int));
    }

    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            global_var_mat[i][j] = rand() % 100 + 1;

    // printf("\nArrays in Random Order:\n");
    // print_matrix(global_var_mat, m, n);

    /* Preparing threads */

    num_threads = NUMBER_OF_PROCESSORS;

    /* Allocate memory for pthread_create() arguments */
    tinfo = calloc(num_threads, sizeof(struct thread_info));
    if (tinfo == NULL)
        handle_error("calloc");

    for (tnum = 0; tnum < num_threads; tnum++)
    {
        int startIndex = tnum * processingPackSize;
        int endIndex = fmin(startIndex + processingPackSize, m);

        tinfo[tnum].thread_matrix_row_position = tnum;
        tinfo[tnum].number_of_elements = n;
        tinfo[tnum].thread_num = tnum;
        tinfo[tnum].processing_time = 0;
        tinfo[tnum].startIndex = startIndex;
        tinfo[tnum].endIndex = endIndex;

        s = pthread_create(&tinfo[tnum].thread_id, NULL,
                           &thread_start_bubble, &tinfo[tnum]);
        if (s != 0)
            handle_error_en(s, "pthread_create");
    }

    printf("\nTimings for processing by each thread.\n");
    double sum = 0;
    for (tnum = 0; tnum < num_threads; tnum++)
    {
        s = pthread_join(tinfo[tnum].thread_id, NULL);
        printf("Thread Using Bubble Sort [id: %d], Accumulated time: [%f seconds].\n", tinfo[tnum].thread_num, tinfo[tnum].processing_time, tinfo[tnum].thread_matrix_row_position);

        if (s != 0)
            handle_error_en(s, "pthread_join");

        sum += tinfo[tnum].processing_time;
    }
    printf("\nSum of timings %f seconds.\n", sum);

    // printf("\nSorted Arrays in Ascending Order:\n");
    // print_matrix(global_var_mat, m, n);

    for (int i = 0; i < m; ++i)
        free(global_var_mat[i]);

    free(tinfo);
    free(global_var_mat);
    exit(EXIT_SUCCESS);
}
