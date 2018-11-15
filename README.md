# UnixSocketProgramming
** Kavish Jadwani **
** USC ID : 6917934471 **
This project implements a model computational offloading where a single client issues three parameters (link ID, file/packet size, and signal power) to the AWS server and expects the reply for the end-to-end delay of the designated link.

## Description
The server communicating with the client acts similar to AWS (Amazon Web Server). There are three backend servers, named Back-Server A, Back-Server B and Back-Server C. Back-Servers A and B are storage servers. Each of them possesses a database file, database_a.csv and database_b.csv, respectively, in which attribute values regarding information of links are stored.    
The Back-Server C is a computing server. It receives data from the AWS server, performs computational tasks, and returns the results to the AWS server. The monitor connecting to the AWS server is used to record results of every steps and print them out. The client, monitor and the AWS communicate over TCP connections while the AWS and the Back-Servers A, B, & C communicate over UDP connections.

## What I have done. Phases of Project   
### Phase 1
	- All four server programs (AWS -a.k.a. server-D, Back-Server A, B, & C) boot up, listening at specific port for incoming packets/connections and display boot up message.  
  - The client takes an input argument from the command line that specifies the Link Id, Size and Power for the computation, where size is in bits and power in dBm.
  -Client sends the input to AWS server over TCP  
	###Phase 2
  ####Phase 2A  
  - The AWS server receives the input from the client and displays a message to indicate this
  - The AWS server sends the information received by the client to the monitor over TCP Connection and displays the message to indicate this
  - The monitor will receive the information received from client and will display the message to indicate  this
	- The AWS sends Link Id to Backend Server A and Backend Server B over UDP and display the message to indicate this
	- Each server (A and B) will search for the Link ID separately and send results back to AWS server over UDP, displaying their operation states
  - The AWS will also print the number of results obtained from each of the Server A and Server B, 0 indicating that the link id was not found on the respective server  and 1 representing that the link id was found on the server
  ####Phase 2B
  - Once the AWS server receives replies from both storage servers, the AWS server check the replied messages and if both the messages indicate that the link id was not found of the storage servers, the AWS sends out a result indicating "No Match Found" and this message is displayed on both client and monitor
  - If Link Id and other information is found on any of the storage servers, the AWS sends all the information to Backend Server C ie. the computing server to calculate the delay over UDP connection
  - The backend server C does the computation
  - Both AWS and Server C prints messages to indicate the communication of information and calculation of delay
	###Phase 3  
	 - AWS combines returning result from three back-end servers and send the combined result to monitor and client.  
	4. Phase 3 (**extra points**)  
	- At the end of phase 2B, backend server C will have the results ready and those results are sent to AWS over UDP
  - When AWS receives the computation result, it forwards the end-to-end delay to the client and the client displays the end to end result. This is done over TCP Connection
  - The AWS also sends final results to the monitor over TCP and monitor displays all the final results

## Code files  
#### aws.c  
	Receive input from client over TCP, send the input to two backend storage servers (A and B)over UDP. IF Link Id available on any of the storage server, AWS will send all information to Server C for Computation. After receving final values from Server C, AWS sends results to client and monitor.
  After booting, the aws server can only be shut down by crtl+c command.  
#### client.c
	Send input to aws over TCP. Receive feedback from aws and show on screen. The client will terminate itself after receiving feedback from aws.  
#### monitor.c
  Monitor will boot before client and will display the results sent by AWS.
	After booting, the monitor can only be shut down by crtl+c command.   
#### serverA.c
	Receive link id from aws over UDP, do the searching in database A, send back result to aws.
  After booting, the server A can only be shut down by crtl+c command.  
#### serverB.c
  Receive link id from aws over UDP, do the searching in database B, send back result to aws.
  After booting, the server B can only be shut down by crtl+c command.  
#### serverC.c
	Receive input and other information from aws over UDP, do the computation for propagation delay, transmission delay and end to end delay and send back results to aws.  

### How to run
	1. Put all files and two database txt files in one directory. Open six terminal in the directory, run each program in the following order on different terminals
  2. Compile all fiels and create executables using the command : $make all
	3. run Server A by using the command :$make serverA
  4. run Server B by using the command :$make serverB
  4. run Server C by using the command :$make serverC
  6. run AWS server by using the command : $make aws
  7. run Monitor by using the command : $make monitor
	8. Run client using : $ ./client <LINK_ID> <SIZE> <POWER>
	6. The client.c will terminate by itself, other programs need to use <crtl+c> to terminate.

### Messages exchange format example  
#### Backend-Server A Terminal
The Server A is up and running using UDP on port <21471>.
The Server A received input <118>  
The server A has found <1> match
The Server A finished sending the output to AWS  
#### Backend-Server B Terminal
The Server B is up and running using UDP on port <22471>.
The Server B received input <118>  
The server B has found <0> match
The Server B finished sending the output to AWS
#### Backend-Server C Terminal
The Server C is up and running using UDP on port <23471>.
The Server C received link information of link <118>, file size <10000>, and signal power <-30>  
The Server C finished the calculation for link <118>
The Server C finished sending the output to AWS  
#### AWS Terminal
The AWS is up and running.
The AWS received link ID= <118>, size= <10000> and power= <-30> from the client using TCP over port <25471>
The AWS sent link ID= <118>, size= <10000> and power= <-30> to the monitor using TCP over port <26471>
The AWS sent link ID= <118> to Backend-Server <A> using UDP over port <21471>
The AWS sent link ID= <118> to Backend-Server <B> using UDP over port <22471>
The AWS received <1> matches from Backend-Server <A> using UDP over port <21471>
The AWS received <0> matches from Backend-Server <B> using UDP over port <22471>
The AWS sent link ID= <118>, size = <10000>, power = <-30> and link information to Backend-Server C using UDP over port <23471>
The AWS received outputs from Backend-Server C using UDP over port <23471>
The AWS sent delay = <0.01> ms to the client using TCP over port <25471>
The AWS sent detailed results to the monitor using TCP over port <26471>
The AWS sent No Match to the monitor and the client using TCP over ports <25471> and <26471>, repsectively
#### Client Terminal
The client is up and running.
The client sent ID= <118>, size= <10000> and power = <-30> to AWS
The delay for link <118> is <0.01> ms
Found no matches for link <555>
#### Monitor Terminal
The monitor is up and running.
The  monitor received link ID = <118>, size = <10000>, and power = <-30> from the AWS
The result for link  <118> :
Tt = <0.012479> ms,
Tp = <0.002509> ms,
Delay = <0.01> ms
Found no matches for link <555>

### Reused Code  
	My TCP and UDP setting up code is based on examples in Beej's book.  
