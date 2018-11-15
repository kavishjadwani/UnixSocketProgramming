/*
** client.c
** Kavish Jadwani
** ID: 6917934471
** Session 2
*/

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
#include <signal.h>
#include <ctype.h>
#include <math.h>

#define AWSPORT "25471"   //aws TCP port
#define HOST "localhost"
#define IPADDRESS "127.0.0.1" // local IP address


int total_num = 0;
int data[30000];

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}

int main(int argc, char* argv[]){
	char function_name[3];
	strcpy(function_name,argv[1]);
	long conv1 = strtol(argv[1], NULL, 10);
	int linkId = conv1;
	long conv2 = strtol(argv[2], NULL, 10);
	int size = conv2;
	long conv3 = strtol(argv[3], NULL, 10);
	int power = conv3;
	int k = 0;
	for( k = 0; k < 3; k ++){
		function_name[k] = toupper(function_name[k]);
	}
	//set up TCP --from Beej
	int sockfd = 0;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(HOST, AWSPORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and connect to the first we can----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("client: socket");
			continue;
		}
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "client: failed to connect. \n");
		exit(0);
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure
	printf("The client is up and running. \n");
	send(sockfd,  (int *)&linkId, sizeof (linkId), 0);
	send(sockfd,  (int *)&size, sizeof (size), 0);
	send(sockfd,  (int *)&power, sizeof (power), 0);
	// send(sockfd, (char *)& data, sizeof data, 0);
	printf("The client sent ID= <%d>, size= <%d> and power = <%d> to AWS \n",linkId,size,power);
	float result = -1;
	recv(sockfd, (float *)&result, sizeof result, 0);
	if(result!=-1){
		// result = round(result);
		printf("The delay for link <%d> is <%.2f> ms \n", linkId,round(result*100)/100);
	}
	else
		printf("Found no matches for link <%d> \n",linkId);
	result = -1;
}
