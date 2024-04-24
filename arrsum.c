#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define n 10

int a[] = { 4, 5, 6, 7, 8, 9, 0, 1, 2, 3 };
int a2[1000];

int main(int argc, char* argv[])
{
    int pid, np,
        elements_per_process,
        n_elements_recieved;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    MPI_Comm_size(MPI_COMM_WORLD, &np);

    if (pid == 0) {
        int index, i;
        elements_per_process = n / np;

        if (np > 1) {
            for (i = 1; i < np - 1; i++) {
                index = i * elements_per_process;

                MPI_Send(&elements_per_process, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                MPI_Send(&a[index], elements_per_process, MPI_INT, i, 0, MPI_COMM_WORLD);
            }

            index = i * elements_per_process;
            int elements_left = n - index;

            MPI_Send(&elements_left, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&a[index], elements_left, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int total = 0;
        printf("Master elements are: ( ");
        for (i = 0; i < elements_per_process; i++)
            printf("%d ", a[i]);
        printf(")\n");

        for (i = 0; i < elements_per_process; i++)
            total += a[i];
        printf("Sum of master is: %d\n", total);

        int tmp;
        for (i = 1; i < np; i++) {
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int sender = status.MPI_SOURCE;

            total += tmp;
        }

        printf("Sum of array is: %d\n", total);
    } else {
        MPI_Recv(&n_elements_recieved, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&a2, n_elements_recieved, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

        int partial_sum = 0;
        printf("Slave elements are: ( ");
        for (int i = 0; i < n_elements_recieved; i++)
            printf("%d ", a2[i]);
        printf(")\n");

        for (int i = 0; i < n_elements_recieved; i++)
            partial_sum += a2[i];
        printf("Sum of slave element is: %d\n", partial_sum);

        MPI_Send(&partial_sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
