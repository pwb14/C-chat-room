//server.c
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
#include <fcntl.h>
#include <unistd.h>

#define SERVER_PORT 5432
#define MAX_LINE 256
#define MAX_PENDING 5
#define MAXNAME 121

void *join_handler();
void *multicaster();
void *chat_handler();

/* structure of Registration Table */
struct global_table
{
	int sockid;
	int req_no;
	int group;
};
struct message_array
{
	int sender_sockid;
	char data[MAXNAME];
	int group;

};

struct message_array messages[100];
int message_index = 0;
int messages_sent = 0;
struct global_table record[20];
int table_index;

// declare a glbal mutex

pthread_mutex_t my_mutex = PTHREAD_MUTEX_INITIALIZER;



int main(int argc, char* argv[])
{
	
	
	
	struct sockaddr_in sin;
	struct sockaddr_in clientAddr;
	struct hostent *hp;
	//struct packet *ptr;
	pthread_t threads[3];
	int s, new_s;
	int len,type;

	struct packet
	{
		short type;
		char data[MAXNAME];
		int group;
	};
	struct packet packet_reg;

	//create multicaster thread
	    if( pthread_create( &threads[1] , NULL ,  multicaster , NULL) < 0)
        {
            perror("could not create thread");
            exit(1);
        }

	/* structure of Registration Table */
/*	struct global_table
	{
		int sockid;
		int req_no;
	};

	struct global_table record[20];*/
	//struct global_table client_info[10];

	/* translate host name into peer's IP address */
	
    char hostname[1024];
    gethostname(hostname, 1024);
	hp = gethostbyname(hostname);
    


	/* setup passive open */
	if((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		perror("tcpserver: socket");
		exit(1);
	}

	/* build address data structure */
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);


	if(bind(s,(struct sockaddr *)&sin, sizeof(sin)) < 0){
		perror("tcpclient: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);

	printf("Listening\n");

	/* wait for connection, then receive and print text */
	

	while((new_s = accept(s, (struct sockaddr *)&clientAddr, &len)) > 0)
	
	{
		printf("accepted\n");
		struct global_table client_info;
		
		// recieve the first packet
		printf("%d\n", new_s);

		if(recv(new_s,&packet_reg,sizeof(packet_reg),0) < 0)
	 	{
	 		printf("\n Could not receive first registration packet \n");
	 		exit(1);
	 	};
	 	printf(" Client's socketid is %d \n", new_s );

	 	
	 	//struct packet *ptr = &packet_reg;
	 	printf("%d----%d----%d\n", packet_reg.type,htons(121),htons(321));

	 	if(packet_reg.type  == htons(121))
	 	{

		 	client_info.sockid = new_s;
			client_info.req_no = 1;

			//creat a join handler thread to handle the rest of the join
			pthread_create(&threads[0], NULL, join_handler, &client_info);
			
			
			printf("join_handler assigned\n");
			pthread_join(threads[0], NULL);

			//create chat handler thread
			pthread_create(&threads[0], NULL, chat_handler, &client_info);

			fflush(stdout); //force send output from buffer to file
		}
		/*printf("Got past first if------------------\n");
		if(packet_reg.type == htons(321))
		{
			client_info.sockid = new_s;

			pthread_create(&threads[2], NULL, leave_handler, &client_info);
			printf("Leave_handler assigned\n");
			pthread_join(threads[2], NULL);
			fflush(stdout); //force send output from buffer to file
		}*/
	}

	
     
}  

void *chat_handler( struct global_table *rec)
{
	int oldsock;
	int new_s,not_leave;
	struct packet
	{
		short type;
		char data[MAXNAME];
		int sender_sockid;
		int group;
	};

	struct packet packet_reg;
	printf("In chat handler thread\n");
	new_s = rec -> sockid;
	
	while(1)
	{


		printf("waiting on message packet\n");
		if(recv(new_s,&packet_reg,sizeof(packet_reg),0) < 0)
		 	{
		 		printf("\n Could not receive message packet \n");
		 		exit(1);
		 	};
		 printf("DATA:  %s", packet_reg.data);
	 	// printf("%d %d %d %d %d %sdata %d group\n", new_s, packet_reg.sender_sockid, packet_reg.type,htons(231),packet_reg.type == htons(231), packet_reg.data, packet_reg.group );
		/*if(packet_reg.type == htons(231))
		{
*/
			pthread_mutex_lock(&my_mutex);
			
			messages[message_index].sender_sockid = packet_reg.sender_sockid;
			strcpy(messages[message_index].data, packet_reg.data);
			messages[message_index].group = packet_reg.group;

			message_index ++;

			pthread_mutex_unlock(&my_mutex);
			printf("message recieved group%d :%s\n", packet_reg.group, packet_reg.data);
		//}
		//printf("-------------while loop-----------%d\n", not_leave);

	}
	
	


} 
//handles the rest of the client connection conformation. This include sending an acknolegment and adding
//the client to the global table

void *join_handler( struct global_table *rec)
{
	printf("You are in the join join_handler\n");
	int newsock,group;
	pthread_t threads[1];
	struct packet
	{
		short type;
		char data[MAXNAME];
		int sockid;
		int group;
	};

	struct packet packet_reg;
	newsock = rec -> sockid;


	//wait for the the second packet
	if(recv(newsock,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Could not recieve\n");
		exit(1);
	}

	printf("Recieved 2nd registration packet\n");

	//wait for one more packet from client
	if(recv(newsock,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Could not recieve\n");
		exit(1);
	}
	printf("Recieved 3rd registration packet\n Sending Ack\n");

	//send acknowledgement
	struct hostent *hp;
	char hostname[1024];
    gethostname(hostname, 1024);
	hp = gethostbyname(hostname);
	packet_reg.type = htons(221);
 	strcpy(packet_reg.data,hp); 
 	packet_reg.sockid=newsock;


 	printf("Conformation packet created, ready to send\n");

 	if(send(newsock,&packet_reg,sizeof(packet_reg),0) <0)
	{
		printf("\n Send failed\n");
 		exit(1);
	}

	printf("updating table\n");
	//update table

	pthread_mutex_lock(&my_mutex);
	record[table_index].sockid = newsock;
	record[table_index].req_no = 3;
	record[table_index].group = packet_reg.group;

	table_index++;

	pthread_mutex_unlock(&my_mutex);

	//leave the thread
	printf("leaving thread\n");

	//launch leave handler to wait for leave request
	//pthread_create(&threads[1], NULL, leave_handler, rec);
	//pthread_join(threads[1],NULL);
	

	fflush(stdout); //force send output from buffer to file

	pthread_exit(NULL);
}

// Waits for at least 1 client to connect and then sends a broadcast out to every connected client listed in the 
// global table.
void *multicaster(){

	printf("multicast started\n");
	char *filename;
	char text[100];
	int fd,sock,i,is_empty,group;
	



	struct datapacket
	{
		short type;
		char data[MAXNAME];
		int sender_sockid;
		int group;

	};
	struct datapacket filedata;


	//filename = "multicasterinput.txt";
	//fd = open(filename, O_RDONLY, 0);

	
	
	while(1)
	{
		
		while(!isEmpty())
		{
			printf("no new messages. Recieved: %d Sent: %d. waiting 5 seconds\n\n", message_index,messages_sent);
	/*		for(i = 0; i < 20 ; i++)
			{

				sock = record[i].sockid;
				if(sock != 0)
				{
					is_empty =0;
					
			 	}*/
				//printf("filedata packet sent\n");
		   

		  	 
			
			sleep(5);
			fflush(stdout); //force send output from buffer to file
			
		}
		
		
		//broad cast start when someone connects
		printf("going to sent?\n");
		
		
		while(isEmpty())
		{
			printf("-----------------------------------------------\n");
			printf("\nsending broadcast for group %d:\n", messages[messages_sent].group);

			//if at least on client is listed, read 100 bytes of data from the file and sotr it in text

			//read( fd, text, 100);
			

			//construct datapacket

			filedata.type = htons(231);
			pthread_mutex_lock(&my_mutex);
		 	strcpy(filedata.data, messages[messages_sent].data); 
		 	//filedata.type = broadcast_num;
		 	filedata.group = messages[messages_sent].group;
		 	filedata.sender_sockid = messages[messages_sent].sender_sockid;

		 	pthread_mutex_unlock(&my_mutex);
			printf("message:%s\n",filedata.data);

		 

			//send data packet
			for(i = 0; i < 20 ; i++)
			{
				pthread_mutex_lock(&my_mutex);
				sock = record[i].sockid;
				group = record[i].group;
				pthread_mutex_unlock(&my_mutex);

				//if in the group in the messege send
				printf("group%d filedata.group%d\n", group, filedata.group );
				if(group == filedata.group)
				{
					if(send(sock,&filedata,sizeof(filedata),0) > -1)
					{
						printf("\n Data send to connection %d\n", i);
						
				 	}
			 	}
				//printf("filedata packet sent\n");
		   

		  	 
			}
			printf("broadcast sent to group %d : %s\n\n", filedata.group,filedata.data);
			printf("-----------------------------------------------\n");
			fflush(stdout); //force send output from buffer to file
			messages_sent++;
			//wait 5 seconds between broadcast
			sleep(5);
		}
	}
}

int isEmpty()
{

	 return (message_index > messages_sent);
}
