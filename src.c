#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef enum { FALSE, TRUE } boolean;


// Adapted from http://publib.boulder.ibm.com/infocenter/iseries/v5r3/index.jsp?topic=/rzahw/rzahwe17rx.htm
// Fixed to eliminate nonstandard pthread_getthreadid_np()
// Bug fixes

#define checkResults(string, val) {             \
if (val) {                                     \
printf("Failed with %d at %s", val, string); \
exit(1);                                     \
}                                              \
}

#define					OVERFLOW_CEILING	10
#define                 NUMTHREADS     2
pthread_mutex_t         dataMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t          dataPresentCondition = PTHREAD_COND_INITIALIZER;
pthread_cond_t 			dataConsumedCondition = PTHREAD_COND_INITIALIZER;
int sharedData;

void *theConsumer(void *threadid)
{ 
	while(1)
	{
		printf("Consumer locking dataMutex.\n");
		pthread_mutex_lock(&dataMutex);
		printf("Acquired lock.\n");

		while(sharedData <= 0)
		{
			printf("UNDERFLOW AVERTED\n");
			pthread_cond_wait(&dataPresentCondition, &dataMutex);
		}

		sharedData--;

		printf("Consuming 1 data unit.\n");
		pthread_mutex_unlock(&dataMutex);
		pthread_cond_signal(&dataConsumedCondition);
		printf("The number of data items in stack: %d\n", sharedData);
		sleep(5);	//consuming data
	}
}

void *theProducer(void * producerID)
{
	while(1)
	{
		printf("Producer locking dataMutex.\n");
		pthread_mutex_lock(&dataMutex);
		printf("Acquired lock.\n");

		while(sharedData >= OVERFLOW_CEILING)
		{
			printf("OVERFLOW AVERTED\n");
			pthread_cond_wait(&dataConsumedCondition, &dataMutex);
		}

		sharedData++;

		printf("Produced 1 data unit.\n");
		pthread_mutex_unlock(&dataMutex);
		pthread_cond_signal(&dataPresentCondition);

		printf("The number of data items in stack: %d\n", sharedData);
		sleep(1);	//producing data
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t             consumers[NUMTHREADS];
	long long             i;
	
	sharedData = 0;
	pthread_t producer = pthread_create(&producer, NULL, theProducer, NULL);

	for(i=0; i < NUMTHREADS; i++)
	{
		pthread_create(&consumers[i], NULL, theConsumer, (void *) i);
	}
	
	printf("Wait for the threads to complete, and release their resources\n");
	for (i=0; i <NUMTHREADS; ++i) {
		pthread_join(consumers[i], NULL);
	}
	
	printf("Clean up\n");
	pthread_mutex_destroy(&dataMutex);
	pthread_cond_destroy(&dataPresentCondition);
	pthread_cond_destroy(&dataConsumedCondition);
	printf("Main completed\n");
	return 0;
}

//\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n