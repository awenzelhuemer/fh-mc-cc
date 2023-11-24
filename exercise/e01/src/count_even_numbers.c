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

    int evenCount = 0;

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

        for (int i = 1; i < world_size; i++) {
            MPI_Send(&lines[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(numbers[i], lines[i], MPI_INT, i, 1, MPI_COMM_WORLD);
        }
    } else {
        MPI_Recv(&lines[world_rank], 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        int* data = (int*)malloc(sizeof(int) * lines[world_rank]);

        MPI_Recv(data, lines[world_rank], MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        evenCount = countEvenNumbers(data, lines[world_rank]);

        printf("P%d: Amount of even numbers is %d/%d\n", world_rank, evenCount, lines[world_rank]);

        free(data);
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    int totalEvenCount = 0;

    if (world_rank == 0) {
        totalEvenCount = countEvenNumbers(numbers[world_rank], lines[world_rank]);
        printf("P%d: Amount of even numbers is %d/%d\n", world_rank, totalEvenCount, lines[world_rank]);
        for (int i = 1; i < world_size; i++) {
            MPI_Recv(&evenCount, 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            totalEvenCount += evenCount;
        }

        printf("P%d: Total even count is %d\n", world_rank, totalEvenCount);
    } else {
        MPI_Send(&evenCount, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
    }

    MPI_Finalize();

    return 0;
}