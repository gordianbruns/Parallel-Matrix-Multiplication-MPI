#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <getopt.h>
#include <mpi.h>

int	my_rank;
int	comm_sz;
MPI_Comm comm;
  

void Get_arg(int argc, char* argv[], int* debug, int* n_p, int* r_p, char* p_f);
long rdtsc(void);

int main (int argc, char *argv[]) {
    int N = 100, repeats = 1, counter, i, j, k, sum, rows, remainder, debug = 0, original;
    unsigned long totalBytes; 
    struct timeval startTime, stopTime;
    double wallTimeForAll, wallTimeForOne, MatrixKBPerSecond; 
    char platform[32] = "unknown";
    int offset = 0;

    long startCount = rdtsc();
    sleep(1);
    long endCount = rdtsc();
    long cycles = endCount - startCount;
    float coreSpeed = cycles / 1000000000.0;
  
    comm = MPI_COMM_WORLD;
    
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(comm, &my_rank);
    MPI_Comm_size(comm, &comm_sz);

    Get_arg(argc, argv, &debug, &N, &repeats, platform);
  
    if (my_rank == 0)
	{
		
		int (*mOne)[N] = malloc(sizeof(int[N][N]));
    	int (*mTwo)[N] = malloc(sizeof(int[N][N]));
    	int (*mResult)[N] = malloc(sizeof(int[N][N]));
    
  	    totalBytes = N * N * sizeof(int);
        int n_workers = comm_sz - 1;

        if ((mOne == NULL) || (mTwo == NULL) || (mResult == NULL))
		{
            perror("initial malloc() of mOne, mTwo, and/or mResult failed");
            exit(-1);
		}
        
        if (debug)
		{
            fprintf(stderr, "# itemsPerDimension: %d, repeats: %d, platform: %s, coreSpeed: %.3f", N, repeats, platform, coreSpeed);
            return 1;
        }
        if (N < 1 || repeats < 1)
		{
            printf("Use -d for debugging\n");
            fprintf(stderr, "usage: ./mpi_mmm -i <int > 0> -r <int > 0> -p <String>");
            exit(-1);
        }

        printf("# itemsPerDimension: %d, repeats: %d, platform: %s, coreSpeed: %.3f, np = %d\n", N, repeats, platform, coreSpeed, comm_sz);
  
        // initialize the three matrices 
        for (i = 0; i < N; i++)
		{
            for(j = 0; j < N; j++)
			{
                mOne[i][j] = 3333;         
                mTwo[i][j] = 7777; 
                mResult[i][j] = 0;
			}
		}

        if (gettimeofday(&startTime, NULL) != 0)
		{ 
	        perror("gettimeofday() startTime failed");
	        exit(-1);
		}
  
    
        rows = N / n_workers;
        remainder = N % n_workers;

	    offset = 0;
	
        for (i = 1; i <= n_workers; i++)
		{
		    if (remainder > 0)
			{
			    original = rows;
			    ++rows;
			    remainder--;	
			    MPI_Send(&offset, 1, MPI_INT, i, 1, comm);
			    MPI_Send(&rows, 1, MPI_INT, i, 1, comm);
			    MPI_Send(&(mOne[offset][0]), rows * N, MPI_INT, i, 1, comm);
			    MPI_Send(&(mTwo[0][0]), N * N, MPI_INT, i, 1, comm);
			    offset += rows;
			    rows = original;
		    } else {
			    MPI_Send(&offset, 1, MPI_INT, i, 1, comm);
			    MPI_Send(&rows, 1, MPI_INT, i, 1, comm);
			    MPI_Send(&(mOne[offset][0]), rows * N, MPI_INT, i, 1, comm);
			    MPI_Send(&(mTwo[0][0]), N * N, MPI_INT, i, 1, comm);
			    offset += rows;
		    }
        }
		
        for (i = 1; i <= n_workers; i++)
		{
		    MPI_Recv(&offset, 1, MPI_INT, i, 2, comm, &status);
		    MPI_Recv(&rows, 1, MPI_INT, i, 2, comm, &status);
		    MPI_Recv(&(mResult[offset][0]), rows * N, MPI_INT, i, 2, comm, &status);
		}
		
		if (gettimeofday(&stopTime, NULL) != 0)
		{
            perror("gettimeofday() stopTime failed");
            exit(-1);
		}
	
	    wallTimeForAll = (double)(stopTime.tv_sec - startTime.tv_sec) + (double)((stopTime.tv_usec - startTime.tv_usec) * (double)0.000001); 

        // factor out repeats, generate rate in MatrixItemsPerSecond
        wallTimeForOne = wallTimeForAll / (double)repeats; 
        MatrixKBPerSecond = ((double)totalBytes / (double)1024.0) / wallTimeForOne; 

        // display a portion of mResult, for testing and debugging, enabled via gcc -DDISPLAY ...
        #ifdef DISPLAY 
        for (i = 0; i < 2; i++)
		{
            for(j = 0; j < 2; j++)
			{
                fprintf(stderr, "mResult[%d][%d] = %d\n", i, j, mResult[i][j]);
			}
		}
        #endif
        
        printf("# platform, totalBytes, wallTimeForAll, wallTimeForOne, MatrixKBPerSecond\n"); 
        printf("%s, %lu, %.3lf, %.3lf, %.3lf\n", platform, totalBytes, wallTimeForAll, wallTimeForOne, MatrixKBPerSecond); 

        free(mOne); free(mTwo); free(mResult);
  
    } else {
    	
    	int (*mOne)[N] = malloc(sizeof(int[N][N]));
    	int (*mTwo)[N] = malloc(sizeof(int[N][N]));
    	int (*mResult)[N] = malloc(sizeof(int[N][N]));
    	
    	if ((mOne == NULL) || (mTwo == NULL) || (mResult == NULL))
		{
            perror("initial malloc() of mOne, mTwo, and/or mResult failed");
            exit(-1);
		}
    	
	    MPI_Recv(&offset, 1, MPI_INT, 0, 1, comm, &status);
	    MPI_Recv(&rows, 1, MPI_INT, 0, 1, comm, &status);
	    MPI_Recv(&(mOne[offset][0]), rows * N, MPI_INT, 0, 1, comm, &status);
	    MPI_Recv(&(mTwo[0][0]), N * N, MPI_INT, 0, 1, comm, &status);
	
		for (counter = 0; counter < repeats; counter++) {
			sum = 0;
	        for (i = 0; i < N; i++)
			{
	            for (j = 0; j < rows; j++)
				{
	      	        sum = 0;
	                for (k = 0; k < N; k++)
	                    sum = sum + mOne[j][k] * mTwo[k][i];
	                mResult[j][i] = sum;
				}
			} 
		}
	
	    MPI_Send(&offset, 1, MPI_INT, 0, 2, comm);
	    MPI_Send(&rows, 1, MPI_INT, 0, 2, comm);
	    MPI_Send(&(mResult[offset][0]), rows * N, MPI_INT, 0, 2, comm);
    }

    MPI_Finalize(); 
    return 0;
}

