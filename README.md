####Kavish Jadwani
####USC ID : 6917934471
# UnixSocketProgramming
This project implements a model computational offloading where a single client issues three parameters (link ID, file/packet size, and signal power) to the AWS server and expects the reply for the end-to-end delay of the designated link.

## Description
The server communicating with the client acts similar to AWS (Amazon Web Server). There are three backend servers, named Back-Server A, Back-Server B and Back-Server C. Back-Servers A and B are storage servers. Each of them possesses a database file, database_a.csv and database_b.csv, respectively, in which attribute values regarding information of links are stored.    
The Back-Server C is a computing server. It receives data from the AWS server, performs computational tasks, and returns the results to the AWS server. The monitor connecting to the AWS server is used to record results of every steps and print them out. The client, monitor and the AWS communicate over TCP connections while the AWS and the Back-Servers A, B, & C communicate over UDP connections.
##Phases of Project   
###Phase 1
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
####aws.c  
	Receive input from client over TCP, send the input to two backend storage servers (A and B)over UDP. IF Link Id available on any of the storage server, AWS will send all information to Server C for Computation. After receving final values from Server C, AWS sends results to client and monitor.
  After booting, the aws server can only be shut down by crtl+c command.  
####client.c
	Send input to aws over TCP. Receive feedback from aws and show on screen. The client will terminate itself after receiving feedback from aws.  
####monitor.c
  Monitor will boot before client and will display the results sent by AWS.
	After booting, the monitor can only be shut down by crtl+c command.   
####serverA.c
	Receive link id from aws over UDP, do the searching in database A, send back result to aws.
  After booting, the server A can only be shut down by crtl+c command.  
####serverB.c
  Receive link id from aws over UDP, do the searching in database B, send back result to aws.
  After booting, the server B can only be shut down by crtl+c command.  
####serverC.c
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
	1. Execute search hack  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<hack> and function=<search> from the client using TCP over port 25217  
	Sent <search> and <hack> to Backend-Server A  
	Sent <search> and <hack> to Backend-Server B  
	Sent <search> and <hack> to Backend-Server C  
	The AWS received <1> similar words from Backend-Server <A> using UDP over port <21217>  
	The AWS received <1> similar words from Backend-Server <B> using UDP over port <22217>  
	The AWS received <1> similar words from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <1> matches to client  
	The AWS sent <hack> and <Jack> to client to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <hack> and <search> to AWS.  
	Found a match for < hack >:  
	< Hackneyed; hired; mercenary. >  
	- monitor console  
	The monitor is up and running.  
	Found a match for < hack >:  
	< Hackneyed; hired; mercenary. >  
	One edit distance match is <Jack>:  
	<"To wash on a rack  as metals or ore.">  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <search> and operation <hack>  
	The Server A has found < 0 > matches and < 1 > similar words  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <search> and operation <hack>  
	The Server B has found < 1 > matches and < 1 > similar words  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <search> and operation <hack>  
	The Server C has found < 1 > matches and < 1 > similar words  
	The Server C finished sending the output to AWS  

	2. Execute <prefix> <accuse>  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<accus> and function=<prefix> from the client using TCP over port 25217  
	Sent <prefix> and <accus> to Backend-Server A  
	Sent <prefix> and <accus> to Backend-Server B  
	Sent <prefix> and <accus> to Backend-Server C  
	The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  
	The AWS received <3> matches from Backend-Server <B> using UDP over port <22217>  
	The AWS received <3> matches from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <7> matches to client  
	The AWS sent <7> matches to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <accus> and <prefix> to AWS.  
	Found <7> matches for <accus>:  
	<Accustomed>  
	<Accuser>  
	<Accuse>  
	<Accusatorially>  
	<Accustom>  
	<Accuse>  
	<Accusement>  
	- monitor console  
	The monitor is up and running.  
	Found <7> matches for <accus>:  
	<Accustomed>  
	<Accuser>  
	<Accuse>  
	<Accusatorially>  
	<Accustom>  
	<Accuse>  
	<Accusement>  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <prefix> and operation <accus>  
	The Server A has found < 1 > matches  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <prefix> and operation <accus>  
	The Server B has found < 3 > matches  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <prefix> and operation <accus>  
	The Server C has found < 3 > matches  
	The Server C finished sending the output to AWS  

	3. Execute <suffix> <ntable>  
	- aws console  
	The AWS is up and running.  
	The AWS received input=<ntable> and function=<suffix> from the client using TCP over port 25217  
	Sent <suffix> and <ntable> to Backend-Server A  
	Sent <suffix> and <ntable> to Backend-Server B  
	Sent <suffix> and <ntable> to Backend-Server C  
	The AWS received <1> matches from Backend-Server <A> using UDP over port <21217>  
	The AWS received <1> matches from Backend-Server <B> using UDP over port <22217>  
	The AWS received <2> matches from Backend-Server <C> using UDP over port <23217>  
	The AWS sent <4> matches to client  
	The AWS sent <4> matches to the monitor via TCP port <26217>  
	- client console  
	The client is up and running.”  
	The client sent <ntable> and <suffix> to AWS.  
	Found <4> matches for <ntable>:  
	<Replantable>  
	<Acquaintable>  
	<Accountable>  
	<Fermentable>  
	- monitor console  
	The monitor is up and running.  
	Found <4> matches for <ntable>:  
	<Replantable>  
	<Acquaintable>  
	<Accountable>  
	<Fermentable>  
	- servera console  
	The Server A is up and running using UDP on port <21217>.  
	The Server A received input <suffix> and operation <ntable>  
	The Server A has found < 1 > matches  
	The Server A finished sending the output to AWS  
	- serverb console  
	The Server B is up and running using UDP on port <22217>.  
	The Server B received input <suffix> and operation <ntable>  
	The Server B has found < 1 > matches  
	The Server B finished sending the output to AWS  
	- serverc console  
	The Server C is up and running using UDP on port <23217>.  
	The Server C received input <suffix> and operation <ntable>  
	The Server C has found < 2 > matches  
	The Server C finished sending the output to AWS  


### Reused Code  
	My TCP and UDP setting up code is based on examples in Beej's book.  
