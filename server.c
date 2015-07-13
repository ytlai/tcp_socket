/*
@author Yuting Lai
(c)2015
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <unistd.h>

#define AMOUNT_RES 2
#define RES_TYPE 3

sem_t semaphores[RES_TYPE];
char open_msg[100];

void error(char *msg)
{
	perror(msg);
	exit(1);
}

typedef struct m_args{
	int newsockfd;
	int k;
}m_args;

void get_resource_info(){
	int i,ret;	
	for(i=0;i<RES_TYPE;i++){	
		sem_getvalue(&semaphores[i],&ret);
		if (!i)
			sprintf(open_msg,"Number of resource %d: %d\n",i,ret);
		else
			sprintf(open_msg,"%sNumber of resource %d: %d\n",open_msg,i,ret);
	}
		
}

void* msg_handle(void *args){
	m_args *argss = args;
	char buffer[256];
	int n,res_id,ret,flag=1;
	char *r_msg =malloc(sizeof(char)*100);

	
	
	printf("Thread%d: Created!\n",argss->k);
	get_resource_info();
	n = write(argss->newsockfd,open_msg,strlen(open_msg));
	if (n < 0) error("ERROR writing to socket");

	bzero(buffer,256);
	n = read(argss->newsockfd,buffer,255);
	
	if (n < 0) error("ERROR reading from socket");

	
	buffer[1]='\0';

	
	res_id=atoi(buffer);
	
	
	

	while(sem_trywait(&semaphores[res_id]))
		if (flag){
			printf("Thread%d: Wait...\n",argss->k);
			sprintf(r_msg,"Thread %d has to Wait for resource %d",argss->k,res_id);
			n = write(argss->newsockfd,r_msg,strlen(r_msg));
			if (n < 0) error("ERROR writing to socket");
			flag=0;
		}

	
	sem_getvalue(&semaphores[res_id],&ret);
	printf("Thread%d: Resource %d is provisioned \n",argss->k,res_id);
	printf("Amount of resource %d : %d\n",res_id,ret);

	
	sprintf(r_msg,"Resource %d Get!",res_id);
	

	n = write(argss->newsockfd,r_msg,strlen(r_msg));
	if (n < 0) error("ERROR writing to socket");

	sleep(10);
	



	
	sem_post(&semaphores[res_id]);
	sem_getvalue(&semaphores[res_id],&ret);
	printf("Thread%d: Resource %d is withdrew \n",argss->k,res_id);
	printf("Amount of resource %d : %d\n",res_id,ret);
	strcpy(r_msg,"End");
	n = write(argss->newsockfd,r_msg,strlen(r_msg));
	if (n < 0) error("ERROR writing to socket");

	free(r_msg);

	return NULL;

}



int main(int argc, char *argv[])
{
	int sockfd, newsockfd, portno, clilen;

	struct sockaddr_in serv_addr, cli_addr;



	int i,j;

	if (argc < 2) {
		fprintf(stderr,"usage %s port\n", argv[0]);
		exit(1);
	}

	//define amount of resources
	for(i=0;i<RES_TYPE;i++)
		sem_init(&semaphores[i],0,AMOUNT_RES);


	get_resource_info();
	printf("%s" ,open_msg);

	i=0;j=1;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(int*)&j,sizeof(int));
	if (sockfd < 0)
		error("ERROR opening socket");
	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = atoi(argv[1]);

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
	      sizeof(serv_addr)) < 0)
	      error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);

	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

		if (newsockfd < 0){
			 error("ERROR on accept");
			 continue;
		 }

		pthread_t *_p=malloc(sizeof(pthread_t));
		m_args *args = malloc(sizeof(m_args));

		args->k=++i;
		args->newsockfd= newsockfd;


		if (pthread_create(_p,NULL,&msg_handle,args)!=0)
			error("Failed to create thread");


	}
	return 0;
}


