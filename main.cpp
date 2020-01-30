#include <iostream>
#include <mpi.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

#define MCW MPI_COMM_WORLD

int main(int argc, char **argv){

    int rank, size;
    int cnt = -1;
    const int MAX_TIME = 10;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW,&rank);
    MPI_Comm_size(MCW,&size);

    // Use process 0 to initialize the count
    if(!rank && cnt == -1) {
      srand(5337228);
      cnt = rand() % MAX_TIME; 
      MPI_Send(&cnt, 1, MPI_INT, rand() % size, 0, MCW);
    }

    // While loop that receives, decrements, and passes the potato
    while(true) {
      MPI_Recv(&cnt, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW,MPI_STATUS_IGNORE);

      if(!cnt) {
        cout << "I am " << rank << " of " << size << "; got a countdown of " << cnt << " and I am the LOSER." << endl;
        --cnt;
        MPI_Send(&cnt, 1, MPI_INT, size + cnt, 0, MCW);  // Sends to the highest rank to begin termination
      }
      else if(cnt > 0) {
        cout << "I am " << rank << " of " << size << "; got a countdown of " << cnt << endl;
        sleep(1);
        --cnt;
        MPI_Send(&cnt, 1, MPI_INT, rand() % size, 0, MCW);
      }
      else {
        // Decrement though all precesses to terminate while loop
        --cnt;
        int next_process = size + cnt;
        if(next_process > -1) {
          MPI_Send(&cnt, 1, MPI_INT, next_process, 0, MCW);
        }
        break;
      }
    }

    MPI_Finalize();
}