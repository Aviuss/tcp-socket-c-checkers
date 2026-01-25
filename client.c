#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "checkers.h"
#define BUFFER_SIZE 1000

void deserialize_game(char *buffer, struct Game *game) {
    memcpy(game->board, buffer, 64);
    game->turn = buffer[64];
}

int split_by_space(const char *input, char ***words) {
    if (!input) return 0;

    int count = 0;
    const char *tmp = input;

    while (*tmp) {
        while (*tmp == ' ') tmp++;
        if (*tmp) count++;
        while (*tmp && *tmp != ' ') tmp++;
    }

    *words = malloc(count * sizeof(char*));
    if (!*words) return 0;

    int index = 0;
    tmp = input;
    while (*tmp) {
        while (*tmp == ' ') tmp++;
        if (*tmp) {
            const char *start = tmp;
            while (*tmp && *tmp != ' ') tmp++;
            int len = tmp - start;
            (*words)[index] = malloc(len + 1);
            strncpy((*words)[index], start, len);
            (*words)[index][len] = '\0';
            index++;
        }
    }

    return count;
}

int SocketFD;
int partyId = -1;
char buff[BUFFER_SIZE];
struct Game game;

void updateBoard() {
    bzero(buff, BUFFER_SIZE);
    strcpy(buff, "getboard");
    write(SocketFD, buff, sizeof buff);   
    bzero(buff, BUFFER_SIZE);
    read(SocketFD, buff, sizeof buff);
    deserialize_game(buff, &game);
}

void createGame() {
    bzero(buff, BUFFER_SIZE);
    strcpy(buff, "create");
    write(SocketFD, buff, sizeof buff);   

    bzero(buff, BUFFER_SIZE);
    read(SocketFD, buff, sizeof buff);
    if (strcmp(buff, "-1") == 0) {
        return;
    }
    partyId = atoi(buff);
}


int lobbyReady() {
    bzero(buff, BUFFER_SIZE);
    strcpy(buff, "lobbyReady");
    write(SocketFD, buff, sizeof buff);   

    bzero(buff, BUFFER_SIZE);
    read(SocketFD, buff, sizeof buff);
    if (strcmp(buff, "-1") == 0) {
        return 0;
    }
    return 1;
}

void joinGame(char* buffor, size_t len) {
    printf("%d\n", len);
    write(SocketFD, buffor, len);   

    bzero(buff, BUFFER_SIZE);
    read(SocketFD, buff, sizeof buff);
    if (strcmp(buff, "-1") == 0) {
        return;
    }
    partyId = atoi(buff);
}

int main(int argc, char *argv[])
{
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
	
    char color = 'w';

    while (partyId == -1) {
        system("clear");
        printf("Checkers online \ntype 'create' to create a lobby \ntype 'join [party id]' to join a lobby \n> ");
        bzero(buff, BUFFER_SIZE);
        fgets(buff, sizeof(buff), stdin);
        printf("\n");

        char **words;
        buff[strcspn(buff, "\r\n")] = '\0';
        int num_words = split_by_space(buff, &words);

        if (num_words >= 1 && strcmp(words[0], "create") == 0) {
            createGame();
            printf("Party created. Party id is '%d' \nWaiting for other player to join .. .\n", partyId);
            while(lobbyReady() == 0) {
                sleep(1);
            }
        } else if (num_words >= 2 && strcmp(words[0], "join") == 0) {
            joinGame(buff, sizeof buff);
            if (partyId != -1) {
                color = 'b';
            }
        }

        for (int i = 0; i < num_words; i++) {
            free(words[i]);
        }
        free(words);
    }

    updateBoard();
    system("clear");
    printBoard(&game);

    while (1)
    {
        if (game.turn == color) {
            printf("Type turn sequence (e.g. A1=>B2 or A1=>B2=>A3): ");
            bzero(buff, sizeof(buff));
            fgets(buff, sizeof buff, stdin);
            printf("\n");

            char* prefix = "move ";
            size_t prefix_len = strlen(prefix);
            size_t original_len = strlen(buff);
            memmove(buff + prefix_len, buff, original_len + 1);
            memcpy(buff, prefix, prefix_len);
            write(SocketFD, buff, sizeof(buff));
            
            bzero(buff, sizeof(buff));
            read(SocketFD, buff, sizeof(buff));
            printf("buff: %s\n", buff);
            if (strcmp(buff, "1") == 0) {
                updateBoard();
            }
        } else {
            printf("Opponents turn .. .\n");
            while(game.turn != color) {
                sleep(1);
                updateBoard();
            }
        }

        system("clear");
        printBoard(&game);
    }
    
    close(SocketFD);
    return EXIT_SUCCESS;
}