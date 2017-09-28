#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

const int bufSize = 5;
int buf[5];

int nProducer = 2;
int nConsumer = 2;

typedef sem_t semaphore;
semaphore mutex;
semaphore empty;
semaphore full;

int count = 0;

void* consumer(void *arg);
void* producer(void *arg);

int main(int argc, char **argv)
{
	srand(time(NULL));

	sem_init(&mutex, 0, 1);
	sem_init(&empty, 0, bufSize);
	sem_init(&full, 0, 0);

	if (argc > 1)
	{
		nProducer = atoi(argv[1]);
		if (argc > 2)
		{
			nConsumer = atoi(argv[2]);
		}
	}

	pthread_t* children;
	children = malloc((nProducer + nConsumer) * sizeof(pthread_t));
	int consumerNr[nConsumer];
	int producerNr[nProducer];

	for (int i = 0; i < nProducer; i++)
	{
		producerNr[i] = i;

		pthread_create(&(children[i]),
			NULL,
			producer,
			&producerNr[i]);

	}

	for (int i = 0; i < nConsumer; i++)
	{
		consumerNr[i] = i;

		pthread_create(&(children[i]),
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

int remove_item() {
	int item = buf[0];
	for (int i = 1; i < count; ++i) {
		buf[i - 1] = buf[i];
	}
	count--;
	return item;
}

void * consumer(void *arg)
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

int produce_item() {
	return (rand()%9000) + 1000;
}

void insert_item(int item) {
	buf[count] = item;
	count++;
}

void * producer(void *arg)
{
	int nr = *((int*)arg);
	int item;

	while(1)
	{
		item = produce_item();

		sem_wait(&empty);
		sem_wait(&mutex);

		insert_item(item);
		printf("Producer nr %d produced item %d\n", nr, item);

		sem_post(&mutex);
		sem_post(&full);

		sleep((rand()%5)+1);
	}
}