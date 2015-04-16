/* Modified simple server from LHT using TCP
   Port # is passed as an argument*/

/*The way this works is that the server starts running on a port#
and accepts the mesages from the client
The first message will be the username
The second message will be the password
Then the server will authenticate this user
and retrieve their document ids from the database
and return these ids to the client */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>

void dostuff(int, int); /* function prototype */
bool auth( char, char); /*function prototype */
int getDocID(char); /*function prototype* /

/*will be called if the system call fails.
Displays an error message from stderr*/
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/* main function handles the server*/

int main(int argc, char *argv[])
{
     //sockfd and newsockfd are file descriptors
     //they store the values returned by the socket system call
     int sockfd, newsockfd, portno, pid; 
     socklen_t clilen; 
     
	//a struct containing the internet address
     struct sockaddr_in serv_addr, cli_addr; 

     //if the user does not provide a port # give error:
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     //creating a new socket
     //AF_INET is the unix domain for two processes that share a common file system
     //SOCK_STREAM is the internet domain for any two hosts on the Internet
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     //if a socket call fails it returns -1
     if (sockfd < 0) 
        error("ERROR opening socket");
     //bzero sets all values in a buffer to zero
     //takes a pointer to the buffer, and the size of the buffer
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]); 

     //adding the fields of the sevr_addr struct
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_port = htons(portno); //htons converts it to nertwork byte order
     serv_addr.sin_addr.s_addr = INADDR_ANY;

     //bind() binds a socket to an address
     //takes the current addr of the current hose + the port #
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");

     //liste system call to listen for connections
     listen(sockfd,5);
     //accept wakes up the process on successful connection
     clilen = sizeof(cli_addr);

	 //we want to be able to have multiple connections established
	 while(1)
	 {
		 newsockfd = accept(sockfd, 
		             (struct sockaddr *) &cli_addr, 
		             &clilen);
		 //we make sure the new connection fire descriptor is not -1 (error)
		 if (newsockfd < 0) 
		      error("ERROR on accept");
	 	 pid = fork();
		 if(pid < 0)
			error("ERROR on FORK()");
		 if(pid == 0)
		 {
			close(sockfd);
			dostuff(newsockfd, pid);
			exit(0);
		 }
		 else
			close(newsockfd);
	 }//end of while(1)

     close(sockfd);
     return 0; 
}


/* function dostuff() 
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 */
void dostuff (int sock, int pid)
{
	int n, user, pw;
	char buffer[256];

	//handle the reading   
	bzero(buffer,256);
	n = read(sock,buffer,255);
	if (n < 0) error("ERROR reading from socket");
	
	//testing
	printf("Here is the PID:%d\n", pid);
	printf("Here is the message: %s\n",buffer);

	if(pid == 0) // this means we have the username
	{
		user = write(sock, "I got your username", 20);

		//does the user exist? otherwise exit
	}
	else if (pid == 1) // this means we have the password
	{
		pw = write(sock,"I got your password",18);
		auth(user, pw); //if false = exit
		//if true
		int idlist;
		idlist = getDocID(user);
		n = write(sock, "I have your doc Ids:", 25); 
		//print the list?
	}
	else //return an error
	{
		n = write(sock,"You messed up.", 18);
	}
}

bool auth( char user, char pw){
	//check the user + pw
	//if incorrect, return false
	return true;
}

int getDocID(char user){
	//take the user
	//and find their list of documents
	//return the IDs
	return 1;
}

