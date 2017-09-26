#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>



int nProducer = 2;
int nConsumer = 2;
const int bufSize = 5;
int buf[5];
int bufEnd = 0; 
int bufStart = 0;
 
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
		if (bufEnd != bufStart)
		{
			pthread_mutex_lock(&count_mutex);

			printf("			Consumer nr %d consumed item %d\n", nr, buf[bufStart]);
			buf[bufStart] = 0;


			bufStart++;
			bufStart = bufStart % bufSize;

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
		if ((bufEnd + 1) % bufSize != bufStart)
		{
			pthread_mutex_lock(&count_mutex);

			buf[bufEnd] = (rand()%9000) + 1000;
			printf("Producer nr %d produced item %d\n", nr, buf[bufEnd]);
			bufEnd++;
			bufEnd = bufEnd % bufSize;			

			pthread_mutex_unlock(&count_mutex);	
		}
		sleep((rand()%5)+1);
	}
}
