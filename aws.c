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
int LinkId = 0;
int Size = 0;
int Power = 0;
float Bandwidth = 0;
float Length = 0;
float Velocity = 0 ;
float NoisePower = 0;
float TTrans = 0;
float TProp = 0;

void sigchld_handler(int s){
    // waitpid() might overwrite errno, so we save and restore it:
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
  //printf("port number: %s \n", backserver_port);
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
  // recvfrom(mysock, (float *)& Bandwidth, sizeof Bandwidth, 0 , NULL, NULL);
  // recvfrom(mysock, (float *)& Length, sizeof Length, 0 , NULL, NULL);
  // recvfrom(mysock, (float *)& Velocity, sizeof Velocity, 0 , NULL, NULL);
  // recvfrom(mysock, (float *)& NoisePower, sizeof NoisePower, 0 , NULL, NULL);
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
  //printf("port number: %s \n", backserver_port);
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
  printf("The AWS sent link ID= <%d> to Backend-Server B using UDP over port <%s>\n", linkid,PORTB);
	recvfrom(mysock, (float *)& LinkId, sizeof LinkId, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Bandwidth, sizeof Bandwidth, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Length, sizeof Length, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Velocity, sizeof Velocity, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& NoisePower, sizeof NoisePower, 0 , NULL, NULL);
	return LinkId;
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
  //printf("port number: %s \n", backserver_port);
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
  printf("The AWS sent link ID= <%d> to Backend-Server A using UDP over port <%s>\n", linkid,PORTA);
	recvfrom(mysock, (int *)& LinkId, sizeof LinkId, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Bandwidth, sizeof Bandwidth, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Length, sizeof Length, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& Velocity, sizeof Velocity, 0 , NULL, NULL);
  recvfrom(mysock, (float *)& NoisePower, sizeof NoisePower, 0 , NULL, NULL);
	//printf("the result is %d \n", result);
	return LinkId;
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

  //keep accepting monitor requiring
      /*sin_size = sizeof their_addr;
      new_fd_M = accept(sockfd_M, (struct sockaddr *) &their_addr, &sin_size);
      if (new_fd_M == -1) {
                  perror("accept");
                  exit(1);
      }*/
      int monitorOn=0;
      while (monitorOn == 0){
        // child socket connect with monitor
        sin_size = sizeof their_addr;
        new_fd_M = accept(sockfd_M, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd_M == -1) {
          perror("accept");
          continue;
        }
        //printf("debug:monitorOn=%d\n",monitorOn);
        // inet_ntop(their_addr.ss_family,
        //   get_in_addr((struct sockaddr *)&their_addr),
        //   s, sizeof s);
        //printf("server: got connection from %s\n", s);
        if (!fork()) { // this is the child process
          close(sockfd_M); // child doesn't need the listener
          monitorOn=1;
          // printf("debug:monitorOn=%d\n",monitorOn);
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
		// get the port of client
		// inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
		// struct sockaddr_in addrTheir;
		// memset(&addrTheir, 0, sizeof(addrTheir));
		// int len = sizeof(addrTheir);
		// getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
		// int client_port = addrTheir.sin_port;
		// //receive all the information from client
		// char function_name[3];
		int total_num;
    int linkId;
    recv(new_fd, (int *)&linkId, sizeof linkId, 0);
    recv(new_fd, (int *)&Size, sizeof Size, 0);
    recv(new_fd, (int *)&Power, sizeof Power, 0);
    send(new_fd_M, (const int *)&linkId, sizeof linkId , 0);
    // printf("******************************\n");
    // printf("The AWS sent link id %d to monitor\n", linkId);
    // printf("******************************\n");
    send(new_fd_M, (const int *)&Size, sizeof Size , 0);
    send(new_fd_M, (const int *)&Power, sizeof Power , 0);
    printf("The AWS received link ID= <%d>, size= <%d> and power= <%d> from the client using TCP over port  \n",linkId,Size,Power);
		int resultA = getDataA(linkId);
    if(resultA == linkId)
      printf("The AWS received <1> matches from Backend-Server A using UDP over port <%s> \n", PORTA);
    else
    printf("The AWS received <0> matches from Backend-Server A using UDP over port <%s> \n", PORTA);
    if(resultA != linkId){
      resultA = getDataB(linkId);
      if(resultA == linkId)
        printf("The AWS received <1> matches from Backend-Server B using UDP over port <%s> \n", PORTB);
      else
      printf("The AWS received <0> matches from Backend-Server B using UDP over port <%s> \n", PORTB);
    }

		//calculate the final result
    float result;
    float finalResult = -1;
    if(resultA == linkId){
		  result = finalResult;
      finalResult = compute();
      }
    else
      finalResult = -1;

    send(new_fd_M, (const float *)&finalResult, sizeof(finalResult), 0);
    send(new_fd_M, (const float *)&TTrans, sizeof(TTrans), 0);
    send(new_fd_M, (const float *)&TProp, sizeof(TProp), 0);
		send(new_fd, (const float *)&finalResult, sizeof(finalResult), 0);

    // printf("The final result is %f\n", result);
    if(finalResult!=-1)
      printf("The AWS sent delay = <%f> ms to the client using TCP over port <%s>\n",finalResult,TCPPORT);
    else{
        printf("The AWS sent No Match to the monitor and the client using TCP over ports <%s> and <%s>, repsectively\n",TCPPORT,MONITORPORT);
    }
    // finalResult = -1;
    close(new_fd);
    // close(new_fd_M);
	}//Close while loop for client
// }// Close While loop for monitor


}
