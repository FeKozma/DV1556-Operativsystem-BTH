#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>

#define bufSize 5

int nProducer = 2;
int nConsumer = 2;
int buf[bufSize];

sem_t mutex;
sem_t semEnd;
sem_t semStart;


void* consumer(void *arg);
void* producer(void *arg);

int main(int argc, char **argv)
{
	srand(time(NULL));

	if (argc > 1)
	{
		nProducer = atoi(argv[1]);
		if (argc > 2)
		{
			nConsumer = atoi(argv[2]);
		}
	}

sem_init(&semEnd, 0, 0);
sem_init(&semStart, 0, 0);
sem_init(&mutex, 0, 1);


int tmpValue;
sem_getvalue(&semEnd, &tmpValue);
printf("%d\n", tmpValue);
	
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

void * consumer(void *arg)
{
	int nr = *((int*)arg);
	int bufEnd;
	int bufStart;

	while(1==1)
	{
		sem_wait(&mutex);

		sem_getvalue(&semEnd, &bufEnd);
		sem_getvalue(&semStart, &bufStart);

		if (bufEnd % bufSize != bufStart % bufSize)
		{

			printf("			Consumer nr %d consumed item %d\n", nr, buf[bufStart % bufSize]);
			//buf[bufStart % bufSize] = 0;


			sem_post(&semStart);
			

			//int temp;
			//sem_getvalue(&semStart, &temp);
			//printf("			value: %d\n", temp);
			
			
		}
	

		sem_post(&mutex);

		sleep((rand()%5)+1);
	}
}

void * producer(void *arg)
{

	int nr = *((int*)arg);
	int bufEnd;
	int bufStart;

	while(1==1)
	{

		sem_wait(&mutex);

		sem_getvalue(&semEnd, &bufEnd);
		sem_getvalue(&semStart, &bufStart);
		if ((bufEnd + 1) % bufSize != bufStart % bufSize)
		{
			buf[bufEnd % bufSize] = (rand()%9000) + 1000;
			printf("Producer nr %d produced item %d\n", nr, buf[bufEnd % bufSize]);

			sem_post(&semEnd);
			

			//int temp;
			//sem_getvalue(&semEnd, &temp);
			//printf("value: %d\n", temp);

		}
		

		sem_post(&mutex);
		

		sleep((rand()%5)+1);
	}
}
