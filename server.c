#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUFFER_SIZE 1000

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;	
	memset(&server_addr, 0, sizeof server_addr);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1100);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);	
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
		perror("cannot bind socket");
		close(server_socket);
		exit(EXIT_FAILURE);
	}

    if (listen(server_socket, 100) == -1) {
        perror("cannot make socket listen");
        close(server_socket);
        exit(EXIT_FAILURE);
	}


    char buff[BUFFER_SIZE];
	while (1) {
		int client_communication_socket = accept(server_socket, NULL, NULL);
		if (client_communication_socket == -1) {
            perror("cannot accept, skip this connection");
            close(server_socket);
		} else {
			printf("Connection accepted \n");
		}

		
        bzero(buff, BUFFER_SIZE);
        recv(client_communication_socket, buff, sizeof buff, 0);
        printf("client: %s \n", buff);


        bzero(buff, 1000);
        printf("wpisz wiadomosc dla clienta:");
        fgets(buff, sizeof(buff), stdin);
        printf("\n");
        write(client_communication_socket, buff, sizeof buff);
		
		
		close(client_communication_socket);
		printf("Connection closed \n\n");
	}

	close(server_socket);
}
