#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h> 
#include<unistd.h> 
#include<pthread.h>
#include "checkers.h"
#define BUFFER_SIZE 1000
#define MAX_GAMES 100


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

struct gameDataStruct {
    struct Game *game;
    int active;              // 0 = inactive, 1 = white move, -1 = black move, 2 = awaits for other player, 5 = other player disconnected
    pthread_mutex_t lock;    // Mutex for thread-safe access
};

struct gameDataStruct gameData[MAX_GAMES];

void serialize_game(struct gameDataStruct *gameData, char *buffer) {
    memcpy(buffer, gameData->game->board, 64);
    buffer[64] = gameData->game->turn;
    buffer[65] = gameData->game->won;
    buffer[66] = gameData->active;
}


int getNewGameData() {
    for (int index = 0; index < MAX_GAMES; index++) {
        pthread_mutex_lock(&gameData[index].lock);
        if (gameData[index].active == 0) {
            gameData[index].active = 2;
            gameData[index].game = NULL;
            pthread_mutex_unlock(&gameData[index].lock);
            return index;
        }
        pthread_mutex_unlock(&gameData[index].lock);
    }
    return -1;
}

struct playerThreadData {
    int sockedFd;
};

void *playerThread(void *arg)
{
    struct playerThreadData *data = (struct playerThreadData *)arg;
    int newSocket = data->sockedFd;
    char buff[BUFFER_SIZE];
    int gameDataIndex = -1;

    bzero(buff, BUFFER_SIZE);

    fd_set readfds;
    struct timeval timeout;
    char color = 'w';

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(newSocket, &readfds);

        timeout.tv_sec = 5; timeout.tv_usec = 0; // 5sec
        int ready = select(newSocket + 1, &readfds, NULL, NULL, &timeout);
        if (ready < 0) {
            pthread_mutex_lock(&gameData[gameDataIndex].lock);
            if (gameDataIndex != -1 && gameData[gameDataIndex].game != NULL) {
                gameData[gameDataIndex].active = 5;
                pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                sleep(60);
                pthread_mutex_lock(&gameData[gameDataIndex].lock);
                if (gameData[gameDataIndex].game != NULL) {
                    free(gameData[gameDataIndex].game);
                    gameData[gameDataIndex].game = NULL;
                }
            }
            pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                
            free(data);
            close(newSocket);
            pthread_exit(NULL);
            break;
        } else if (ready == 0) {
            continue;
        } else {
            int n = recv(newSocket, buff, sizeof(buff), 0);
            if (n <= 0) {
                pthread_mutex_lock(&gameData[gameDataIndex].lock);
                if (gameDataIndex != -1 && gameData[gameDataIndex].game != NULL) {
                    gameData[gameDataIndex].active = 5;
                    pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                    sleep(60);
                    pthread_mutex_lock(&gameData[gameDataIndex].lock);
                    if (gameData[gameDataIndex].game != NULL) {
                        free(gameData[gameDataIndex].game);
                        gameData[gameDataIndex].game = NULL;
                    }
                }
                pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                    
                free(data);
                close(newSocket);
                pthread_exit(NULL);
                break;
            }

            char **words;
            buff[strcspn(buff, "\r\n")] = '\0';
            int num_words = split_by_space(buff, &words);

            printf("\nFound %d words:\n", num_words);
            for (int i = 0; i < num_words; i++) {
                printf("[%d]: %s\n", i, words[i]);
            }

            if (gameDataIndex == -1 && num_words >= 1 && strcmp(words[0], "create") == 0) {
                gameDataIndex = getNewGameData();
                if (gameDataIndex == -1) {
                    bzero(buff, BUFFER_SIZE);
                    strcpy(buff, "-1");
                    write(newSocket, buff, sizeof(buff));
                } else {
                    bzero(buff, BUFFER_SIZE);
                    sprintf(buff, "%d", gameDataIndex);
                    write(newSocket, buff, sizeof(buff));
                    printf("Assigned %d as new game\n", gameDataIndex);
                    pthread_mutex_lock(&gameData[gameDataIndex].lock);
                    gameData[gameDataIndex].game = malloc(sizeof(struct Game));
                    if (num_words >= 2 && strcmp(words[1], "endgame") == 0) {
                        initEndGame(gameData[gameDataIndex].game);
                    } else {
                        initGame(gameData[gameDataIndex].game);
                    }
                    
                    pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                }
            } else if (gameDataIndex == -1 && num_words >= 2 && strcmp(words[0], "join") == 0) {
                int partyId = atoi(words[1]);
                printf("partyId: %d", partyId);

                if (!(partyId >= 0 && partyId < MAX_GAMES)) {
                    bzero(buff, BUFFER_SIZE);
                    strcpy(buff, "-1");
                    write(newSocket, buff, sizeof(buff));
                }

                pthread_mutex_lock(&gameData[partyId].lock);
                if (gameData[partyId].active == 2) {
                    gameData[partyId].active = 1;
                    color = 'b';
                    pthread_mutex_unlock(&gameData[partyId].lock);
                } else {
                    pthread_mutex_unlock(&gameData[partyId].lock);
                    partyId = -1;
                }
                    
                printf("partyId: %d", partyId);

                if (partyId != -1) {
                    gameDataIndex = partyId;
                    bzero(buff, BUFFER_SIZE);
                    strcpy(buff, "1");
                    write(newSocket, buff, sizeof(buff));
                } else {
                    bzero(buff, BUFFER_SIZE);
                    strcpy(buff, "-1");
                    write(newSocket, buff, sizeof(buff));
                }
            } else if (gameDataIndex != -1 && num_words >= 1 && strcmp(words[0], "lobbyReady") == 0) {
                pthread_mutex_lock(&gameData[gameDataIndex].lock);
                bzero(buff, sizeof(buff));

                if (gameData[gameDataIndex].active == 2) {
                    strcpy(buff, "-1");
                } else {
                    strcpy(buff, "1");
                }
                write(newSocket, buff, sizeof(buff));

                pthread_mutex_unlock(&gameData[gameDataIndex].lock);
            } else if (gameDataIndex != -1 && num_words >= 1 && strcmp(words[0], "getboard") == 0) {
                pthread_mutex_lock(&gameData[gameDataIndex].lock);
                bzero(buff, sizeof(buff));
                if (gameData[gameDataIndex].game == NULL) {
                    strcpy(buff, "-1");
                } else {
                    serialize_game(&gameData[gameDataIndex], buff);
                }
                pthread_mutex_unlock(&gameData[gameDataIndex].lock);
                write(newSocket, buff, sizeof(buff));
            } else if (gameDataIndex != -1 && num_words >= 2 && strcmp(words[0], "move") == 0) {
                pthread_mutex_lock(&gameData[gameDataIndex].lock);
                if ((gameData[gameDataIndex].game)->turn == color) {
                    int isvalid = makeMove(gameData[gameDataIndex].game, color, words[1], strlen(words[1]));
                    bzero(buff, sizeof(buff));
                    if (isvalid == 1) {
                        strcpy(buff, "1");
                    } else {
                        strcpy(buff, "-1");
                    }
                    write(newSocket, buff, sizeof(buff));
                }
                pthread_mutex_unlock(&gameData[gameDataIndex].lock);
            }
            
            for (int i = 0; i < num_words; i++) {
                free(words[i]);
            }
            free(words);
        }

    }
    
    sleep(5);
    if (gameDataIndex != -1) {
        pthread_mutex_lock(&gameData[gameDataIndex].lock);
        if (gameData[gameDataIndex].game != NULL) {
            free(gameData[gameDataIndex].game);
            gameData[gameDataIndex].game = NULL;
        }
        pthread_mutex_unlock(&gameData[gameDataIndex].lock);
    }

    free(data);
    close(newSocket);
    pthread_exit(NULL);
}

int main(){
    system("clear");
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serverAddr, 0, sizeof serverAddr);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(1100);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == -1) {
        perror("cannot bind socket");
        close(serverSocket);    
        exit(EXIT_FAILURE);
    }

	if(listen(serverSocket,10)==0) {
		printf("Listening\n");
	} else {
		printf("Error\n");
	}

    for (int i = 0; i < MAX_GAMES; i++) {
        gameData[i].active = 0;
        pthread_mutex_init(&gameData[i].lock, NULL);
    }

	pthread_t thread_id;
	while(1)
	{
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

        struct playerThreadData *data = malloc(sizeof(struct playerThreadData));
        data->sockedFd = newSocket;

        if(pthread_create(&thread_id, NULL, playerThread, data) != 0 ) {
            free(data);    
            close(newSocket);
            printf("Failed to create thread\n");
        }

        pthread_detach(thread_id);
	}

	close(serverSocket);
    for (int i = 0; i < MAX_GAMES; i++) {
        pthread_mutex_destroy(&gameData[i].lock);
    }
	return 0;
}