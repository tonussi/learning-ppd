#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <fcntl.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "shm_com.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))

#define NUMBER_OF_PROCESSORS sysconf(_SC_NPROCESSORS_ONLN)

sem_t mutex;

void *thread_start_quick(void *arg);
void *thread_start_bubble(void *arg);
int cmpfunc(const void *a, const void *b);
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

int cmpfunc(const void *a, const void *b)
{
    return (*(int *)a - *(int *)b);
}

struct process_info
{ /* Used as argument to process_start_xyz() */
    int m;
    int n;
    int process_num;        /* Application-defined process # */
    int processingPackSize; /* Which position this process will work on sorting */
    int number_of_elements; /* Number of elements */
    double processing_time; /* How many seconds it took for calculations */
    int startIndex;         /* Starting index where process will get tasks */
    int endIndex;           /* Ending index where process will stop getting tasks */
};

void start_quick(void *arg, void *arg1)
{
    clock_t t;
    struct process_info *tinfo = arg;
    int *arr = arg1;

    t = clock();
    int j = 0;

    // printf("\n[son] pid %d from [parent] pid %d and value ou i=%d, startIndex %d endIndex %d.\n",
    // getpid(), getppid(), tinfo->process_num, tinfo->startIndex, tinfo->endIndex);

    for (int i = tinfo->startIndex; i < tinfo->endIndex; i++)
    {
        // 0 * 5 = 0 1 2 3 4
        // 1 * 5 = 5 6 7 8 9
        // 2 * 5 = 10 11 12 13 14
        // 3 * 5 = 15 16 17 18 19
        j = i * tinfo->number_of_elements;
        quicksort(&arr[j], tinfo->number_of_elements);
        // qsort(&arr[j], tinfo->number_of_elements, sizeof(int), cmpfunc);
    }

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    tinfo->processing_time += time_taken;

    printf("time i took %f sorting %d elements (quick sort)\n", tinfo->processing_time, (tinfo->endIndex - tinfo->startIndex) * tinfo->number_of_elements);
}

void start_bubble(void *arg, void *arg1)
{
    clock_t t;
    struct process_info *tinfo = arg;
    int *arr = arg1;

    t = clock();
    int j = 0;

    // printf("\n[son] pid %d from [parent] pid %d and value ou i=%d, startIndex %d endIndex %d.\n",
    // getpid(), getppid(), tinfo->process_num, tinfo->startIndex, tinfo->endIndex);

    for (int i = tinfo->startIndex; i < tinfo->endIndex; i++)
    {
        // 0 * 5 = 0 1 2 3 4
        // 1 * 5 = 5 6 7 8 9
        // 2 * 5 = 10 11 12 13 14
        // 3 * 5 = 15 16 17 18 19
        j = i * tinfo->number_of_elements;
        bubblesort(&arr[j], tinfo->number_of_elements);
    }

    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    tinfo->processing_time += time_taken;

    printf("time i took %f sorting %d elements (bubble sort)\n", tinfo->processing_time, (tinfo->endIndex - tinfo->startIndex) * tinfo->number_of_elements);
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

int main(int argc, char const *argv[])
{
    int shmid;
    int shmid2;

    void *shared_memory = (void *)0;
    void *shared_memory2 = (void *)0;

    struct shared_use_st *shared_stuff;

    key_t keyv = 1337;
    key_t kayv = 1447;

    shmid = shmget(keyv, sizeof(struct shared_use_st), 0666 | O_RDWR | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = shmat(shmid, (void *)0, 0);
    if (shared_memory == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    shared_stuff = (struct shared_use_st *)shared_memory;

    shmid2 = shmget(kayv, sizeof(int) * (shared_stuff->m * shared_stuff->n), 0666 | O_RDWR | IPC_CREAT);
    if (shmid2 == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory2 = (int *)shmat(shmid2, (void *)0, 0);
    if (shared_memory2 == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    int processingPackSize = (shared_stuff->m + NUMBER_OF_PROCESSORS - 1) / NUMBER_OF_PROCESSORS;
    if (processingPackSize <= 0)
    {
        processingPackSize = 1;
    }

    /* Preparing processes 
    int sem_init(sem_t *sem, int pshared, unsigned int value);
    */
    if (sem_init(&mutex, 1, 1) < 0)
    {
        perror("semaphore initilization");
        exit(0);
    }

    pid_t pids[NUMBER_OF_PROCESSORS];

    printf("Memory 1 attached at %X\n", (int)shared_memory);

    printf("Memory 2 attached at %X\n", (int)shared_memory2);

    for (size_t i = 0; i < sizeof(pids) / sizeof(pids[0]); i++)
    {

        int startIndex = i * processingPackSize;
        int endIndex = min(startIndex + processingPackSize, shared_stuff->m);

        if ((pids[i] = fork()) < 0)
        {
            perror("fork(2) failed");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0)
        {
            struct process_info *tinfo;

            /* Allocate memory for pthread_create() arguments */
            tinfo = calloc(1, sizeof(struct process_info));
            if (tinfo == NULL)
                handle_error("calloc");

            tinfo->process_num = i;
            tinfo->m = shared_stuff->m;
            tinfo->n = shared_stuff->n;
            tinfo->number_of_elements = shared_stuff->n;
            tinfo->processingPackSize = tinfo->endIndex - tinfo->startIndex;
            tinfo->startIndex = startIndex;
            tinfo->endIndex = endIndex;

            printf("\n[son] pid %d from [parent] pid %d and value ou i=%ld, startIndex %d endIndex %d.\n",
                   getpid(), getppid(), i, startIndex, endIndex);

            // printf("\n\n%d\n\n", shared_stuff->unfinished_tasks_data[0][0]);
            // printf("@@@@@@@@@ %d\n", tinfo->process_num);
            // print_matrix2((int *)shared_memory2, shared_stuff->m, shared_stuff->n);
            // printf("@@@@@@@@@ %d\n", tinfo->process_num);

            // Critical section
            // printf("Reducing task is counter %d.\n", shared_stuff->unfinished_tasks_counter);

            start_bubble(tinfo, (int *)shared_memory2);

            sem_wait(&mutex);
            shared_stuff->unfinished_tasks_counter -= (tinfo->endIndex - tinfo->startIndex);
            sem_post(&mutex);

            if (shmdt(shared_memory2) == -1)
            {
                fprintf(stderr, "shmdt failed\n");
                exit(EXIT_FAILURE);
            }

            if (shmctl(shmid2, IPC_RMID, 0) == -1)
            {
                fprintf(stderr, "shmctl(IPC_RMID) failed\n");
                exit(EXIT_FAILURE);
            }

            if (shmdt(shared_memory) == -1)
            {
                fprintf(stderr, "shmdt failed\n");
                exit(EXIT_FAILURE);
            }

            if (shmctl(shmid, IPC_RMID, 0) == -1)
            {
                fprintf(stderr, "shmctl(IPC_RMID) failed\n");
                exit(EXIT_FAILURE);
            }

            free(tinfo);
            exit(EXIT_SUCCESS);
        }
    }

    for (size_t i = 0; i < sizeof(pids) / sizeof(pids[0]); i++)
        if (waitpid(pids[i], NULL, 0) < 0)
            perror("waitpid(2) failed");

    if (shmdt(shared_memory2) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid2, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmdt(shared_memory) == -1)
    {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if (shmctl(shmid, IPC_RMID, 0) == -1)
    {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
