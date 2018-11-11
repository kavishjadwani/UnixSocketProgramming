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

#define AWSPORT "26471"   //aws TCP port
#define HOST "localhost"

int total_num = 0;
int data[30000];
int LinkId;
int Size;
int Power;
float TTrans;
float TProp;

void *get_in_addr(struct sockaddr *sa) {
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*) sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*) sa)->sin6_addr);
}
int main(void){
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
			perror("monitor: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("monitor: connect");
			continue;
		}
		break;
	}

	if (p == NULL) {
		fprintf(stderr, "monitor: failed to connect. \n");
		exit(0);
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure
	printf("The monitor is up and running. \n");
	float result = -1;
	// while (1) {
	recv(sockfd, (int *)&LinkId, sizeof LinkId, 0);
	recv(sockfd, (int *)&Size, sizeof Size, 0);
	recv(sockfd, (int *)&Power, sizeof Power, 0);
	printf("The  monitor received link ID = <%d>, size = <%d>, and power = <%d> from the AWS\n",LinkId,Size,Power);
  recv(sockfd, (float *)&result, sizeof result, 0);
	recv(sockfd, (float *)&TTrans, sizeof TTrans, 0);
	recv(sockfd, (float *)&TProp, sizeof TProp, 0);


	if(result!=-1){
		printf("The result for link  <%d> :\n",  LinkId);
		printf("Tt = <%f> \n", TTrans);
		printf("Tp = <%f> \n", TProp);
		printf("Delay = <%f> \n", result);
	}
	else
		printf("Found no matches for link <%d> \n",LinkId);
	result = -1;
	close(sockfd);
	return 0;

}
