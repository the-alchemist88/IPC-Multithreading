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

// Returns hostname for the local computer
void checkHostName(int hostname)
{
    if (hostname == -1)
    {
        perror("gethostname");
        exit(1);
    }
}
  
// Returns host information corresponding to host name
void checkHostEntry(struct hostent * hostentry)
{
    if (hostentry == NULL)
    {
        perror("gethostbyname");
        exit(1);
    }
}
  
// Converts space-delimited IPv4 addresses
// to dotted-decimal format
void checkIPbuffer(char *IPbuffer)
{
    if (NULL == IPbuffer)
    {
        perror("inet_ntoa");
        exit(1);
    }
}

static void publishIPAddressInfo(const char* host) 
{
	struct hostent *he = gethostbyname(host);
	if (he == NULL) ,
	{ // NULL return value means resolution attempt failed
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


int main(int argc, char **argv){
  
    char hostbuffer[256];
    char *IPbuffer;
    struct hostent *host_entry;
    int hostname;
  
    // To retrieve hostname
    hostname = gethostname(hostbuffer, sizeof(hostbuffer));
    checkHostName(hostname);
	
	publishIPAddressInfo(hostbuffer);
  
    if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    exit(0);
  }
  
  char *ip = "127.0.1.1"; // Address to send data
  int port = atoi(argv[1]);

  int sockfd;
  struct sockaddr_in addr;
  char buffer[1024];
  socklen_t addr_size;
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&addr, '\0', sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ip);
  
   char* assignedAddr = inet_ntoa(addr.sin_addr); ;
  printf("\nIP that is client sending msg to : %s\n", assignedAddr);

  bzero(buffer, 1024);
  strcpy(buffer, "Hello, World!");
  sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&addr, sizeof(addr));
  printf("\n[+]Data send: %s\n", buffer);

  bzero(buffer, 1024);
  addr_size = sizeof(addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&addr, &addr_size);
  printf("[+]Data recv: %s\n", buffer);

  return 0;
}