void Get_arg(int argc, char* argv[], int* debug, int* i_p, int* r_p, char* p_f) {
    int opt = 0;
    *r_p = 10;
    char err_msg[128] = "usage: ./mpi_mmm -i <int > 0> -r <int > 0> -p <String>\n";

    if (my_rank == 0)
	{
	    if (argc < 2)
		{
	        *i_p = 1000;
	        *r_p = 10;
		}
		if (comm_sz < 2)
		{
	    	MPI_Finalize();
	    	fprintf(stderr, err_msg);
	    	exit(-1);
		} else {
	    	while((opt = getopt(argc, argv, "d:i:r:p:")) != -1) {
		    	switch(opt) {
		        	case 'd':
			        	*debug = 1;
			        	break;
		        	case 'i':
			        	*i_p = atoi(optarg);
			        	break;
		        	case 'r':
			        	*r_p = atoi(optarg);
			        	break;
		        	case 'p':
			        	strcpy(p_f, optarg);
			        	break;
		        }
	        }
        }
    }
    MPI_Bcast(i_p, 1, MPI_INT, 0, comm);
    MPI_Bcast(r_p, 1, MPI_INT, 0, comm); 
}

long rdtsc(void)
{
    long ret0[2];
    __asm__ __volatile__("rdtsc" : "=a"(ret0[0]), "=d"(ret0[1]));
    return ((long)ret0[1] << 32) | ret0[0];
}
