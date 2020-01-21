#include "mpi.h"
#include "stdlib.h"
#include "iostream"
#include "math.h"
#include <time.h> 
#include <stdio.h>
using namespace std;
void Process(int rank, int size, int max, int *startNum, int *endNum);
double Distace_With_Line(double x, double y, double z);

int main(int argc,char** argv) {
	srand(time(NULL));
	int localResult = 0, globalResult = 0;
	int itr = 50000;
	int p, id;
	int startNum, endNum;
	double startTime, endTime;
	double ratio;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	MPI_Comm_rank(MPI_COMM_WORLD, &id);

	startTime = MPI_Wtime();

	Process(id, p, itr, &startNum, &endNum);

	for (int i = startNum; i < endNum; i++) {
		double x = 
			static_cast <double> (rand()) 
				/ static_cast <double> (RAND_MAX / 2.0);
		double y = 
			static_cast <double> (rand()) 
				/ static_cast <double> (RAND_MAX / 2.0);
		double z = 
			static_cast <double> (rand()) 
				/ static_cast <double> (RAND_MAX / 2.0);

		double distance = Distace_With_Line(x,y,z);

		if (distance >= 0.15) {
			localResult++;
		}
	}

	MPI_Reduce(&localResult, &globalResult, 1, 
				MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	MPI_Barrier(MPI_COMM_WORLD);
	endTime = MPI_Wtime();

	if (id == 0) {
		ratio = double(globalResult) / double(itr);
		ratio = ratio * 8;
		printf("the volumn is: %.5f," 
				"compare to answer 7.77180\n", double(ratio));
		printf("the total run time is:¡@%.8f\n", endTime - startTime);
	}
	MPI_Finalize();
	system("pause");
	return 0;
}
double Distace_With_Line(double x, double y, double z) {
	return	sin(acos((x + y + z) 
		/ (sqrt(3)*sqrt(x*x + y * y + z * z))))*sqrt(x*x + y * y + z * z);	
}
void Process(int rank, int size, int max, int *startNum, int *endNum) {
	*startNum = ((rank*max) / size) ;
	*endNum = ((((rank + 1)*max)) / size) - 1;
}
