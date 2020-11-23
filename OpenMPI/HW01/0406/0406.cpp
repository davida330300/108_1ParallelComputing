#include "mpi.h"
#include "iostream"
using namespace std;

int main(int argc, char** argv) {
	int id;
	int p;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	printf("Hello world, from process < %d >\n", id);
	fflush(stdout);

	MPI_Finalize();
	return 0;
}