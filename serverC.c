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
float TProp = 0;
float TTrans = 0;
float calculate(int linkId){
	//This will calculate TProp in microseconds
	TProp = (Length/Velocity)/10;
	double NoisePowerInWatts = pow(10,(NoisePower/10)-3);

	double SignalPowerInWatts = pow(10,(Power/10)-3);
	//The capacity is in Mbps
	float Capacity = Bandwidth*(log(1 + (SignalPowerInWatts/NoisePowerInWatts))/log(2));
	//Trans is in microseconds
	TTrans = (Size/Capacity);
	float EndToEndDelay = 2*TProp + TTrans;
	TTrans = TTrans/1000;
	TProp = TProp/1000;
	EndToEndDelay = EndToEndDelay/1000;
	/*
	Debug :
	printf("*********************************************************\n");
	printf("The Link id is %d \n",LinkId);
	printf("The Bandwidth is %f \n",Bandwidth);
	printf("The Length is %f \n",Length);
	printf("The Velocity is %f \n",Velocity);
	printf("The Noise Power in db is %f \n",NoisePower);
	printf("The Noise Power in Watts is %f \n",NoisePowerInWatts);
	printf("The Capacity is %f \n",Capacity);
	printf("The Transmission Time Delay in ms is %f \n",TTrans);
	printf("The Propagation Delay in ms is %f \n",TProp);
	printf("The Delay in ms is %f \n",EndToEndDelay);
	printf("The File Size in ms is %d \n",Size);
	printf("The Signal Power in db is %d \n",Power);
	printf("The Signal Power in Watts is %f \n",SignalPowerInWatts);
	printf("*********************************************************");
*/
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
	//Keep the Socket for Server C ON
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
		printf("The Server C received link information of link <%d>, file size <%d>, and signal power <%d>  \n", LinkId, Size,Power);
		result = calculate(LinkId);
		printf("The Server C finished the calculation for link <%d> \n", LinkId);
		//send back to aws
		sendto(sockfd, (float *)& result, sizeof result , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& TTrans, sizeof TTrans , 0,(struct sockaddr *) &their_addr, addr_len);
		sendto(sockfd, (float *)& TProp, sizeof TProp , 0,(struct sockaddr *) &their_addr, addr_len);
		printf("The Server C finished sending the output to AWS  \n");

	}
}
