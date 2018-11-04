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
#include <math.h>

#define MYPORT "23471"   //my port number for ServerB
#define HOST "localhost"
int LinkId = -1;
//Size is in bits
int Size = 0;
int Power = 0;
float Bandwidth = 0;
float Length = 0;
float Velocity = 0 ;
float NoisePower = 0;

float calculate(int linkId){
	//This will calculate TProp in microseconds
	float TProp = (Length/Velocity)/10;
	float NoisePowerInWatts = pow(10,(NoisePower/10)-3);
	float SignalPowerInWatts = pow(10,(Power/10)-2);
	//The capacity is in Mbps
	float Capacity = Bandwidth*(log(1 + (SignalPowerInWatts/NoisePowerInWatts))/log(2));
	//Trans is in microseconds
	float TTrans = (Size/Capacity);
	float EndToEndDelay = 2*TProp + TTrans;
	return EndToEndDelay;
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
			perror("serverC: socket");
			continue;
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("serverC: bind");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "serverC: failed to bind socket\n");
		return 0;
	}
	freeaddrinfo(servinfo);
	printf( "The Server C is up and running using UDP on port <%s>.\n", MYPORT);
	while(1){
		addr_len = sizeof their_addr;
		char function[3];
		int pre_num = 0;
		float result = 0;
		// recvfrom(sockfd, function, sizeof function , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (int *)& LinkId, sizeof LinkId , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (int *)& Size, sizeof Size , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (int *)& Power, sizeof Power , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (float *)& Bandwidth, sizeof Bandwidth , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (float *)& Length, sizeof Length , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (float *)& Velocity, sizeof Velocity , 0,(struct sockaddr *)&their_addr, &addr_len);
		recvfrom(sockfd, (float *)& NoisePower, sizeof NoisePower , 0,(struct sockaddr *)&their_addr, &addr_len);
		printf("Bandwidth <%f>, Length <%f>, Velocity <%f> , Noise Power <%f> \n", Bandwidth,Length, Velocity,NoisePower);




		printf("The Server C received link information of link <%d>, file size <%d>, and signal power <%d>  \n", LinkId, Size,Power);

		result = calculate(LinkId);
		//send back to aws
		sendto(sockfd, (float *)& result, sizeof result , 0,(struct sockaddr *) &their_addr, addr_len);
		// sendto(sockfd, (float *)& Bandwidth, sizeof Bandwidth , 0,(struct sockaddr *) &their_addr, addr_len);
		// sendto(sockfd, (float *)& Length, sizeof Length , 0,(struct sockaddr *) &their_addr, addr_len);
		// sendto(sockfd, (float *)& Velocity, sizeof Velocity , 0,(struct sockaddr *) &their_addr, addr_len);
		// sendto(sockfd, (float *)& NoisePower, sizeof NoisePower , 0,(struct sockaddr *) &their_addr, addr_len);
		printf("The Server C finished sending the output to AWS  \n");

	}
}
