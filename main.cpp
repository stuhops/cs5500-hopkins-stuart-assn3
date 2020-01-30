#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

using namespace std;

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv){

    int rank, size;
    const int ARR_SIZE = 10; 
    int data[ARR_SIZE];
    const int MAX_TIME = 10;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW,&rank);
    MPI_Comm_size(MCW,&size);
    MPI_Status mystatus;

    // Use process 0 as the master process
    if(!rank) {
      srand(time(NULL));
      // Initialize the array
      // Start at 1 because this process is already rank 0
      for(int i = 1; i < ARR_SIZE; i++) {
        data[i] = rand() % ARR_SIZE;
        cout << " " << data[i];
      }
      cout << "\n\n\n";

      // Send out random chunks to processes
      int size_left = ARR_SIZE;
      int TMP_ARR_SIZE;
      for(int i = 1; i < size; i++) {
        if(!size_left) break;


        if(i+1 != size) {
          int prev_size_left = size_left;
          size_left -= rand() % size_left;
          TMP_ARR_SIZE= prev_size_left - size_left;
          sleep(1);
          MPI_Send(&data[size_left], TMP_ARR_SIZE, MPI_INT, i, 0, MCW);
        }
        else
          MPI_Send(&data[1], size_left-1, MPI_INT, i, 0, MCW);
      }

    }

    // Sort array in all processes except 0
    else {
      MPI_Probe(MPI_ANY_SOURCE, 0, MCW, &mystatus);
      int count;
      MPI_Get_count(&mystatus, MPI_INT, &count);
      MPI_Recv(data, count, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);

      cout << "Array received by rank " << rank << " ... Received a count of " << count << endl;
      for(int i = 0; i < count; i++)
        cout << " " << data[i];

      cout << endl;
    }

    // MPI_Send(data, 1, MPI_INT, rand() % size, 0, MCW);
    // MPI_Recv(data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, &mystatus);

    MPI_Finalize();
}