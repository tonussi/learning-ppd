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
#include <sys/mman.h>
#include <signal.h>

#include "shm_com.h"

void print_matrix(int **mat, int m, int n);

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

int main(int argc, char const *argv[])
{
    int *unfinished_tasks_data;

    int shmid;
    int shmid2;
    key_t keyv = 1337;
    key_t kayv = 1447;
    void *shared_memory = (void *)0;
    int *shared_memory2 = (int *)0;
    struct shared_use_st *shared_stuff;

    srand(time(NULL));

    int m = 40, n = 40;

    /* Preparing shared memory */
    shmid = shmget(keyv, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shmid2 = shmget(kayv, sizeof(int) * (m * n), 0666 | IPC_CREAT);
    if (shmid2 == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory = (int *)shmat(shmid, NULL, 0);
    if (shared_memory == (int *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    shared_memory2 = (int *)shmat(shmid2, NULL, 0);
    if (shared_memory2 == (int *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }

    shared_stuff = (struct shared_use_st *)shared_memory;

    shared_stuff->m = m;
    shared_stuff->n = n;
    shared_stuff->unfinished_tasks_counter = m;

    /* Preparing tasks */
    // unfinished_tasks_data = (int **)malloc(m * sizeof(int *));
    // for (int i = 0; i < m; i++)
    //     unfinished_tasks_data[i] = (int *)malloc(n * sizeof(int));

    unfinished_tasks_data = malloc(sizeof(int) * (m * n));

    unfinished_tasks_data = shared_memory2;

    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            unfinished_tasks_data[i * m + j] = rand() % 100 + 1;

    // unfinished_tasks_data[i][j] = rand() % 100 + 1;

    printf("Memory 1 attached at %X\n", (int)shared_memory);

    printf("Memory 2 attached at %X\n", (int)shared_memory2);

    print_matrix2(unfinished_tasks_data, shared_stuff->m, shared_stuff->n);

    while (shared_stuff->unfinished_tasks_counter > 0);

    print_matrix2(unfinished_tasks_data, shared_stuff->m, shared_stuff->n);

    // for (int i = 0; i < m; ++i)
    //     free(unfinished_tasks_data[i]);

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

    free(unfinished_tasks_data);
    exit(EXIT_SUCCESS);
}
