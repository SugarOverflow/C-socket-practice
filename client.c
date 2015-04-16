#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /*defines the structure hostent*/

/*will be called if the system call fails.
Displays an error message from stderr*/
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
	
/*the main function*/
int main(int argc, char *argv[])
{	
	//sockfd and newsockfd are file descriptors
    //they store the values returned by the socket system call
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	//server is a pointer to hostent type
	//defines a host computer on the internet

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
	
	//if a socket call fails it returns -1
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

	//could not locate a host with that name
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

	//bzero sets all values in a buffer to zero
	//takes a pointer to the buffer, and the size of the buffer
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;

	//copies length byes from one to the other
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

	//connect is called to establish the connection
	//takes the socket file descriptor
	//the address of the host to connect to
	//and the size of the address
	//returns 0 on success, -1 on failure
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
	
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);

    close(sockfd);
    return 0;
}
