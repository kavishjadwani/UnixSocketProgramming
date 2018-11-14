/*
** aws.c
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

#define TCPPORT "25471"   //TCP port
#define MONITORPORT "26471"
#define UDPPORT "24471"		//UDP port
#define HOST "localhost"
#define BACKLOG 10 // how many pending connections queue will hold
#define PORTA "21471"
#define PORTB "22471"
#define PORTC "23471"
#define IPADDRESS "127.0.0.1" // local IP address

int LinkId = 0;
int Size = 0;
int Power = 0;
float Bandwidth = 0;
float Length = 0;
float Velocity = 0 ;
float NoisePower = 0;
float TTrans = 0;
float TProp = 0;
int LinkIdA = 0;
float BandwidthA = 0;
float LengthA = 0;
float VelocityA = 0 ;
float NoisePowerA = 0;
int LinkIdB = 0;
float BandwidthB = 0;
float LengthB = 0;
float VelocityB = 0 ;
float NoisePowerB = 0;
void sigchld_handler(int s){
    int saved_errno = errno;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}
void *get_in_addr(struct sockaddr *sa){
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
//Send information to server C and get result
float compute(){
  int mysock;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char ch = 'C';
  char* backserver_port;
  if(ch == 'A')
    backserver_port = PORTA;
  else if (ch == 'B')
    backserver_port = PORTB;
  else if( ch == 'C')
    backserver_port = PORTC;
  //set up UDP -- from Beej
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
  hints.ai_socktype = SOCK_DGRAM;

 //Create a parent socket with client
  if ((rv = getaddrinfo(HOST, backserver_port, &hints, &servinfo))!= 0) {
	   fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
  float finalResult;
	//using UDP to send the data
	sendto(mysock, (int *)& LinkId, sizeof LinkId, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (int *)& Size, sizeof Size, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (int *)& Power, sizeof Power, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (float *)& Bandwidth, sizeof Bandwidth, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (float *)& Length, sizeof Length, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (float *)& Velocity, sizeof Velocity, 0, p->ai_addr,p->ai_addrlen);
  sendto(mysock, (float *)& NoisePower, sizeof NoisePower, 0, p->ai_addr,p->ai_addrlen);
  printf("The AWS sent link ID= <%d>, size = <%d>, power = <%d> and link information to Backend-Server C using UDP over port <%s>\n", LinkId, Size,Power,PORTC);
	recvfrom(mysock, (float *)& finalResult, sizeof finalResult, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& TTrans, sizeof TTrans, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& TProp, sizeof TProp, 0 , NULL, NULL);
  printf("The AWS received outputs from Backend-Server C using UDP over port <%s> \n",PORTC);
	return finalResult;
}
// using UDP to calculate the result for back-server
int getDataB(int linkid){
  int mysock;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char ch = 'B';
  char* backserver_port;
  if(ch == 'A')
    backserver_port = PORTA;
  else if (ch == 'B')
    backserver_port = PORTB;
  else if( ch == 'C')
    backserver_port = PORTC;
  //set up UDP -- from Beej
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(HOST, backserver_port, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and make a socket----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	//using UDP to send the data
	sendto(mysock, (int *)& linkid, sizeof linkid, 0, p->ai_addr,p->ai_addrlen);
  printf("The AWS sent link ID= <%d> to Backend-Server <B> using UDP over port <%s>\n", linkid,PORTB);
	recvfrom(mysock, (float *)& LinkIdB, sizeof LinkIdB, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& BandwidthB, sizeof BandwidthB, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& LengthB, sizeof LengthB, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& VelocityB, sizeof VelocityB, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& NoisePowerB, sizeof NoisePowerB, 0 , NULL, NULL);
	return LinkIdB;
}
int getDataA(int linkid){
  int mysock;
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char ch = 'A';
  char* backserver_port;
  if(ch == 'A')
    backserver_port = PORTA;
  else if (ch == 'B')
    backserver_port = PORTB;
  else if( ch == 'C')
    backserver_port = PORTC;
  //set up UDP -- from Beej
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    if ((rv = getaddrinfo(HOST, backserver_port, &hints, &servinfo))
			!= 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	// loop through all the results and make a socket----Beej
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((mysock = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("talker: socket");
			continue;
		}
		break;
	}
	if (p == NULL) {
		fprintf(stderr, "talker: failed to bind socket\n");
		return 2;
	}
	//using UDP to send the data
	sendto(mysock, (int *)& linkid, sizeof linkid, 0, p->ai_addr,p->ai_addrlen);
  printf("The AWS sent link ID= <%d> to Backend-Server <A> using UDP over port <%s>\n", linkid,PORTA);
	recvfrom(mysock, (int *)& LinkIdA, sizeof LinkIdA, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& BandwidthA, sizeof BandwidthA, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& LengthA, sizeof LengthA, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& VelocityA, sizeof VelocityA, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& NoisePowerA, sizeof NoisePowerA, 0 , NULL, NULL);
	return LinkIdA;
}
void clear_dead_process(){
	struct sigaction sa; //sigaction() code is responsible for reaping zombie processes
						//that appear as the fork()ed child processes exit.
	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}
}
// setup TCP at port
int setupTCP(char* port){
	int rv; // use for error checking of getaddrinfo()
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes=1;
	memset(&hints, 0, sizeof hints); //Zero the whole structure before use
	hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
	hints.ai_socktype = SOCK_STREAM; //TCP SOCK_STREAM sockets
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
		// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo); // all done with this structure
	if (p == NULL)  {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	clear_dead_process();
	return sockfd;
}
int main(){
	//set up TCP --from Beej
	int sockfd, new_fd,sockfd_M, new_fd_M;;  // listen on sock_fd, new connection on new_fd
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; // connector's address information
  socklen_t sin_size;
  sockfd = setupTCP(TCPPORT);
  sockfd_M = setupTCP(MONITORPORT);
	printf( "The AWS is up and running. \n");
      int monitorOn=0;
      while (monitorOn == 0){
        // child socket connect with monitor
        sin_size = sizeof their_addr;
        new_fd_M = accept(sockfd_M, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd_M == -1) {
          perror("accept");
          continue;
        }
        if (!fork()) { // this is the child process
          close(sockfd_M); // child doesn't need the listener
          monitorOn=1;
          continue;
        }
        close(new_fd_M);  // parent doesn't need this
      }//Close while loop for monitor

	//keep handling client query
	while(1){
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			exit(1);
		}
		int total_num;
    int linkId;
    recv(new_fd, (int *)&linkId, sizeof linkId, 0);
    recv(new_fd, (int *)&Size, sizeof Size, 0);
    recv(new_fd, (int *)&Power, sizeof Power, 0);
    send(new_fd_M, (const int *)&linkId, sizeof linkId , 0);
    send(new_fd_M, (const int *)&Size, sizeof Size , 0);
    send(new_fd_M, (const int *)&Power, sizeof Power , 0);
    printf("The AWS received link ID= <%d>, size= <%d> and power= <%d> from the client using TCP over port <%s> \n",linkId,Size,Power,TCPPORT);
    printf("The AWS sent link ID= <%d>, size= <%d> and power= <%d> to the monitor using TCP over port <%s> \n",linkId,Size,Power,MONITORPORT);

		int resultA = getDataA(linkId);
    int resultB = getDataB(linkId);
    if(resultA == linkId){
      printf("The AWS received <1> matches from Backend-Server <A> using UDP over port <%s> \n", PORTA);
      LinkId = LinkIdA;
      Length = LengthA;
      Bandwidth = BandwidthA;
      Velocity = VelocityA;
      NoisePower = NoisePowerA;
    }
    else
    printf("The AWS received <0> matches from Backend-Server <A> using UDP over port <%s> \n", PORTA);
    if(resultB == linkId){
      printf("The AWS received <1> matches from Backend-Server <B> using UDP over port <%s> \n", PORTB);
      LinkId = LinkIdB;
      Length = LengthB;
      Bandwidth = BandwidthB;
      Velocity = VelocityB;
      NoisePower = NoisePowerB;
    }
    else
    printf("The AWS received <0> matches from Backend-Server <B> using UDP over port <%s> \n", PORTB);

    // if(resultA != linkId){
    //   resultA = getDataB(linkId);
    //   if(resultA == linkId)
    //     printf("The AWS received <1> matches from Backend-Server <B> using UDP over port <%s> \n", PORTB);
    //   else
    //   printf("The AWS received <0> matches from Backend-Server <B> using UDP over port <%s> \n", PORTB);
    // }
		//calculate the final result
    float result;
    float finalResult = -1;
    if(LinkId == linkId){
		  result = finalResult;
      finalResult = compute();
      }
    else
      finalResult = -1;

    send(new_fd_M, (const float *)&finalResult, sizeof(finalResult), 0);
    send(new_fd_M, (const float *)&TTrans, sizeof(TTrans), 0);
    send(new_fd_M, (const float *)&TProp, sizeof(TProp), 0);
		send(new_fd, (const float *)&finalResult, sizeof(finalResult), 0);
    if(finalResult!=-1){
      printf("The AWS sent delay = <%.2f> ms to the client using TCP over port <%s>\n",finalResult,TCPPORT);
      printf("The AWS sent detailed results to the monitor using TCP over port <%s> \n",MONITORPORT);
}else{
        printf("The AWS sent No Match to the monitor and the client using TCP over ports <%s> and <%s>, repsectively\n",TCPPORT,MONITORPORT);
    }
    printf("\n \n \n");
    // finalResult = -1;
    close(new_fd);
    // close(new_fd_M);
	}



}
