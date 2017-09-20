#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>



int nProducer = 2;
int nConsumer = 2;
const int bufSize = 5;
int buf[5];
int bufPos = 0; 
 
pthread_mutex_t count_mutex;

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
	while(1==1)
	{
		if (bufPos != 0)
		{
			pthread_mutex_lock(&count_mutex);
			bufPos--;
			printf("			Consumer nr %d consumed item %d\n", nr, buf[bufPos]);
			buf[bufPos] = 0;

			pthread_mutex_unlock(&count_mutex);	
		}
		sleep((rand()%5)+1);
	}
}

void * producer(void *arg)
{

	int nr = *((int*)arg);

	while(1==1)
	{
		if (bufPos != 5)
		{
			pthread_mutex_lock(&count_mutex);
			buf[bufPos] = (rand()%9000) + 1000;
			printf("Producer nr %d produced item %d\n", nr, buf[bufPos]);
			bufPos++;

			pthread_mutex_unlock(&count_mutex);	
		}
		sleep((rand()%5)+1);
	}
}
