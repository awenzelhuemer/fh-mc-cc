#include <mpi.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

void readData(char* fname, int lines,int** nums, int* lens) {
	int i = -1, j = 0;
	FILE* fp;
	char ch;
	int newLine=1;
	int val =0 ;

	const int DEBUG=0;

	if (fp = fopen(fname, "r")) {
        if(DEBUG) printf("Opened file\n");
		while (fscanf(fp, "%d%c", &val,&ch) != EOF) {
            if(DEBUG) printf("Read '%d' '%c'\n",val,ch);
			if(newLine==1) {
				i++;
				if(DEBUG) printf("New line found (I am now in line %d)...",i);
				nums[i]=(int*)malloc(sizeof(int)*val);
				lens[i]=val;
                if(DEBUG) printf("allocated array nums[%d] of size %d\n",i,val);
				j=0;
				newLine=0;
			} else {
                if(DEBUG) printf("Storing value %d at num[%d][%d]...",val,i,j);
				nums[i][j]=val;
                if(DEBUG) printf("done\n");
				if(ch=='\n') {
                    if(DEBUG) printf("Char was new line!\n\n");
					newLine=1;
				}
                j++;
			}
		}
		fclose(fp);
	}
}

int countEvenNumbers(int* arrOfNumbers, int arrSize) {
  int count = 0;
  for (int i = 0; i < arrSize; i++) {
    if (arrOfNumbers[i] % 2 == 0) {
      count++;      
    }
  }
  return count;
}

int main(int argc, char** argv) {
  const int FILE_LINES = 10;

    int even_count = 0;

    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if (world_size != FILE_LINES) {
        printf("This program requires exactly 10 processes (%d provided).\n", world_size);
        MPI_Abort(MPI_COMM_WORLD, -1);
    }

    int** numbers = (int**)malloc(sizeof(int*) * FILE_LINES);
    int lines[FILE_LINES];

    if (world_rank == 0) {
        readData("input.txt", FILE_LINES, numbers, lines);
        even_count = countEvenNumbers(numbers[0], lines[0]);

        for (int i = 1; i < world_size; i++) {
            MPI_Send(numbers[i], lines[i], MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    } else {
        int number_size;
        MPI_Status status;
        MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &number_size);

        int* data = (int*)malloc(sizeof(int) * number_size);

        MPI_Recv(data, number_size, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);

        even_count = countEvenNumbers(data, number_size);

        printf("P%d: Amount of even numbers is %d/%d\n", world_rank, even_count, number_size);

        free(data);
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    // ring around rosie
    int token;
    if (world_rank == 0) {
        token = even_count;
    } else {
        MPI_Recv(&token, 1, MPI_INT, world_rank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        token += even_count;
    }

    MPI_Send(&token, 1, MPI_INT, (world_rank + 1) % world_size, 1, MPI_COMM_WORLD);

    if (world_rank == 0) {
        MPI_Recv(&token, 1, MPI_INT, world_size - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                  printf("P%d: Total even count is %d\n", world_rank, token);
    }

    MPI_Finalize();

    return 0;
}