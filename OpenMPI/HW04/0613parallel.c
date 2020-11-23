#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

#define DATA_MSG           0
#define PROMPT_MSG         1
#define RESPONSE_MSG       2

#define OPEN_FILE_ERROR    -1
#define MALLOC_ERROR       -2
#define TYPE_ERROR         -3

#define MIN(a,b)           ((a)<(b)?(a):(b))
#define BLOCK_LOW(id,p,n)  ((id)*(n)/(p))
#define BLOCK_HIGH(id,p,n) (BLOCK_LOW((id)+1,p,n)-1)
#define BLOCK_SIZE(id,p,n) (BLOCK_HIGH(id,p,n)-BLOCK_LOW(id,p,n)+1)
#define BLOCK_OWNER(j,p,n) (((p)*((j)+1)-1)/(n))
#define PTR_SIZE           (sizeof(void*))
#define CEILING(i,j)       (((i)+(j)-1)/(j))

void *my_malloc (int id,int bytes);
int get_size (MPI_Datatype t);
void read_row_striped_matrix (char *, void ***, void **, MPI_Datatype, int *, int *, MPI_Comm);
void print_row_striped_matrix (void **, MPI_Datatype, int, int, MPI_Comm);
void update(int id, int p, int **a,int m,int n,int iteration,int printperiod);


#define MPI_TYPE MPI_INT

int main (int argc, char *argv[]) {
    int** a;         
    int*  storage;  
    int     id,p;    
    int     m,n;    
    int     iteration = atoi(argv[2]);
    int     printperiod = atoi(argv[3]);
    double  time, max_time;

    MPI_Init (&argc, &argv);
    MPI_Comm_rank (MPI_COMM_WORLD, &id);
    MPI_Comm_size (MPI_COMM_WORLD, &p);
    printf("process %d of size %d\n",id,p);
    printf("read %s iteration %d print every %d iteration\n",argv[1],iteration,printperiod); 
    read_row_striped_matrix (argv[1], (void *) &a,(void *) &storage, MPI_TYPE, &m, &n, MPI_COMM_WORLD);

    MPI_Barrier (MPI_COMM_WORLD);
    time = -MPI_Wtime();
    update (id, p, (int **) a,m,n,iteration,printperiod);
    time += MPI_Wtime();
    //MPI_Reduce (&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0,MPI_COMM_WORLD);
    if (!id) printf ("Matrix size %d, %d processes: %6.2f seconds\n",n, p, time);
    MPI_Finalize();
}

