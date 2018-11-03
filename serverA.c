#include <stdio.h>
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

#define MYPORT "21471"   //my port number for ServerA
#define HOST "localhost"
int LinkId = -1;
float Bandwidth = 0;
float Length = 0;
float Velocity = 0 ;
float NoisePower = 0;
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

void read_file(int linkId){

	 FILE* file = fopen("database_a.csv", "r");
	 char line[1024];
    while (fgets(line, 1024, file))
    {
        char* tmp = strdup(line);
				char* tmp2 = strdup(line);
				char* tmp3 = strdup(line);
				char* tmp4 = strdup(line);
				char* tmp5 = strdup(line);
				if(atoi(getfield(tmp,1)) == linkId){
					LinkId = atoi(getfield(tmp,1));
					Bandwidth = atof(getfield(tmp2,2));
					Length = atof(getfield(tmp3,3));
					Velocity = atof(getfield(tmp4,4));
					NoisePower = atof(getfield(tmp5,5));
					// printf("Link Id : %d, Bandwidth : %d, Length: %d, Velocity : %d, Noise Power : %d \n", LinkId,Bandwidth,Length,Velocity,NoisePower);
					break;
				}
        free(tmp);
    }
}
int main(void){
	// set up UDP  -- From Beej
	int sockfd;
	int rv;
	struct addrinfo hints;  // the struct addrinfo have already been filled with relevant info
	struct addrinfo *servinfo; //point out the result
	struct sockaddr_storage their_addr;
	struct addrinfo *p;  //tempoary point
	socklen_t addr_len;
	memset(&hints, 0, sizeof hints);  // make sure the struct is empty
	hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_DGRAM; // UDP dgram sockets
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(HOST, MYPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 0;
	}
// loop through all the results and bind to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("serverA: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("serverA: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "serverA: failed to bind socket\n");
		return 0;
	}
	freeaddrinfo(servinfo);
	printf( "The Server A is up and running using UDP on port <%s>.\n", MYPORT);


	while(1){
		addr_len = sizeof their_addr;
		char function[3];
		int pre_num = 0;
		int result = 0;
		// recvfrom(sockfd, function, sizeof function , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (char *)& pre_num, sizeof pre_num , 0,(struct sockaddr *)&their_addr, &addr_len);
		printf("The Server A received input <%d>  \n", pre_num);
		read_file(pre_num);
		if(LinkId == pre_num)
			printf("The server A has found <1> match \n");
		else
			printf("The server A has found <0> match \n");

		result = 999;
		// printf("The Server A has successfully finished the reduction  %d \n", result);

		//send back to aws
		sendto(sockfd, (int *)& LinkId, sizeof LinkId , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& Bandwidth, sizeof Bandwidth , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& Length, sizeof Length , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& Velocity, sizeof Velocity , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& NoisePower, sizeof NoisePower , 0,(struct sockaddr *) &their_addr, addr_len);
		printf("The Server A finished sending the output to AWS  \n");
	}
}
