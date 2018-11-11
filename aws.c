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
double TTrans = 0;
double TProp = 0;

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
void setMonitor(){

   int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(HOST, MONITORPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return ;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
			perror("setsockopt");
			exit(1);
		}
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("monitor: bind");
			continue;
		}
		break;
	}
	if(p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return ;
	}
	freeaddrinfo(servinfo); // all done with this structure
	//listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
  printf("The connection with monitor is already set up\n");
	while(1){
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
    printf("The new fd is \n");
		if (new_fd == -1) {
			perror("accept");
			exit(1);
		}
      printf("The new fd is \n");
		// get the port of client
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
		struct sockaddr_in addrTheir;
		memset(&addrTheir, 0, sizeof(addrTheir));
		int len = sizeof(addrTheir);
		getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
		int client_port = addrTheir.sin_port;
		//receive all the information from client
		float finalResult = 541994;
    int linkId;

		send(new_fd, (const float *)&finalResult, sizeof(finalResult), 0);
    // float finalResult = 0;
    printf("******************************************************************\n");
    printf("The value is sent to montior and the value is %f \n",finalResult);
    printf("******************************************************************\n");
    close(new_fd);
	}
}
int main(){
	//set up TCP --from Beej
	int sockfd, new_fd,sockfd_M, new_fd_M;;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(HOST, TCPPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
	}
	// loop through all the results and bind to the first we can
	for (p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
				== -1) {
			perror("server: socket");
			continue;
		}
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
				== -1) {
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
	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		return 2;
	}
	freeaddrinfo(servinfo); // all done with this structure
	//listen
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
   //Create a parent socket with monitor
  if ((rv = getaddrinfo(HOST, MONITORPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd_M = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            == -1) {
            perror("server: socket");
            continue;
        }
        if (setsockopt(sockfd_M, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))
            == -1) {
            perror("setsockopt");
            exit(1);
        }
        if (bind(sockfd_M, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd_M);
            perror("server: bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        return 2;
    }
    freeaddrinfo(servinfo); // all done with this structure

    if (listen(sockfd_M, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
	printf( "The AWS is up and running. \n");

  //keep accepting monitor requiring
      sin_size = sizeof their_addr;
      new_fd_M = accept(sockfd_M, (struct sockaddr *) &their_addr, &sin_size);
      if (new_fd_M == -1) {
                  perror("accept");
                  exit(1);
      }


	//keep handling client query
	while(1){
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			exit(1);
		}
		// get the port of client
		inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *) &their_addr), s, sizeof s);
		struct sockaddr_in addrTheir;
		memset(&addrTheir, 0, sizeof(addrTheir));
		int len = sizeof(addrTheir);
		getpeername(new_fd, (struct sockaddr *) &addrTheir, (socklen_t *) &len);
		int client_port = addrTheir.sin_port;
		//receive all the information from client
		char function_name[3];
		int total_num;
    int linkId;
    recv(new_fd, (int *)&linkId, sizeof linkId, 0);
    recv(new_fd, (int *)&Size, sizeof Size, 0);
    recv(new_fd, (int *)&Power, sizeof Power, 0);
    send(new_fd_M, (const int *)&linkId, sizeof(linkId), 0);
    send(new_fd_M, (const int *)&Size, sizeof(Size), 0);
    send(new_fd_M, (const int *)&Power, sizeof(Power), 0);
    printf("The AWS received link ID= <%d>, size= <%d> and power= <%d> from the client using TCP over port <%d> \n",linkId,Size,Power,client_port);
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
    int result;
    float finalResult;
    if(resultA == linkId){
		  result = resultA;
      finalResult = compute();
      }
    else
      finalResult = -1;

    send(new_fd_M, (const float *)&finalResult, sizeof(finalResult), 0);
    send(new_fd_M, (const float *)&TTrans, sizeof(TTrans), 0);
    send(new_fd_M, (const float *)&TProp, sizeof(TProp), 0);
		send(new_fd, (const float *)&finalResult, sizeof(finalResult), 0);

    printf("The final result is %f\n", finalResult);
    if(finalResult!=-1)
      printf("The AWS sent delay = <%f> ms to the client using TCP over port <%s>\n",finalResult,TCPPORT);
    else{
        printf("The AWS sent No Match to the monitor and the client using TCP over ports <%s> and <%s>, repsectively\n",TCPPORT,TCPPORT);
    }
    finalResult = -1;
    close(new_fd);
    close(new_fd_M);
	}

}
