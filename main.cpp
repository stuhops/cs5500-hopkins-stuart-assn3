#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

using namespace std;

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv){

    int rank, size;
    const int ARR_SIZE = 64; 
    int data[ARR_SIZE];
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW,&rank);
    MPI_Comm_size(MCW,&size);
    MPI_Status mystatus;

    // Use process 0 as the master process
    if(!rank) {
      srand(time(NULL));
      // Initialize the array
      // Start at 1 because this process is already rank 0
      cout << "\nBeginning Array:\n";
      for(int i = 0; i < ARR_SIZE; i++) {
        data[i] = rand() % ARR_SIZE;
        cout << " " << data[i];
      }
      cout << "\n";

      // Send out random chunks to processes
      int size_left = ARR_SIZE;
      int TMP_ARR_SIZE;
      for(int i = 1; i < size; i++) {
        if(!size_left) break;


        if(i+1 != size) {
          int prev_size_left = size_left;
          size_left -= rand() % size_left;
          TMP_ARR_SIZE= prev_size_left - size_left;
          MPI_Send(&data[size_left], TMP_ARR_SIZE, MPI_INT, i, 0, MCW);
        }
        else
          MPI_Send(&data[1], size_left-1, MPI_INT, i, 0, MCW);
      }

    }

    // Work processes sort array 
    else {
      MPI_Probe(MPI_ANY_SOURCE, 0, MCW, &mystatus);
      int count;
      MPI_Get_count(&mystatus, MPI_INT, &count);
      MPI_Recv(data, count, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);

      sort(&data[0], &data[count]);
      MPI_Send(&data[0], 1, MPI_INT, 0, 0, MCW);
    }

    // Combine sorted arrays
    // MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);
    if(!rank) {
      // for(int i = 1; i < size; i++)  // Start at 1 because this is process 0
      sort(&data[0], &data[ARR_SIZE]);
      cout << "\n\nEnd result: " << endl;
      for(int i = 0; i < ARR_SIZE; i++)
        cout << " " << data[i];
      cout << "\n\n";
    }

    MPI_Finalize();
}