#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <syscall.h>
#include "matmul.h"

int linear_mult(int *col_a, int *row_b, int n)
{
		int i, j, r = 0;
		
		for(i = 0; i < n; i++) {
				r += col_a[i] * row_b[i];
		}

		return r;
}

void read_matrix(char *fname)
{
		FILE *fp;
		int i, j, k;

		fp = fopen(fname, "r");
		if (fp == NULL) {
				printf("File open fail: %s\n", fname);
				exit(0);
		}
		
		/** Read matrix size **/
		fscanf(fp, "%d %d %d", &m, &n, &p);
		printf("================================================\n");
		printf("matrix size: A- %d * %d, B- %d * %d\n", m, n, n, p);
	
		/** Allocate matrix arrays **/
		alloc_matrix();

		/** Read matrix A from the file **/
		for(i = 0; i < m; i++) {
				for(j = 0; j < n; j++) {
						fscanf(fp, "%d", &A[i][j]);
				}
		}

		if(m < 10 && n < 10) {
				printf("Matrix A: \n");
				print_matrix(A, m, n);
		}


		/** Read matrix B from the file **/
		for(i = 0; i < n; i++) {
				for(j = 0; j < p; j++) {
						fscanf(fp, "%d", &B[i][j]);
				}
		}

		if(n < 10 && p < 10) {
				printf("\nMatrix B: \n");
				print_matrix(B, n, p);
		}

		matrix_tranpose();

		if(n < 10 && p < 10) {
				printf("\nTranposed matrix B:\n");
				print_matrix(B_tran, p, n);
		}
		printf("================================================\n\n");

		/** Initialize C **/
		for(i = 0; i < m; i++) {
				for(j = 0; j < p; j++) {
						C_single[i][j] = 0;
						C_multi_proc[i][j] = 0;
						C_multi_thread[i][j] = 0;
				}
		}
		
		fclose(fp);
		return;
}

void matrix_tranpose()
{
		int i, j;

		for(i = 0; i < n; i++)
				for(j = 0; j < p; j++)
						B_tran[j][i] = B[i][j];
}

void alloc_matrix()
{
		int i, j, k;

	/** Memory Allocation **/
		A = (int**)malloc(sizeof(int*) * m);
		B = (int**)malloc(sizeof(int*) * n);
		B_tran = (int**)malloc(sizeof(int*) * p);
		C_single = (int**)malloc(sizeof(int*) * m);
		C_multi_proc = (int**)malloc(sizeof(int*) * m);
		C_multi_thread = (int**)malloc(sizeof(int*) * m);
		

		for(i = 0; i < m; i++)
				A[i] = (int*)malloc(sizeof(int) * n);

		for(i = 0; i < n; i++)
				B[i] = (int*)malloc(sizeof(int) * p);

		for(i = 0; i < p; i++)
				B_tran[i] = (int*)malloc(sizeof(int) * n);

		for(i = 0; i < m; i++) {
				C_single[i] = (int*)malloc(sizeof(int) * p);
				C_multi_proc[i] = (int*)malloc(sizeof(int) * p);
				C_multi_thread[i] = (int*)malloc(sizeof(int) * p);
		}
}

void print_matrix(int **mat, int l, int m)
{
		int i, j;
		
		for(i = 0; i < l; i++) {
				for(j = 0; j < m; j++) {
						printf("%d ", mat[i][j]);
				}
				printf("\n");
		}
		return;
}

int compare_matrices(int **mat1, int **mat2, int m, int n)
{
		int i, j;
		for(i = 0; i < m; i++)
		{
				for(j = 0; j < n; j++) 
				{
						if(mat1[i][j] != mat2[i][j])
						{
								return -1;
						}
				}
		}

		return 1;
}

void simulate_process_crash(int i)
{
		srand(getpid());
		if(crashRate > 0 && (rand()%100 < crashRate)) 
		{
				printf("*** Process[%d] (pid %d) crashed.\n", i, getpid());
				abort();
		}
}

void simulate_thread_crash(int i)
{
		srand(syscall(186));
		if(crashRate > 0 && (rand()%100 < crashRate)) 
		{
				printf("*** Thread[%d] (tid %d) crashed.\n", i, syscall(186));
				pthread_exit(NULL);
		}
}
