/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 20

enum { MAXLINES = 30 };

int numOfCustomers;

void getInput();

struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];


void *CustomerThread(void *currentCust){

	struct Customer *info = (struct Customer*)currentCust;

	printf("hi it's %d\n",info->id);
	printf("service time = %d\n",info->serviceTime);

	pthread_exit(NULL);

}


int main(){

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

	int k = 0;

	while (k < numOfCustomers){
		pthread_join(&threads[k],NULL);
		k++;

	}

     }
	printf("the end\n\n");
   /* Last thing that main() should do */
	exit(0);

   return 0;
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
    printf("%d\n", i);
    srand(time(0));
    int j = 1;
	numOfCustomers = atoi(lines[0]);
	printf("num of customers = %d\n",numOfCustomers);
	while (j < i){

    		printf("%s\n", lines[j]);
		j++; 
	}   

	
	int k = 1;
	int custIndex = 0;

	while (k <= numOfCustomers){
	
	printf("lines[k] = %s\n",lines[k]);

	char* token;
	char* tmp = lines[k];
	token = strtok(tmp,":");
	printf("token =%s \n",token);
	customers[custIndex].id = atoi(token);

	char* token2;

	char* tmp2 = lines[1];

	token = strtok(NULL,",");

	printf("token = %s\n",token);
	customers[custIndex].arrivalTime = atoi(token);

	
	token = strtok(NULL,",");

	printf("token = %s\n",token);
	customers[custIndex].serviceTime = atoi(token);

	k++;
	custIndex++;


	

	}


	printf("\n\n");

	int l = 0;

	while (l < numOfCustomers){
		printf("customer %d id = %d\n",l,customers[l].id);
	printf("customer %d  arrival = %d\n",l,customers[l].arrivalTime);
	printf("customer %d service = %d\n",l,customers[l].serviceTime);

	
		l++;
	}	

}
