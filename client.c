//client.c
//Paul Kyser
// CSCI 632
// project 2

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define SERVER_PORT 5432
#define MAX_LINE 256
#define MAXNAME 121



void *chat_listener(sockid)
{

	int s,hp;
	struct packet
	{
		short type;
		char data[MAXNAME];
		int sockid;
		int group;
	};
	
	struct packet packet_reg;
	s = sockid;

	printf("chat_listener\n");

	while(1){

		packet_reg.type = htons(231);
		//strcpy(packet_reg.data,hp); 

	 	// keep waiting for broadcast from the server
	 	
		if(recv(s,&packet_reg,sizeof(packet_reg),0) < 0)
	 	{
	 		printf("\n Could not receive data packet \n");
	 		exit(1);
	 	}
	 	printf("\n\nSendersockID: %d\n message: %s\n", packet_reg.sockid, packet_reg.data);
		fflush(stdout); //force send output from buffer to file

 
	}

}


int main(int argc, char* argv[])
{
	
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s, sockid;
	int group;
	pthread_t threads[1];

	/* structure of the packet */
	struct packet
	{
		short type;
		char data[MAXNAME];
		int sockid;
		int group;
	};
	struct packet packet_reg;

	

	if(argc == 3){
		host = argv[1];
		long x = strtol(argv[2], NULL, 10);
		group = x;
		//printf("%d group%d  %d\n", argv[0],group,argv[2]);
		//printf("%d\n", inlen);
	}
	
	else{
		fprintf(stderr, "usage:newclient server\n");
		exit(1);
	}


	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if(!hp){
		fprintf(stderr, "unkown host: %s\n", host);
		exit(1);
	}

	/* active open */
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("tcpclient: socket");
		exit(1);
	}

	/* build address data structure */
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	
	if(connect(s,(struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("tcpclient: connect");
		close(s);
		exit(1);
	}
	printf("Connected sendint packet\n");

	/* Constructing the registration packet at client */
 	packet_reg.type = htons(121);

 	printf("%d--client--%d\n", packet_reg.type,htons(121) );
 

 	strcpy(packet_reg.data,hp); 
 	packet_reg.group = group;

 	printf("packet 1 created, ready to send\n");
	
	
	/* Send the registration packet to the server */
 	if(send(s,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Send failed\n");
 		exit(1);
	}

	printf("1st conformation packet sent\n");

	//create and send 2nd registration packet
	packet_reg.type = htons(121);
 

 	strcpy(packet_reg.data,hp); 

 	printf("packet 2 created, ready to send\n");
	if(send(s,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Send failed\n");
 		exit(1);
	}


	printf("2nd conformation packet sent\n");

	//create and send 3rd registration packet
	packet_reg.type = htons(121);
 

 	strcpy(packet_reg.data,hp); 

 	printf("packet 3 created, ready to send\n");

	if(send(s,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Send failed\n");
 		exit(1);
	}

	printf("3rd conformation packet sent\n");
	

	//Recieve Conformation packet

	if(recv(s,&packet_reg,sizeof(packet_reg),0) < 0)
 	{
 		printf("\n Could not receive conformation packet \n");
 		exit(1);
 	}


	printf("Conformation packet recieved\n");


	//start chat listener
	pthread_create(&threads[0], NULL, chat_listener, s);

	//get user input and send it to chat server
	packet_reg.type = htons(231);
 

 	//strcpy(packet_reg.data,hp); 
 	packet_reg.sockid = sockid;

 	while(fgets(buf, sizeof(buf), stdin)){
		buf[MAX_LINE-1] = '\0';
		strcpy(packet_reg.data,buf); 
		packet_reg.group = group;
		send(s, &packet_reg,sizeof(packet_reg),0 );
		sleep(5);
	}


 }	 	


	// thread to listen for broadcast from server

 

