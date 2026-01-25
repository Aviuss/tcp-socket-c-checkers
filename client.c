#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define BUFFER_SIZE 1000

int main(int argc, char *argv[])
{
    int SocketFD;
	SocketFD = socket(AF_INET, SOCK_STREAM, 0);
	if (SocketFD == -1) {
		perror("cannot create socket");
		exit(EXIT_FAILURE);
	}
    struct sockaddr_in sa;
	memset(&sa, 0, sizeof sa);
	sa.sin_addr.s_addr=inet_addr("127.0.0.1");
	sa.sin_family = AF_INET;
	sa.sin_port = htons(1100);
    
    if (connect(SocketFD, (struct sockaddr *)&sa, sizeof sa) == -1) {
		perror("connect failed");
		close(SocketFD);
		exit(EXIT_FAILURE);
	}
	
    while (1)
    {
        char buff[BUFFER_SIZE];
        bzero(buff, BUFFER_SIZE);
        printf("wpisz wiadomosc dla servera:");
        fgets(buff, sizeof(buff), stdin);
        printf("\n");
        write(SocketFD, buff, sizeof buff);
        
        
        bzero(buff, 1000);
        read(SocketFD, buff, sizeof buff);
        printf("server: %s \n", buff);
        close(SocketFD);       
    }
    
    return EXIT_SUCCESS;
}