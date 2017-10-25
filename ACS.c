/* https://stackoverflow.com/questions/4237084/reading-from-a-file-and-storing-in-array  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

enum { MAXLINES = 30 };

int numOfCustomers;

void getInput();

struct Customer{
		
	int id;
	int serviceTime;
	int arrivalTime;
};

struct Customer customers[30];

int main(){

	getInput();
	
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
