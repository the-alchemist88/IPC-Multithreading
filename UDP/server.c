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
        perror("inet_ntoa: ");
        exit(1);
    }
}

static void publishIPAddressInfo(const char* host) // Lists the IP addresses that the current machine uses
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

int main(int argc, char **argv){
	
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
  char *ip = "127.0.0.1"; // Address to set host IP (uncomment where "ip" is assigned to s_addr and comment INADDR_ANY line to be in action)
 
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[1024];
  socklen_t addr_size;
  int n;

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

  n = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)); //
  if (n < 0) {
    perror("[-]bind error");
    exit(1);
  }

  bzero(buffer, 1024);
  addr_size = sizeof(client_addr);
  recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, &addr_size);
  printf("\n[+]Data recv: %s\n", buffer);

  bzero(buffer, 1024);
  strcpy(buffer, "Welcome to the UDP Server.");
  sendto(sockfd, buffer, 1024, 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
  printf("[+]Data send: %s\n", buffer);

  return 0;
}
