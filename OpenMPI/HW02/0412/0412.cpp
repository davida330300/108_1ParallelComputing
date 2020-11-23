#include "mpi.h"
#include "iostream"
#include <stdlib.h>
#include "math.h"
#define n 50
#define a 0
#define b 1
double f(double d);

using namespace std;
int main(int argc, char**argv) {
	
	int p, id;
	double width;
	double localAreaResult = 0.0, globalAreaResult = 0.0;
	double startTime, endTime;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	startTime = MPI_Wtime();

	width = (double)(1.0/ (double)n);

	for (int i = (id+1); i <= (n/2); i += p) {	
		localAreaResult += 4.0 * f((2 * i - 1)*width)+ 2.0 * f((2 * i)*width);
	}

	

	MPI_Reduce(&localAreaResult, &globalAreaResult, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	endTime = MPI_Wtime();

	

	MPI_Finalize();

	printf("processor %d : localAreaResult is : %13.15f\n", id, localAreaResult);
	if (id == 0) {
		printf("globalAreaResult is : %13.15f\n", globalAreaResult);
	}
	globalAreaResult = ((f(0) - f(1) + globalAreaResult))/ (3*n);
	if (id == 0) {
		printf("Approximation of pi: %13.15f vs 3.141592653589793\n", globalAreaResult);
		printf("total time : %2.10f\n",endTime - startTime);
	}
	return 0;
}
double f(double d) {

	return 4.0 / (1.0 + (d * d));
}
/*
	from i = 1 to i = n/2 => 4f(x2i-1)+2f(x2i)	--(1)
	(f(x0)-f(xn)+(1))/3n

*/
