#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>

#define BUFFER_SIZE 5      //  limited buffer size
typedef int buffer_item;
buffer_item buffer[BUFFER_SIZE];

int currentPos = 0;             //  indicate which position to insert
int item_num = 0;          //  record the number of items in buffer
int start_time;            //  the start time of produce/consume operation
int duration;              //  the duration of produce/consume operation

sem_t empty, full, mutex;  //  POSIX signal, to synchronize or exclude the threads

/*
** used by producer
** insert data into buffer
** return 0 if succeed. Otherwise return -1
*/
int insert_item(buffer_item item) {
	if (item_num > BUFFER_SIZE) {
		return -1;
	}
	sem_wait(&empty);  //  apply for resources
	sem_wait(&mutex);
	buffer[currentPos] = item;
	item_num++;
	currentPos = (currentPos + 1) % BUFFER_SIZE;
	sem_post(&full);  //  release resources
	sem_post(&mutex);
	return 0;
}

/*
** used by consumer
** remove data from buffer
** return 0 if succeed. Otherwise return -1
*/
int remove_item(buffer_item *item) {
	if (item_num == 0) {
		return -1;
	}
	sem_wait(&full);  //  apply for resources
	sem_wait(&mutex);
	*item = buffer[(currentPos + BUFFER_SIZE - 1) % BUFFER_SIZE];
	item_num--;
	currentPos = (currentPos + BUFFER_SIZE - 1) % BUFFER_SIZE;
	sem_post(&empty);  //  release resources
	sem_post(&mutex);
	return 0;
}

/*
** generate a number randomly, and call insert_item to deal with it
*/
void *producer(void *Param) {
	buffer_item item;
	sleep(start_time);
	item = rand();
	printf("producer has produced %d\n", item);
	sleep(duration);
	if (insert_item(item)) {
		printf("buffer is full\n");
	} else {
		printf("insert data success!\n");
	}
	pthread_exit(0);
}

/*
** consume the item in buffer, and call remove_item to complete
*/
void *consumer(void *Param) {  
	buffer_item item;
	sleep(start_time);
	printf("consumer try to consume data\n");
	if (remove_item(&item)) {
		sleep(duration);
		printf("buffer is empty\n");
	} else {
		sleep(duration);
		printf("consumer consumed %d\n", item);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]) {

	//  create the signal
	sem_init(&full, 0, 0);
	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, 5);

	//  create the threads
	pthread_t tid[5];

	//  read the file
	FILE * ptr = fopen("test.txt", "r");
	fseek(ptr, 0, SEEK_END);
	int length = ftell(ptr);
	rewind(ptr);
	char * fileContent = (char*)malloc(sizeof(char)*(length + 1));
	length = fread(fileContent, sizeof(char), length, ptr);
	fileContent[length] = '\0';
	fclose(ptr);

	//  deal with the data in file
    int i;
	for (i = 0; i < 5; i++) {
		start_time = fileContent[i * 8 + 4] - '0';
		duration = fileContent[i * 8 + 6] - '0';
		
		if (fileContent[i * 8 + 2] == 'C') {
			pthread_create(&tid[i], NULL, consumer, NULL);
		}
		else {
			pthread_create(&tid[i], NULL, producer, NULL);
		}
		pthread_join(tid[i], NULL);
	}

	free(fileContent);
	
	return 0;
}
