#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
  MPI_Init(NULL, NULL);

  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

  if(world_size < 2) {
    printf("These program requires at least 2 processes but only %d provided.\n",world_size);
    MPI_Abort( MPI_COMM_WORLD , -1);
  }
  printf("Current Process %d!\n",world_rank);
  
  int token;
  if (world_rank == 0) {
    token = -1;
  } else {
    printf("[DEBUG]: P%d - Receiving\n",world_rank);
    MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("[DEBUG]: P%d - Received token %d from P%d\n", world_rank, token, world_rank - 1);
  }

  printf("[DEBUG]: P%d - Sending token\n", world_rank);
  MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 0, MPI_COMM_WORLD);
  printf("[DEBUG]: P%d - Sending done.\n", world_rank);
    

  if (world_rank == 0) {
    MPI_Recv(&token, 1, MPI_INT, world_size - 1, 0,MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("[DEBUG]: P%d - Received token %d from P%d\n", world_rank, token, world_size - 1);
    printf("Ring closed\n");
  }

  MPI_Finalize();    
}