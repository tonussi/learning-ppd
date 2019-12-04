#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <time.h>




#define DEFAULT_ARRAY_SIZE 10
#define MAX_NUM_SIZE 1000
#define MASTER 0




typedef struct
{
    int world_rank,
        world_size,
        array_size,
        stride,
        debug,
        maximum_value,
        print_array,
        print_ranges,
        showtime;
} Configuration;





typedef struct
{
    int *array;
    int index;
    int size;
} Bucket;





void bucket_insert(Bucket *b, int x)
{
    b->array[b->index] = x;
    b->index = b->index + 1;
}





int compare(const void *n1, const void *n2)
{
    return (*(int *)n1 - *(int *)n2);
}





typedef struct
{
    int range_min;
    int range_max;
} Stride;




void print_buckets(Bucket ** buckets_rand_nums, Stride ** strides, Configuration *p) {
    for (int k = 0; k < p->world_size; k++)
    {
        fprintf(
            stdout,
            "Bucket size [%d] range [%d]-[%d] -> ",
            buckets_rand_nums[k]->index,
            strides[k]->range_min,
            strides[k]->range_max);

        for (int t = 0; t < buckets_rand_nums[k]->size; t++)
        {
            fprintf(stdout, "%d ", buckets_rand_nums[k]->array[t]);
        }

        fprintf(stdout, "\n");
    }
}





void print_bucket_array(int * array, int size) {
    fprintf(stdout, "Bucket size [%d] -> ", size);
    for (int t = 0; t < size; t++)
        fprintf(stdout, "%d <~", array[t]);
    fprintf(stdout, "\n");
}




int random_int(int min, int max)
{
    return min + rand() % (max + 1 - min);
}




void create_rand_nums(int *array, Configuration *p)
{
    for (int i = 0; i < p->array_size; i++)
    {
        array[i] = random_int(0, p->maximum_value);
    }
}




Stride ** make_strides(Configuration *p)
{
    Stride **strides = NULL;
    strides = (Stride **)malloc(sizeof(Stride) * p->world_size);

    for (int source = 0; source < p->world_size; source++)
    {
        int range_min = (p->maximum_value * source) / p->world_size;
        int range_max = (p->maximum_value * (source + 1)) / p->world_size;
        strides[source]->range_min = range_min;
        strides[source]->range_max = range_max;
    }
    return strides;
}





Bucket ** semi_bucket_sort(int *array, Stride **strides, Configuration *p)
{
    Bucket ** buckets_rand_nums = (Bucket **)malloc(sizeof(Bucket) * p->world_size);

    for (int i = 0; i < p->world_size; i++)
    {
        buckets_rand_nums[i] = (Bucket *)malloc(sizeof(Bucket));
        buckets_rand_nums[i]->index = 0;
        buckets_rand_nums[i]->size = 0;
    }

    for (int s = 0; s < p->array_size; s++)
        for (int j = 0; j < p->world_size; j++)
            if (array[s] >= strides[j]->range_min && array[s] < strides[j]->range_max)
                buckets_rand_nums[j]->size++;

    for (int i = 0; i < p->world_size; i++)
    {
        buckets_rand_nums[i]->array = (int *)malloc(sizeof(int) * buckets_rand_nums[i]->size);
    }

    for (int s = 0; s < p->array_size; s++)
        for (int j = 0; j < p->world_size; j++)
            if (array[s] >= strides[j]->range_min && array[s] < strides[j]->range_max)
                bucket_insert(buckets_rand_nums[j], array[s]);

    if (p->debug) {
        print_buckets(
            buckets_rand_nums,
            strides,
            p
        );
    }

    free(strides);
    return buckets_rand_nums;
}




