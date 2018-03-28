#include <pthread.h>
#include <stdio.h>

int result[3][2];  //  store the result
int A[3][2];  //  source matrix A
int B[2][2];  //  source matrix B
struct index  //  parameter of matrixMulti
{
	int i, j;
};

void *matrixMulti(void *number);   //  function to calculate each element in result matrix

int main(int argc, char *argv[])
{
	pthread_t tid[6]; // the thread identifier
	pthread_attr_t attr; 
	int i, j;

	for (i = 0; i < 3; i++) {  //  initial matrix A
		for (j = 0; j < 2; j++) {
			A[i][j] = i + j;
		}
	}

	for (i = 0; i < 2; i++) {  //  initial matrix B
		for (j = 0; j < 2; j++) {
			B[i][j] = i + j;
		}
	}
	
	pthread_attr_init(&attr); // get the default attributes 
	
	int k = 0;
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2; j++) {
			struct index ind;
			ind.i = i;
			ind.j = j;
			pthread_create(&tid[k], &attr, matrixMulti, (void *)&ind); // create pthreads 
			pthread_join(tid[k], NULL); // now join on each thread
			k++;
		}
	}

	// output
	for (i = 0; i < 3; i++) {
		for (j = 0; j < 2; j++) {
			printf("%d ", result[i][j]);
		}
		printf("\n");
	}
}

void *matrixMulti(void *number)
{
	struct index *ind = (struct index*)number;
	int n;
	int res = 0;
	int i = (*ind).i;
	int j = (*ind).j;
	for (n = 0; n < 2; n++) {      //  calculate the [i, j] element of result matrix
		res += A[i][n] * B[n][j];
	}
	result[i][j] = res;

	pthread_exit(0);
}