void update(int id, int p, int **a,int m,int n,int iteration,int printperiod){
    int local_rows = BLOCK_SIZE(id,p,m); // a process own how many row
    printf("id %d local_row %d\n",id,local_rows);
    int *toabove = (int *)my_malloc(id,n * sizeof(int));;
    int *tobelow = (int *)my_malloc(id,n * sizeof(int));;
    int *upper = (int *)my_malloc(id,n * sizeof(int));
    int *lower = (int *)my_malloc(id,n * sizeof(int));
    int **b = (int**) my_malloc (id,local_rows * sizeof(int*));
    for(int i = 0;i < local_rows;++i)
        b[i] = (int*) my_malloc (id,n * sizeof(int));

    int which = 0;
    int** board[] = {a,b};
    memset(upper,0,n*sizeof(int)); // set recv data to 0
    memset(lower,0,n*sizeof(int));


    for(int it = 0;it < iteration;++it,which ^= 1){
        if(it % printperiod == 0){
            //print_row_striped_matrix ((void **) board[which], MPI_TYPE, m, n,MPI_COMM_WORLD);
            if(!id)
                printf("============================\n");
        }
        if(p != 1){
            for (int j = 0; j < n; j++){
                toabove[j] = board[which][0][j]; // copy first row 
                tobelow[j] = board[which][local_rows - 1][j]; // copy last row
            }
            memset(upper,0,n*sizeof(int)); // set recv data to 0
            memset(lower,0,n*sizeof(int));
            /* transmit data */
            if(id == 0){ // the first process, since there is nothing above.
                MPI_Send(tobelow,n,MPI_INT,id + 1,0,MPI_COMM_WORLD);
                MPI_Recv(lower,n,MPI_INT,id + 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
            else if(id == p - 1){  // the last process, since there is nothing below.
                MPI_Send(toabove,n,MPI_INT,id - 1,0,MPI_COMM_WORLD);
                MPI_Recv(upper,n,MPI_INT,id - 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
            else{
                MPI_Send(tobelow,n,MPI_INT,id + 1,0,MPI_COMM_WORLD);
                MPI_Recv(upper,n,MPI_INT,id - 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                MPI_Send(toabove,n,MPI_INT,id - 1,0,MPI_COMM_WORLD);
                MPI_Recv(lower,n,MPI_INT,id + 1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        }
        for (int i = 0; i < local_rows; i++){ // each procss update its owned row
            for(int j = 0;j < n;++j){
                int count = 0; // count how many lived neighber.
                count += ((i == 0) ? upper[j] : board[which][i - 1][j]);
                count += ((i == local_rows - 1) ? lower[j] : board[which][i + 1][j]);
                count += ((j == 0) ? 0 : board[which][i][j - 1]);
                count += ((j == n - 1) ? 0 : board[which][i][j + 1]);

                if(board[which][i][j]){ // live
                    if(count != 2 && count != 3)
                        board[which ^ 1][i][j] = 0;
                    else
                        board[which ^ 1][i][j] = 1;
                }
                else{ // dead
                    if(count == 3)
                        board[which ^ 1][i][j] = 1;
                    else
                        board[which ^ 1][i][j] = 0;
                }
                
            }
        }
    }

    free(upper);
    free(lower);
    free(toabove);
    free(tobelow);
    for(int i = 0;i < local_rows;++i)
        free(b[i]);
    free(b);
    return;
}


void read_row_striped_matrix (char *s,void ***subs,void **storage,MPI_Datatype dtype,int *m,int *n,MPI_Comm comm){
    int          datum_size; 
    int          i;
    int          id;       
    FILE        *infileptr;
    int          local_rows;
    void       **lptr;     
    int          p;       
    void        *rptr;   
    MPI_Status   status;
    int          x;    

    MPI_Comm_size (comm, &p);
    MPI_Comm_rank (comm, &id);
    datum_size = get_size (dtype);

    if (id == (p-1)) {
        infileptr = fopen (s, "r");
        if (infileptr == NULL) *m = 0;
        else {
            fread (m, sizeof(int), 1, infileptr);
            fread (n, sizeof(int), 1, infileptr);
            printf("%d X %d board\n",*m,*n);
        }      
    }
    MPI_Bcast (m, 1, MPI_INT, p-1, comm);

    if (!(*m)) MPI_Abort (MPI_COMM_WORLD, OPEN_FILE_ERROR);

    MPI_Bcast (n, 1, MPI_INT, p-1, comm);

    local_rows = BLOCK_SIZE(id,p,*m);

    *storage = (void *) my_malloc (id,local_rows * *n * datum_size);
    *subs = (void **) my_malloc (id, local_rows * PTR_SIZE);

    lptr = (void *) &(*subs[0]);
    rptr = (void *) *storage;
    for (i = 0; i < local_rows; i++) {
        *(lptr++)= (void *) rptr;
        rptr += *n * datum_size;
    }
    if (id == (p-1)) {
        for (i = 0; i < p-1; i++) {
            x = fread (*storage, datum_size,BLOCK_SIZE(i,p,*m) * *n, infileptr);
            MPI_Send (*storage, BLOCK_SIZE(i,p,*m) * *n, dtype,i, DATA_MSG, comm);
        }
        x = fread (*storage, datum_size, local_rows * *n,infileptr);
        fclose (infileptr);
    } else
        MPI_Recv (*storage, local_rows * *n, dtype, p-1,DATA_MSG, comm, &status);
}

void print_submatrix (void **a,MPI_Datatype dtype,int rows,int cols){
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (dtype == MPI_DOUBLE)
                printf ("%6.3f ", ((double **)a)[i][j]);
            else {
                if (dtype == MPI_FLOAT)
                    printf ("%6.3f ", ((float **)a)[i][j]);
                else if (dtype == MPI_INT)
                    printf ("%6d ", ((int **)a)[i][j]);
            }
        }
        putchar ('\n');
    }
}

void print_row_striped_matrix (void **a,MPI_Datatype dtype,int m,int n,MPI_Comm comm){
    MPI_Status  status;          
    void       *bstorage;       
    void      **b;              
    int         datum_size;    
    int         i;
    int         id;           
    int         local_rows;   
    int         max_block_size; 
    int         prompt;        
    int         p;            

    MPI_Comm_rank (comm, &id);
    MPI_Comm_size (comm, &p);
    local_rows = BLOCK_SIZE(id,p,m);
    if (!id) {
        print_submatrix (a, dtype, local_rows, n);
        if (p > 1) {
            datum_size = get_size (dtype);
            max_block_size = BLOCK_SIZE(p-1,p,m);
            bstorage = my_malloc (id,max_block_size * n * datum_size);
            b = (void **) my_malloc (id,
                    max_block_size * datum_size);
            b[0] = bstorage;
            for (i = 1; i < max_block_size; i++)
                b[i] = b[i-1] + n * datum_size;
            for (i = 1; i < p; i++) {
                MPI_Send (&prompt, 1, MPI_INT, i, PROMPT_MSG,MPI_COMM_WORLD);
                MPI_Recv (bstorage, BLOCK_SIZE(i,p,m)*n, dtype,i, RESPONSE_MSG, MPI_COMM_WORLD, &status);
                print_submatrix (b, dtype, BLOCK_SIZE(i,p,m), n);
            }
            free(b);
            free(bstorage);
        }
        putchar ('\n');
    } else {
        MPI_Recv (&prompt, 1, MPI_INT, 0, PROMPT_MSG,MPI_COMM_WORLD, &status);
        MPI_Send (*a, local_rows * n, dtype, 0, RESPONSE_MSG,MPI_COMM_WORLD);
    }
}
int get_size (MPI_Datatype t) {
    if (t == MPI_BYTE) return sizeof(char);
    if (t == MPI_DOUBLE) return sizeof(double);
    if (t == MPI_FLOAT) return sizeof(float);
    if (t == MPI_INT) return sizeof(int);
    printf ("Error: Unrecognized argument to 'get_size'\n");
    fflush (stdout);
    MPI_Abort (MPI_COMM_WORLD, TYPE_ERROR);
}

void *my_malloc (int id,int bytes){
    void *buffer;
    if ((buffer = malloc ((size_t) bytes)) == NULL) {
        printf ("Error: Malloc failed for process %d\n", id);
        fflush (stdout);
        MPI_Abort (MPI_COMM_WORLD, MALLOC_ERROR);
    }
    return buffer;
}