int worker(Configuration *p)
{
    double start, end;

    srand(time(NULL));

    int *rand_nums = NULL;
    Bucket **buckets_rand_nums = NULL;
    Stride **strides = NULL;

    if (p->world_rank == MASTER)
    {
        start = MPI_Wtime();

        rand_nums = (int *)malloc(sizeof(int) * p->array_size);
        create_rand_nums(rand_nums, p);
        strides = make_strides(p);
        buckets_rand_nums = semi_bucket_sort(rand_nums, strides, p);
        free(rand_nums);
    }




    MPI_Request * requests = (MPI_Request *)malloc(sizeof(MPI_Request) * p->world_size);




    if (p->world_rank == MASTER)
    {
        for (int source = 0; source < p->world_size; source++)
        {




            MPI_Isend(
                &buckets_rand_nums[source]->index,
                1,
                MPI_INT,
                source,
                MASTER,
                MPI_COMM_WORLD,
                &requests[source]);





            MPI_Isend(
                buckets_rand_nums[source]->array,
                buckets_rand_nums[source]->index,
                MPI_INT,
                source,
                MASTER,
                MPI_COMM_WORLD,
                &requests[source]);
        }
    }


    MPI_Status status;
    int size = 0;


    // MPI_Wait(&requests[p->world_rank], &status);


    MPI_Recv(
        &size,
        1,
        MPI_INT,
        MASTER,
        MASTER,
        MPI_COMM_WORLD,
        &status);




    Bucket destination_bucket;
    int * my_bucket_array = (int *)malloc(sizeof(int) * size * 4.0);
    destination_bucket.array = my_bucket_array;
    destination_bucket.index = 0;



    // Muito Obrigado Para: Sadi Júnior Domingos Jacinto (17100922) pela ajuda com os RECEIVES

    if (size != 0) {

        MPI_Recv(
            destination_bucket.array,
            size,
            MPI_INT,
            MASTER,
            MASTER,
            MPI_COMM_WORLD,
            &status
        );



        if (p->print_array) {
            printf("\nBucket Desordenado: ");
            for (int i = 0; i < size; i++)
            {
                printf("%d.", destination_bucket.array[i]);
            }
            printf("\n");
        }



        qsort(destination_bucket.array, size, sizeof(int), compare);




        if (p->print_array) {
            printf("\nBucket Ordenado: ");
            for (int i = 0; i < size; i++)
            {
                printf("%d.", destination_bucket.array[i]);
            }
            printf("\n");
        }




        MPI_Isend(
            destination_bucket.array,
            size,
            MPI_INT,
            MASTER,
            p->world_rank,
            MPI_COMM_WORLD,
            &requests[p->world_rank]
        );
    }




    if (p->world_rank == MASTER)
    {




        Bucket ** buckets = (Bucket **)malloc(sizeof(Bucket) * p->world_size);

        for (int i = 0; i < p->world_size; i++)
        {
            buckets[i] = (Bucket *)malloc(sizeof(Bucket));
            buckets[i]->array = (int *)malloc(sizeof(int) * buckets_rand_nums[i]->index);
            buckets[i]->index = buckets_rand_nums[i]->index;
        }




        for (int source = 0; source < p->world_size; source++)
        {
            MPI_Recv(
                buckets[source]->array,
                buckets[source]->index,
                MPI_INT,
                source,
                source,
                MPI_COMM_WORLD,
                &status
            );
        }




        if (p->print_array)
        {
            fprintf(stdout, "\nArray Ordenado: $ ");
            for (int source = 0; source < p->world_size; source++)
            {
                for (int t = 0; t < buckets_rand_nums[source]->index; t++)
                {
                    fprintf(stdout, "%d ", buckets[source]->array[t]);
                }
            }
            fprintf(stdout, "$\n");
        }

        free(buckets);


        end = MPI_Wtime();

        if (p->showtime) {
            fprintf(stdout, "%f", end - start);
        }
    }

    free(requests);
    free(buckets_rand_nums);
    free(destination_bucket.array);
}




void parse_args(int argc, char **argv, Configuration *p)
{
    int c = 0;
    while ((c = getopt(argc, argv, "rs:hm:tdp")) != -1)
    {
        switch (c)
        {
        case 'r':
            p->print_ranges = 1;
            break;
        case 's':
            p->array_size = atoi(optarg);
            break;
        case 'm':
            p->maximum_value = atoi(optarg);
            break;
        case 'h':
            if (p->world_rank == 0)
            {
            }
            exit(0);
        case 'p':
            p->print_array = 1;
            break;
        case 't':
            p->showtime = 1;
            break;
        case 'd':
            p->debug = 1;
            break;
        default:
            break;
        }
    }
    return;
}





int main(int argc, char *argv[])
{
    int world_rank;
    int world_size;

    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        fprintf(stderr, "Unable to initialize MPI!\n");
        return -1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    Configuration p;

    p.showtime = 0;
    p.debug = 0;
    p.print_array = 0;
    p.print_ranges = 0;
    p.maximum_value = MAX_NUM_SIZE;
    p.world_rank = world_rank;
    p.world_size = world_size;
    p.array_size = DEFAULT_ARRAY_SIZE;

    parse_args(argc, argv, &p);

    worker(&p);

    MPI_Finalize();

    return 0;
}
