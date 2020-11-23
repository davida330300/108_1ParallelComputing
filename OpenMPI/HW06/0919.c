#include <stdio.h>
#include "mpi.h"

#define MPI_TYPE MPI_LONG
#define REQUEST_MESSAGE 0
#define TASK_MESSAGE 1
typedef unsigned long long ntype;

int isPerfect(ntype);
void manager(int);
void worker();
int isPrime(ntype);
ntype int_pow(ntype, ntype );

int main(int argc, char *argv[])
{
    int wrk = 0;
    int wsize = 0;

    /* Initializing MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &wrk);
    MPI_Comm_size(MPI_COMM_WORLD, &wsize);

    if(wsize < 2)
    {
        printf("Program needs at least 2 processes.\n");
        return 1;
    }
    else
    {
        if(!wrk)
        {
            manager(wsize);
        }
        else
        {
            worker();
        }
        
    }
    MPI_Finalize();
    return 0;
}

int isPerfect(ntype number)
{
    ntype rem = 0;
    ntype sum = 0;
    ntype i = 0;
 
    for (i = 1; i*i < number; i++)
    {
        rem = number % i;
	    if (rem == 0)
        {
            sum += i;
            sum += number/i;
        }
    }
    if (i*i == number)
    {
        sum += i;
    }
    return (sum == number*2);
}

int isPrime(ntype num)
{
    ntype i = 0;
    for (i = 2; i*i <= num; i += 1)
    {
        if(num % i == 0) return 0;
    }
    return 1;
}

ntype int_pow(ntype base, ntype p)
{
    ntype ans = 1;
    ntype i = 0;
    for(; i < p; i += 1)
    {
        ans *= base;
    }
    return ans;
}

void manager(int wsize)
{
    const int MAX = 6;
    const ntype TERMINATE = 0;
    int termiate_pro = 0;
    ntype progress = 2;
    int per_count = 0;
    ntype pnum = 0;
    ntype per_num[6] = {};
    MPI_Status status;
    int src = 0;
    int i = 0;
    double ela_time = 0;

    ela_time = - MPI_Wtime();

    do
    {
        MPI_Recv(&pnum, 1, MPI_TYPE, MPI_ANY_SOURCE, REQUEST_MESSAGE, MPI_COMM_WORLD, &status);
        src = status.MPI_SOURCE;
        if (pnum)
        {
            per_num[per_count] = pnum;
            per_count += 1;
        }
        if (per_count < MAX)
        {
            MPI_Send(&progress, 1, MPI_TYPE, src, TASK_MESSAGE, MPI_COMM_WORLD);
            progress += 1;
        }
        else
        {
            MPI_Send(&TERMINATE, 1, MPI_TYPE, src, TASK_MESSAGE, MPI_COMM_WORLD);
            termiate_pro += 1;
        }
    } while(termiate_pro < (wsize - 1));
    ela_time += MPI_Wtime();

    printf("perfect numbers : %llu", per_num[0]);
    for(i = 1; i < MAX; i += 1)
    {
        printf(", %llu", per_num[i]);
    }
    printf("\n");

    printf("There are %d processes.\n", wsize);
    printf("Elapsed %.6f seconds.\n", ela_time);
}

void worker()
{
    ntype test_num = 0;
    ntype p = 0;
    ntype job = 0;
    do
    {
        MPI_Send(&test_num, 1, MPI_TYPE, 0, REQUEST_MESSAGE, MPI_COMM_WORLD);
        MPI_Recv(&p, 1, MPI_TYPE, 0, TASK_MESSAGE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if(!p) break;
        test_num = int_pow(2, p) - 1;
        if(isPrime(test_num)) 
        {
            test_num *= int_pow(2, p-1);
        }
        else 
        {
            test_num = 0;
        }
    } while(1);
    
}