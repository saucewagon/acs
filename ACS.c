/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 20

#define NUM_CLERKS 2
#define NUM_QUEUES 4

int queueLength[NUM_QUEUES];


enum { MAXLINES = 30 };

int numOfCustomers;

void getInput();
void initializeQueues();
int chooseMinLengthQueue();


struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];

pthread_mutex_t lock;


void *ClerkThread(void * clerkNum){

	while(1){


	}

	pthread_exit(NULL);

	return NULL;

}

void *CustomerThread(void *currentCust){

	pthread_mutex_lock(&lock);

	struct Customer *info = (struct Customer*)currentCust;

	usleep(info->arrivalTime*100000);

	printf("A customer arrives: customer ID: %d\n",info->id);

	int minQueue = chooseMinLengthQueue();
	queueLength[minQueue]++;
	printf("A  customer enters a queue: the queue ID: %d, length of queue: %d\n",minQueue,5);
	
	printf("A clerk starts serving a customer: customer Id and clerk id\n");

	usleep(info->serviceTime*100000);

	printf("A clerk finishes serving  customer %d\n",info->id);

	pthread_mutex_unlock(&lock);

	pthread_exit(NULL);

}


int main(){


	pthread_mutex_init(&lock,NULL);

	initializeQueues();

	getInput();
	pthread_t threads[numOfCustomers];	
   int rc;
   long t;
   for(t=0;t<numOfCustomers;t++){

     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, CustomerThread, (void *)&customers[t]);

     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);

       }


     }

	int k = 0;

	while (k < numOfCustomers){
		pthread_join(threads[k],NULL);
		k++;

	}

	pthread_mutex_destroy(&lock);

	exit(0);

   
}

void initializeQueues(){

	int i;
	while (i < NUM_QUEUES){
	
		queueLength[i] = 0;
		i++;
	}

}

int chooseMinLengthQueue(){

	int minQueue = queueLength[0];
	int i;

	while (i < NUM_QUEUES){

		if (queueLength[i] < minQueue){

			minQueue = queueLength[i];

		}
	i++;

	}
	
	return minQueue;
}

void getInput(){

	int i = 0;
    char lines[MAXLINES][BUFSIZ];
    FILE *fp = fopen("customers.txt", "r");

    if (fp == 0)
    {
        fprintf(stderr, "failed to open input.txt\n");
        exit(1);
    }
    while (i < MAXLINES && fgets(lines[i], sizeof(lines[0]), fp))
    {
        lines[i][strlen(lines[i])-1] = '\0';
        i = i + 1;
    }
    fclose(fp);
/*    printf("%d\n", i);
  */  srand(time(0));
    int j = 1;
	numOfCustomers = atoi(lines[0]);
/*	printf("num of customers = %d\n",numOfCustomers);
*/
	
	int k = 1;
	int custIndex = 0;

	while (k <= numOfCustomers){
	
/*	printf("lines[k] = %s\n",lines[k]);
*/
	char* token;
	char* tmp = lines[k];
	token = strtok(tmp,":");
/*	printf("token =%s \n",token);
*/	customers[custIndex].id = atoi(token);

	char* token2;

	char* tmp2 = lines[1];

	token = strtok(NULL,",");

/*	printf("token = %s\n",token);
*/	customers[custIndex].arrivalTime = atoi(token);

	
	token = strtok(NULL,",");

/*	printf("token = %s\n",token);
*/	customers[custIndex].serviceTime = atoi(token);

	k++;
	custIndex++;


	

	}
	
}
