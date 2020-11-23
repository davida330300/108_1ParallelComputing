#include "mpi.h"
#include "iostream"
#include "stdlib.h"
#include "math.h"
#define upperbound 1000000
using namespace std;
bool isPrime(int n);
void Process(int rank, int size, int max, int *startNum, int *endNum);
int main(int argc, char** argv) {

	int p, id;
	int n = upperbound;
	int startNum, endNum;

	int lastPrime=0;
	int difference = 1;

	int localResult=0, globalResult=0;

	double startTime, endTime;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD,&p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	startTime = MPI_Wtime();
	Process(id, p, n, &startNum, &endNum);

	for (int i = startNum; i <= endNum; i=i+2) {
		
		if (isPrime(i)) {
			if (!isPrime(lastPrime)||(lastPrime<startNum)) {
				lastPrime = i;
			}
			else {
				if ((i - lastPrime) > difference) {
					difference = (i - lastPrime);
				}
				lastPrime = i;
			}
			
		}
	}
	localResult = difference;

	MPI_Reduce(&localResult, &globalResult, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
	endTime = MPI_Wtime();
	MPI_Finalize();

	
	printf("rank %d processor:\nThe largest prime gap from %d to %d (inclusive) is:  %d\n",id, startNum, endNum, localResult);
	if (id == 0) {
		printf("Global:\nThe largest prime gap in the set 2 and %d (inclusive) is:  %d\n",n, globalResult);
		printf("total runtime %2.10f\n",endTime-startTime);
	}

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
void Process(int rank, int size, int max, int *startNum, int *endNum) {
	*startNum = ((rank*max) / size) + 1;
	*endNum = (((rank + 1)*max)) / size;

	if (*startNum % 2 == 0) {
		*startNum++;
	}
}


