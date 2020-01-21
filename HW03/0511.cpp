#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
using namespace std;
void addForBig(int* ad, int* to);
void dividebyn(int n, int* ans);

#define num 1000000
#define digit 100



int main(int argc, char**argv) {

	int p, id;
	int tmp;
	int numOfN, numOfDigit;
	MPI_Status status;

	int localAns[digit+1];
	int globalAns[digit+1];
	int *buf;

	double starttime, endtime;

	for (int i = 0; i < digit+1; i++) {
		globalAns[i] = 0;
	}

	tmp = 1;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&id);
	MPI_Comm_size(MPI_COMM_WORLD,&p);

	MPI_Barrier(MPI_COMM_WORLD);
	starttime = MPI_Wtime();
	
	for (int i = (num*id / p) + 1; i < (num*(id + 1) / p) + 1; i++) {
		for (int j = 0; j < digit + 1; j++) {
			localAns[j] = 0;
		}
		dividebyn(i, localAns);
		addForBig(localAns, globalAns);
	}

	if (id==0) {

		for (int i = 1; i < p; i++) {
			buf = (int*)malloc(sizeof(int)*(digit + 1));
			MPI_Recv(buf, digit + 1, MPI_INT, i, 1, MPI_COMM_WORLD, &status);
			addForBig(buf, globalAns);
		}

		globalAns[0] += 1;
	}
	else {
		MPI_Send(globalAns, digit + 1, MPI_FLOAT, 0, 1, MPI_COMM_WORLD);
	}
	MPI_Barrier(MPI_COMM_WORLD);
	endtime = MPI_Wtime();
	MPI_Finalize();
	if (id==0) {
		printf("ans:\n");
		printf("%d.", globalAns[0]);
		for (int i = 1; i < digit + 1; i++) {
			printf("%d", globalAns[i]);
		}
		printf("\ntimes : %8.6f\n", endtime-starttime);
	}
	fflush(stdout);
	return 0;
}

void addForBig(int* add, int* global) {
	int carry = 0;
	for (int i = digit; i > 0; --i) {
		global[i] = global[i] + add[i] + carry;
		carry = global[i] / 10;
		global[i] = global[i] % 10;
	}
	global[0] = global[0] + add[0] + carry;
}
void dividebyn(int a, int* ans) {
	int tmp = 1;
	ans[0] = 0;
	for (int i = 1; i < digit + 1; i++) {
		tmp = tmp % a;
		if (tmp == 0)
			break;

		tmp = tmp * 10;
		ans[i] = tmp / a;
	}
}