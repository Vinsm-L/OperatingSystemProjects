#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>
#include <wait.h>
#include <errno.h>

int read_count = 0;       //  to record the number of reader in critical section
int sharedResource = 0;   //  the shared memory for reading or writing

/*
** the parameter passed to thread, indicating each thread's ID
*/
struct threadID {
	int index;            //  the number of thread
	int start_time;       //  the start time of read/write operation
	int duration;         //  the duration of read/write operation
};

/*
** POSIX signal
** mutex: to guarantee change the read_count correctly
** write_sig: to avoid read-write and write-write
*/
sem_t mutex, write_sig;

void * reader(void * param) {
	struct threadID * data = (struct threadID *)param;
	printf("NO.%d reader thread is created\n", data->index);
	sleep(data->start_time);

	printf("NO.%d reader thread applies to read\n", data->index);
	sem_wait(&mutex);  //  apply to read

	read_count++;
	if (read_count == 1) {  //  the first reader enters critical section, and avoids other writers to enter
		sem_wait(&write_sig);
	}
	
	printf("NO.%d reader thread begins to read\n", data->index);
	sem_post(&mutex);  //  when it begins to read, release the resource so that other readers can read
	sleep(data->duration);

	sem_wait(&mutex);  //  when changing read_count, we need to ensure exclusion
	printf("NO.%d reader thread finishes reading\n", data->index);
	
	read_count--;

	if (read_count == 0) {
		sem_post(&write_sig);  //  the last reader leaves critical section, and enables other writers to enter
	}
	sem_post(&mutex);

	pthread_exit(0);
}

void * writer(void * param) {
	struct threadID * data = (struct threadID *)param;
	printf("NO.%d writer thread is created\n", data->index);
	sleep(data->start_time);

	printf("NO.%d writer thread applies to write\n", data->index);
	sem_wait(&write_sig);  //  apply to write

	printf("NO.%d writer thread begins to write\n", data->index);
	sleep(data->duration);
	sharedResource = rand();
	printf("NO.%d writer thread finishes writing %d into shared memory\n", data->index, sharedResource);

	sem_post(&write_sig);  //  now other reader or writer can enter
	pthread_exit(0);
}

int main() {

	//  create the signal
	sem_init(&mutex, 0, 1);
	sem_init(&write_sig, 0, 1);

	//  create the threads and parameters
	pthread_t tid[5];
	struct threadID * ID[5];

	// read the file
	FILE * ptr = fopen("test.txt", "r");
	fseek(ptr, 0, SEEK_END);
	int length = ftell(ptr);
	rewind(ptr);
	char * fileContent = (char *)malloc(sizeof(char)*(length + 1));
	length = fread(fileContent, sizeof(char), length, ptr);
	fileContent[length] = '\0';
	fclose(ptr);

	// deal with the data in file
    int i;
	for (i = 0; i < 5; i++) {
		ID[i] = (struct threadID *)malloc(sizeof(struct threadID));
		ID[i]->index = fileContent[i * 8 + 0] - '0';
		ID[i]->start_time = fileContent[i * 8 + 4] - '0';
		ID[i]->duration = fileContent[i * 8 + 6] - '0';

		if (fileContent[i * 8 + 2] == 'R') {
			pthread_create(&tid[i], NULL, reader, (void *)ID[i]);
		}
		else {
			pthread_create(&tid[i], NULL, writer, (void *)ID[i]);
		}
	}

	// join the threads
	for (i = 0; i < 5; i++) {
		pthread_join(tid[i], NULL);
	}

	for (i = 0; i < 5; i++) {
		free(ID[i]);
	}
	free(fileContent);

	return 0;
}
