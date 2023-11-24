#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main() {
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != 2) {
        printf("This program requires exactly 2 processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    const int MAX_NUMBERS = 100;
    int *numbers;
    int number_amount;
    if (world_rank == 0) {
        // It's this process' turn to send
        srand(time(NULL));
        number_amount = (rand() / (float)RAND_MAX) * MAX_NUMBERS;
        numbers = malloc(number_amount * sizeof(int));
        MPI_Send(numbers, number_amount, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("0 sent %d numbers to 1\n", number_amount);
    } else if (world_rank == 1) {
        // Use MPI_Probe to determine the message size
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &number_amount);
        numbers = malloc(number_amount * sizeof(int));

        // Receive the actual message
        MPI_Recv(numbers, number_amount, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("1 received %d numbers from 0. Message source = %d, tag = %d\n", number_amount, status.MPI_SOURCE, status.MPI_TAG);

        // Free the dynamically allocated memory
        free(numbers);
    }

    MPI_Finalize();

    return 0;
}
