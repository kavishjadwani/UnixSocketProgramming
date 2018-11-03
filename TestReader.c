#include<stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>

const char* getfield(char* line, int num)
{
    const char* tok;
    for (tok = strtok(line, ",");
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}
// void getData(char *buff);
int main()
{
	int LinkId = -1;
	int Bandwidth = 0;
	int Length = 0;
	int Velocity = 0 ;
	int NoisePower = 0;
	 FILE* file = fopen("database_a.csv", "r");
	 char line[1024];
    while (fgets(line, 1024, file))
    {
        char* tmp = strdup(line);
				char* tmp2 = strdup(line);
				char* tmp3 = strdup(line);
				char* tmp4 = strdup(line);
				char* tmp5 = strdup(line);
				if(atoi(getfield(tmp,1)) == 175){
					LinkId = atoi(getfield(tmp,1));
					Bandwidth = atoi(getfield(tmp2,2));
					Length = atoi(getfield(tmp3,3));
					Velocity = atoi(getfield(tmp4,4));
					NoisePower = atoi(getfield(tmp5,5));
					printf("Link Id : %d, Bandwidth : %d, Length: %d, Velocity : %d, Noise Power : %d \n", LinkId,Bandwidth,Length,Velocity,NoisePower);
					break;
				}
        free(tmp);
    }
	}
	 // char line[256];



	 // *pos = ftell(file);

	/* The following code works to read lines
	FILE *fp = NULL;
	fp = fopen("database_a.csv","r");
	if(fp == NULL){
		exit(0);
	}
	char ch;
	ch = getc(fp);
	while(ch!=EOF){
		// char *token = strtok(ch, ",");
		// printf("Token is %s ", token);
		putchar(ch);
		ch = getc(fp);

	}
    fclose(fp);*/
// }
