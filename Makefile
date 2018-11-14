####### Make file
# Kavish Jadwani
# ID: 6917934471
# Use <make> to compile
####### How to run:
# Open six terminal, run each program in the following order on different terminal
# 1. run three servers in any order use <make server#>
# 2. run aws use <make aws>
# 3. run monitor <make monitor>
# 4. run client use <./client [function (choose one from search, suffix or prefix)] word>
# 5. client.c will terminate by itself, other programs need to use <crtl+c> to terminate.
#######

all:
	gcc -o awsoutput aws.c
	gcc -o client client.c
	# gcc -o listener listener.c
	# gcc -o talker talker.c
	gcc -o serverAoutput serverA.c
	gcc -o serverBoutput serverB.c
	gcc -o serverCoutput serverC.c -lm
	gcc -o monitoroutput monitor.c

.PHONY: aws
aws:
	./awsoutput

.PHONY:serverA
serverA:
	./serverAoutput

.PHONY:serverB
serverB:
	./serverBoutput

.PHONY:serverC
serverC:
	./serverCoutput

.PHONY: monitor
monitor:
	./monitoroutput
