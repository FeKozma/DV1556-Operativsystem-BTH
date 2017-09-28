#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define bufSize 5 // Buffer max size.

int buf[bufSize]; // Keeps track of the items.
int nProducer = 2; // Default amount of producers.
int nConsumer = 2; // Default amount of consumers.
int count = 0; // Keep track of the current item count.

typedef sem_t semaphore;
semaphore mutex; // Lock threads.
semaphore empty; // Lock thread if full.
semaphore full; // Lock thread if empty.

void* producer(void *arg);
void* consumer(void *arg);

int main(int argc, char **argv)
{
	// Initialize random and semaphones...
	srand(time(NULL));

	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, bufSize);
	sem_init(&full, 0, 0);

	// If you entered an argument, set the new producer and consumer count.
	if (argc > 1)
	{
		nProducer = atoi(argv[1]);
		if (argc > 2)
		{
			nConsumer = atoi(argv[2]);
		}
	}

	// Allocate memory and initialize threads.
	pthread_t* children;
	children = malloc((nProducer + nConsumer) * sizeof(pthread_t));
	int consumerNr[nConsumer];
	int producerNr[nProducer];

	for (int i = 0; i < nProducer; i++)
	{
		producerNr[i] = i;

		pthread_create(
			&(children[i]),
			NULL,
			producer,
			&producerNr[i]);
	}

	for (int i = 0; i < nConsumer; i++)
	{
		consumerNr[i] = i;

		pthread_create(
			&(children[i]),
			NULL,
			consumer,
			&consumerNr[i]);
	}

	for (int i = 0; i < nProducer + nConsumer; i++)
	{
		pthread_join(children[i], NULL);
	}

	return 0;
}

// Will produce an item and return it.
int produce_item() {
	return (rand()%9000) + 1000;
}

// Will insert an item into the buffer.
void insert_item(int item) {
	buf[count] = item;
	count++;
}

// Producer thread, produces items.
void* producer(void *arg)
{
	int id = *((int*)arg);
	int item;

	while(1)
	{
		item = produce_item();

		// You don't want to have sem_wait(&mutex) before sem_wait(&empty) because
		// then if the buffer is empty, it will lock mutex (and therefore locking the producer threads).
		// It will then wait for an item, which will never happen because mutex is locked so
		// the producer thread is waiting for it to be unlocked.
		sem_wait(&empty);
		sem_wait(&mutex);

		insert_item(item);
		printf("Producer nr %d produced item %d\n", id, item);

		sem_post(&mutex);
		sem_post(&full);

		sleep((rand()%5)+1);
	}
}

// Will remove an item from the buffer, then return the removed item.
int remove_item() {
	int item = buf[0];
	for (int i = 1; i < count; ++i) {
		buf[i - 1] = buf[i];
	}
	count--;
	return item;
}

// Consumer thread, consumes items.
void* consumer(void *arg)
{
	int id = *((int*)arg);
	int item;

	while(1)
	{
		sem_wait(&full);
		sem_wait(&mutex);

		item = remove_item();
		printf("\t\t\tConsumer nr %d consumed item %d\n", id, item);

		sem_post(&mutex);
		sem_post(&empty);

		sleep((rand()%5)+1);
	}
}
