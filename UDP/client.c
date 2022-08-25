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
  
//Lists network adapter addresses on current machine(on Windows works properly, on Linux shows 127.0.1.1)
static void publishIPAddressInfo(const char* host) 
{
	struct hostent *he = gethostbyname(host);
	if (he == NULL)
	{ // NULL return value means resolution attempt failed
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


int main(int argc, char **argv){
  
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
	int retVal;
  
// To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
	
	publishIPAddressInfo(hostbuffer);
  
    if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }
  
// Address to send data 
  char *ip = "127.0.0.1"; 
  int port = atoi(argv[1]);

  int sockfd;
  struct sockaddr_in addr;
  char buffer[BUFFER_SIZE];
  socklen_t addr_size;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  
  char* assignedAddr = inet_ntoa(addr.sin_addr); ;
  printf("\nIP that is client sending msg to : %s\n", assignedAddr);

  bzero(buffer, BUFFER_SIZE);
  strcpy(buffer, "Hello, World!");
  sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr));
  if( (retVal = sendto(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, sizeof(addr))) != BUFFER_SIZE )
  {
    perror("sendto");
    exit(1);
  }
  printf("\n[+]Data send: %s\n", buffer);

  bzero(buffer, BUFFER_SIZE);
  addr_size = sizeof(addr);
  if( (retVal = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&addr, &addr_size)) < 0 )
  {
    perror("recvfrom");
    exit(1);
  }
  printf("[+]Data recv: %s\n", buffer);

  return 0;
}
