#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

static void publishIPAddressInfo(const char* host) 
{
	struct hostent *he = gethostbyname(host);
	if (he == NULL) { // NULL return value means resolution attempt failed
        printf ("%s could not be resolved to an address. Did you mistype it?\n", host);
		return;
	}
	printf ("Client name is %s\n", he->h_name);
	printf  ("\nIP Addresses: \n");
	struct in_addr **addressList = (struct in_addr **) he->h_addr_list;
	unsigned short IPcounter = 1;
	while (*addressList != NULL) 
	{
		char str[INET_ADDRSTRLEN];
		printf( "%d) %s\n", IPcounter, inet_ntop(AF_INET, *addressList, str, INET_ADDRSTRLEN));
		addressList++;
		IPcounter ++;
	}
}

void* sender_th();
int retVal;
int sockfd;
struct sockaddr_in addr;

int main(int argc, char **argv)
{
  
  char hostbuffer[256];
  int hostname;
  
    // To retrieve hostname
  hostname = gethostname(hostbuffer, sizeof(hostbuffer));
  checkHostName(hostname);
  publishIPAddressInfo(hostbuffer);
  
    if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }
  
  char *ip = "192.168.1.39"; // Windows:192.168.1.38 -- 10.0.2.10 Ubuntu:127.0.0.1 VB:192.168.56.1 
  int port = atoi(argv[1]);
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  
  char* assignedAddr = inet_ntoa(addr.sin_addr);
  printf("\nIP that is client sending msg to : %s\n", assignedAddr);
  
  pthread_t sender;
  retVal = pthread_create(&sender, NULL, sender_th, NULL);
	if(retVal != 0)
	{
		perror("Error on creating thread");
	  exit(0);
	}
	
	pthread_join(sender, NULL);

  return 0;
}


void* sender_th()
{
  printf("%s started\n", __FUNCTION__);
  char buffer[BUFFER_SIZE];
  int msgNum = 0;
  while(1)
  {
  bzero(buffer, BUFFER_SIZE);
  strcpy(buffer, "Hello, World!");
		if( (retVal = sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr))) < 0 )
		{
		  perror("sendto");
		  exit(1);
		}
  printf("\n[%d]Msg send: %s\n", ++msgNum, buffer);
  sleep(1);
  }
  
  pthread_exit(0);
}

