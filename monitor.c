/*
** monitor.c
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
#define PORT "26471" // the TCP port of aws that monitor connect to

#define MAXDATASIZE 4000 // max number of bytes we can get at once
#define IPADDRESS "127.0.0.1"
#define HOST "localhost"

int total_num = 0;
int data[30000];
int LinkId = -1;
int Size;
int Power;
float TTrans;
float TProp;
float result = -1;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(void)
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo, *p;
	int rv;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((rv = getaddrinfo(IPADDRESS, PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("monitor : socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			perror("monitor : connect");
			close(sockfd);
			continue;
		}

		break;
	}

	if (p == NULL) {
		fprintf(stderr, "monitor : failed to connect\n");
		return 2;
	}
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
			s, sizeof s);
	printf("The monitor is up and running.\n");
	freeaddrinfo(servinfo); // all done with this structure

	while (1){
		numbytes  = recv(sockfd, (int *)&LinkId, sizeof LinkId, 0);
		if(numbytes==-1)
			exit(1);
		recv(sockfd, (int *)&Size, sizeof Size, 0);
		recv(sockfd, (int *)&Power, sizeof Power, 0);
		result = -1;
		int temp = 0;
		while(LinkId != -1)
		{
			printf("The  monitor received link ID = <%d>, size = <%d>, and power = <%d> from the AWS\n",LinkId,Size,Power);
			temp = -1;
			recv(sockfd, (float *)&result, sizeof result, 0);
			recv(sockfd, (float *)&TTrans, sizeof TTrans, 0);
			recv(sockfd, (float *)&TProp, sizeof TProp, 0);
			if(result!=-1){
				printf("The result for link  <%d> :\n",  LinkId);
				printf("Tt = <%f> ms,\n", TTrans);
				printf("Tp = <%f> ms,\n", TProp);
				printf("Delay = <%f> ms\n", result);
			}else
				printf("Found no matches for link <%d> \n",LinkId);
			LinkId = -1;
		}
	}
	close(sockfd);
	return 0;
}
