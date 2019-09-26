/* A common header file to describe the shared memory we wish to pass about. */
enum SortingAlgorithm {QuickSort = 1, BubbleSort = 0};

struct shared_use_st
{
    int m;
    int n;
    volatile int unfinished_tasks_counter;
};

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

void print_matrix2(int *mat, int m, int n)
{
    for (int i = 0; i < m; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            printf("%d ", mat[i * m + j]);
        }
        printf("\n");
    }
    printf("\n");
}
