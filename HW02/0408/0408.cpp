#include "mpi.h"
#include "iostream"
#include "stdlib.h"
#include "math.h"
#define upperbound 1000000
bool isPrime(int n);
void process(int rank, int size, int max, int *startNum, int *endNum);
using namespace std;


int main(int argc, char**argv) {

	int		p, id;
	int		n = upperbound;
	int		startNum, endNum;
	bool	currIsPrime = false, nextIsPrime = false;
	int		localPrimePairNum=0, globalPrimePairNum=0;
	double	startTime, endTime;


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	MPI_Barrier(MPI_COMM_WORLD);
	startTime = MPI_Wtime();
	process(id, p, n, &startNum, &endNum);

	for (int i = startNum; i <= endNum; i += 2) {
		nextIsPrime = isPrime(i);
		if (currIsPrime && nextIsPrime) {
			localPrimePairNum++;
		}
		currIsPrime = nextIsPrime;
	}	
	if (id == 0) localPrimePairNum--;
	printf("processor %d : The number prime pair in %d and %d is:  %d\n",id, startNum, endNum, localPrimePairNum);

	MPI_Reduce(&localPrimePairNum, &globalPrimePairNum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	MPI_Barrier(MPI_COMM_WORLD);
	endTime = MPI_Wtime();
	if (id == 0) {

		
		
		printf("Global : The number prime pair in 2 and %d is %d\n",n, globalPrimePairNum);
		printf("total run time is : %2.10f\n", endTime - startTime);

	}
	MPI_Finalize();
	return 0;
}

bool isPrime(int n) {
	int nSqrt;

	nSqrt = (int)sqrt(n);
	for (int i = 2; i <= nSqrt; i++) {
		if ((n%i) == 0) {
			return false;
		}
	}
	return true;
}

void process(int rank, int size, int max, int *startNum, int *endNum) {

	*startNum = ((rank*max) / size) + 1;
	*endNum =	((rank+1)*max) / size	;

	if ((*startNum) % 2 == 0) {
		*startNum++;
	}
}
