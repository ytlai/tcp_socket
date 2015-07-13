/*
@author Yuting Lai
(c)2015
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char *argv[])
{
	int sockfd, portno, n,stop=0;

	struct sockaddr_in serv_addr;
	char server[15];

	char buffer[256];
	if (argc < 3) {
		fprintf(stderr,"usage %s serverip port\n", argv[0]);
		exit(0);
	}
	strcpy(server,argv[1]);
	portno = atoi(argv[2]);
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");


	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(server);
	serv_addr.sin_port = htons(portno);

	if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
		error("ERROR connecting");
	bzero(buffer,256);
	n = read(sockfd,buffer,255);
	if (n < 0) 
		error("ERROR reading from socket");
	printf("%s\n",buffer);

	printf("Please enter the resource id: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	n = write(sockfd,buffer,strlen(buffer));
	if (n < 0) 
		error("ERROR writing to socket");

	while(!stop){
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if (n <0) 
			error("ERROR reading from socket");

		if(!strcmp(buffer,"End"))
			stop=1;


	printf("%s\n",buffer);
	}

	return 0;
}
