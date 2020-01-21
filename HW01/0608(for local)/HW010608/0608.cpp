#include "mpi.h"
#include "iostream"
using namespace std;
#define SIZE 100000

int main(int argc, char**argv) {
	int		p, id;

	double	latency = 0.00;
	double	beandwidth = 0.00;
	double	start_time, end_time;
	double	start_bandwidth, end_bandwidth;

	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);	//	number of processor
	MPI_Comm_rank(MPI_COMM_WORLD, &id); //	number of process
	start_time = MPI_Wtime();
	for (int i = 1; i <= 1000000000; i = i * 10) {
		char *message = new char[i];


		if (id == 0) {

			MPI_Barrier(MPI_COMM_WORLD);
			start_bandwidth = MPI_Wtime();

			MPI_Send(&message, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD);
			MPI_Recv(&message, 1, MPI_CHAR, 1, 0, MPI_COMM_WORLD, &status);

			end_bandwidth = MPI_Wtime();
			end_time = MPI_Wtime();
			if (i == 1) {
				
				latency = end_time - start_time;
			}
			else{
				printf("size: %d total time: %.10f bandwidth(Byte): %.10f\n", i, end_time - start_time, i / ((end_time - start_time) - (latency)));
			}
		}
		else {

			MPI_Barrier(MPI_COMM_WORLD);

			MPI_Recv(&message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
			MPI_Send(&message, 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);

		}
//		delete[] message; //in local, comment this line to avoid error interrupt (?
	}
	MPI_Finalize();

	printf("latency: %.10f", latency);
	return 0;
}

