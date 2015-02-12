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

#define					OVERFLOW_CEILING	20
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

		if(sharedData <= 0)
		{
			pthread_cond_wait(&dataPresentCondition, &dataMutex);
		}

		sharedData--;

		printf("Consuming 1 data unit.\n");
		pthread_mutex_unlock(&dataMutex);
		pthread_cond_signal(&dataConsumedCondition);
		sleep(2);	//consuming data
	}
}

void *theProducer(void * producerID)
{
	while(1)
	{
		printf("Producer locking dataMutex.\n");
		pthread_mutex_lock(&dataMutex);
		printf("Acquired lock.\n");

		if(sharedData >= OVERFLOW_CEILING)
		{
			pthread_cond_wait(&dataConsumedCondition, &dataMutex);
		}

		sharedData++;

		printf("Produced 1 data unit.\n");
		pthread_mutex_unlock(&dataMutex);
		pthread_cond_signal(&dataPresentCondition);
		sleep(3);	//producing data
	}
	return NULL;
}

int main(int argc, char **argv)
{
	pthread_t             consumers[NUMTHREADS];
	int                   i;
	
	sharedData = 0;
	pthread_t producer = pthread_create(producer, NULL, theproducer, (void *)i);

	for(i=0; i < NUMTHREADS; i++)
	{
		pthread_create(&threads[t], NULL, theConsumer)
	}
	
	printf("Wait for the threads to complete, and release their resources\n");
	for (i=0; i <NUMTHREADS; ++i) {
		rc = pthread_join(thread[i], NULL);
		checkResults("pthread_join()\n", rc);
	}
	
	printf("Clean up\n");
	rc = pthread_mutex_destroy(&dataMutex);
	rc = pthread_cond_destroy(&dataPresentCondition);
	printf("Main completed\n");
	return 0;
}

//\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n