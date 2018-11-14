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
int LinkId;
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
		// printf("The link id is reset to  %d for the second time \n",LinkId);
		// for (int c = 1; c <= 32767; c++)
		// 	for (int d = 1; d <= 32767; d++)
		// 	{}
		// buf format: function:::<orther part>
		// if function is search: search:::difinition("0" if not found):::
		// onesimilarword("0 if not found"):::onesimilarword difinition
		numbytes  = recv(sockfd, (int *)&LinkId, sizeof LinkId, 0);
		if(numbytes==-1)
			exit(1);
		// printf("The link id received is  %d for the second time \n",LinkId);
		recv(sockfd, (int *)&Size, sizeof Size, 0);
		recv(sockfd, (int *)&Power, sizeof Power, 0);
		// if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
	  //   perror("recv");
	  //   exit(1);
		// }
		// printf("debug: receive from aws <%s>\n",buf);
		// printf("The  monitor received link ID = <%d>\n",LinkId);

		// printf("The  monitor received link ID = <%d>, size = <%d>, and power = <%d> from the AWS\n",LinkId,Size,Power);
		// recv(sockfd, (float *)&result, sizeof result, 0);
		// recv(sockfd, (float *)&TTrans, sizeof TTrans, 0);
		// recv(sockfd, (float *)&TProp, sizeof TProp, 0);
		// if(result!=-1){
		// 	printf("The result for link  <%d> :\n",  LinkId);
		// 	printf("Tt = <%f> \n", TTrans);
		// 	printf("Tp = <%f> \n", TProp);
		// 	printf("Delay = <%f> \n", result);
		// }
		// else
			// printf("Found no matches for link <%d> \n",LinkId);
		result = -1;
		// buf[numbytes] = '\0';
		int temp = 0;
		while(LinkId != 0)
		{
			printf("The  monitor received link ID = <%d>, size = <%d>, and power = <%d> from the AWS\n",LinkId,Size,Power);
			temp = -1;
			recv(sockfd, (float *)&result, sizeof result, 0);
			recv(sockfd, (float *)&TTrans, sizeof TTrans, 0);
			recv(sockfd, (float *)&TProp, sizeof TProp, 0);
			if(result!=-1){
				printf("The result for link  <%d> :\n",  LinkId);
				printf("Tt = <%f> \n", TTrans);
				printf("Tp = <%f> \n", TProp);
				printf("Delay = <%f> \n", result);
			}else
				printf("Found no matches for link <%d> \n",LinkId);
				// printf("The  monitor received link ID = <%d>\n",LinkId);
			//printf("debug: numbytes=%d, monitor received %s\n",numbytes,buf);
			// printf("debug: receive from aws <%s>\n",buf); 	// format: function:::<orther part>
															// if function is search: search:::difinition("0" if not found):::
															// onesimilarword("0 if not found"):::onesimilarword difinition
			// numbytes=0;
			LinkId = 0;
		}
		// recv(sockfd, (int *)&LinkId, sizeof LinkId, 0);
		// recv(sockfd, (int *)&Size, sizeof Size, 0);
		// recv(sockfd, (int *)&Power, sizeof Power, 0);
		// exit(0);
	}
	close(sockfd);

	return 0;
}
