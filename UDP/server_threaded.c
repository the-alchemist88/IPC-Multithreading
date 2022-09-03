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

void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}

//Lists network adapter addresses on current machine(on Windows works properly, on Linux shows 127.0.1.1)
static void publishIPAddressInfo(const char* host)
{
	struct hostent *he = gethostbyname(host);
	if (he == NULL) // NULL return value means resolution attempt failed
	{ 
		perror("gethostbyname: ");
		return;
	}
	printf ("Host name is %s\n", he->h_name);
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

void* receiver_th();
int retVal;
int sockfd;
struct sockaddr_in server_addr, client_addr;

int main(int argc, char **argv)
{
	char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
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

  int port = atoi(argv[1]);
  char *ip = "192.168.1.38"; // Address to set host IP (uncomment where "ip" is assigned to s_addr and comment INADDR_ANY line to be in action)
 
//  int sockfd;
//  struct sockaddr_in server_addr, client_addr;
//  char buffer[BUFFER_SIZE];
//  socklen_t addr_size;

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0){
    perror("[-]socket error");
    exit(1);
  }

  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(port);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Generic IP adress (INADDR_ANY) for host
//  server_addr.sin_addr.s_addr = inet_addr(ip);

  char* assignedAddr = inet_ntoa(server_addr.sin_addr); ;
  printf("\nIP that is assigned to host : %s\n", assignedAddr); // for INADDR_ANY, prints 0.0.0.0

  if ( (retVal = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ) < 0) {
    perror("[-]bind error");
    exit(1);
  }

  pthread_t receiver;
  retVal = pthread_create(&receiver,NULL,receiver_th, NULL);
	if(retVal != 0)
	{
	  perror("Error on creating thread");
	  exit(0);
	}
	
	pthread_join(receiver, NULL);

  return 0;
}

void* receiver_th()
{
  printf("%s started\n",__FUNCTION__);
  char buffer[BUFFER_SIZE];
  bzero(buffer, BUFFER_SIZE);
  strcpy(buffer, "Hello, World!");
  socklen_t addr_size;
  int msgNum = 0;
  while(1)
  {
	    addr_size = sizeof(client_addr);
		if( (retVal = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_addr, &addr_size)) < 0 )
		{
		  perror("sendto");
		  exit(1);
		}
  printf("\n[%d]Msg send: %s\n", ++msgNum, buffer);
  sleep(1);
  }
  
  pthread_exit(0);
}

