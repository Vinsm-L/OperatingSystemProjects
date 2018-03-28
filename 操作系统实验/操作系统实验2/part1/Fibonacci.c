#include <pthread.h>
#include <stdio.h>

int fib[50]; // store the Fibonacci array

void *generateFib(void *number); //  function to generate Fibonacci array

int main(int argc, char *argv[])
{
	pthread_t tid; // the thread identifier
	pthread_attr_t attr; 
	
	int num;  //  the edge of Fibonacci array
	scanf("%d", &num);
	
	pthread_attr_init(&attr); // get the default attributes 

	pthread_create(&tid, &attr, generateFib, (void *)&num); // create pthreads 

	pthread_join(tid, NULL); // now join on each thread 

	int i; // output
	for (i = 0; i < num; i++) {
		printf("%d\n", fib[i]);
	}
}

void *generateFib(void *number)
{
	int *edge = (int*)number;
	if (*edge <= 0) {
		pthread_exit(0);
	} else if (*edge == 1) {
		fib[0] = 0;
	} else if (*edge == 2) {
		fib[0] = 0;
		fib[1] = 1;
	} else {
		fib[0] = 0;
		fib[1] = 1;
		int i;
		for (i = 2; i < *edge; i++) {   // when edge > 2, generate the following numbers
			fib[i] = fib[i-1] + fib[i-2];
		}
	}
	pthread_exit(0);
}