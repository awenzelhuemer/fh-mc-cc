#include <stdio.h>
#include <mpi.h>

int main() {
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Check if there are exactly 8 processes
    if (world_size != 8) {
        printf("This program requires exactly 8 processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Calculate "color" to assign processes in pairs to communicators
    int color = world_rank / 2;

    // Create 4 new communicators, each holding two processes
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &new_comm);

    int new_rank, new_size;
    MPI_Comm_rank(new_comm, &new_rank);
    MPI_Comm_size(new_comm, &new_size);

    // Send a message from process with rank 0 to rank 1 in each new communicator
    int send_value = -1;
    if (new_rank == 1) {
        MPI_Send(&send_value, 1, MPI_INT, 0, 0, new_comm);
    } else if (new_rank == 0) {
        int recv_value;
        MPI_Recv(&recv_value, 1, MPI_INT, 1, 0, new_comm, MPI_STATUS_IGNORE);
        printf("Communicator %d/%d: Rank %d received value %d from rank %d (color: %d)\n",
               new_rank, new_size, world_rank, recv_value, 1, color);
    }

    // Output rank, size, original rank, and color
    printf("Communicator %d/%d: Rank %d (color: %d)\n", new_rank, new_size, world_rank, color);

    // Free the communicator at the end of the program
    MPI_Comm_free(&new_comm);

    MPI_Finalize();

    return 0;
}
