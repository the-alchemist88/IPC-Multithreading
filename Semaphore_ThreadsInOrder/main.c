#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define	CYCLES_NUMBER	500

sem_t sem1;
sem_t sem2;

void* incrementer1_th();
void* incrementer2_th();

static unsigned long ulGlobal = 0;

int main(int argc, char **argv)
{
	int iRet; // Used for checking the return value of system functions

// initialize sem1 with 1 in order to start incrementer1 thread first then signal to incrementer2 thread via sem2 when finish using shared resource(ulGlobal)
	sem_init(&sem1,0,1);
	sem_init(&sem2,0,0);

	pthread_t incrementer1;
	pthread_t incrementer2;

	iRet = pthread_create(&incrementer1,NULL,incrementer1_th, NULL);
	if(iRet != 0){
		perror("Error on creating thread");
	    exit(0);
	}

    iRet = pthread_create(&incrementer2,NULL,incrementer2_th, NULL);

    if(iRet != 0)
    {
    	perror("Error on creating thread");
    	exit(0);
    }

    pthread_join(incrementer1, NULL);
    pthread_join(incrementer2, NULL);

    sem_destroy(&sem1);
    sem_destroy(&sem2);

	printf("usGlobal = %ld\n",ulGlobal);
	return 0;
}

void* incrementer1_th()
{

	unsigned long int counter_1 = 0;
	unsigned long int iRet;

	printf("%s started\n",__FUNCTION__);

	while(counter_1 < CYCLES_NUMBER)
	{
		iRet = sem_wait(&sem1);
		if(iRet != 0)
	    {
	    	perror("Error on sem_wait on incrementer1_th");
	    	exit(0);
	    }

		ulGlobal++;
		printf("counter_1 = %ld, ulGlobal = %ld, %s\n", counter_1, ulGlobal, __FUNCTION__);
		counter_1++;

		iRet = sem_post(&sem2);
		if(iRet != 0)
	    {
	    	perror("Error on sem_post on incrementer2_th");
	    	exit(0);
	    }
	}

	printf("%s ended, counter_1 = %ld\n", __FUNCTION__, counter_1);
	return NULL;
}

void* incrementer2_th()
{
	unsigned long int counter_2 = 0;
	unsigned long int iRet;

	printf("\t%s started\n",__FUNCTION__);

	while(counter_2 < CYCLES_NUMBER )
	{
		iRet = sem_wait(&sem2);
		if(iRet != 0)
	    {
	    	perror("Error on sem_wait on incrementer1_th");
	    	exit(0);
	    }

		ulGlobal++;
		printf("\tcounter_2 = %ld, ulGlobal = %ld, %s\n", counter_2, ulGlobal, __FUNCTION__);
		counter_2++;

		iRet = sem_post(&sem1);
		if(iRet != 0)
	    {
	    	perror("Error on sem_post on incrementer2_th");
	    	exit(0);
	    }
	}

	printf("%s ended, counter_2 = %ld\n",__FUNCTION__, counter_2);
	return NULL;
}
