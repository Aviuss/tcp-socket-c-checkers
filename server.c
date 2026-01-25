#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<fcntl.h> 
#include<unistd.h> 
#include<pthread.h>
#define BUFFER_SIZE 1000
#define MAX_GAMES 100

int buffer_to_int(char *buf, size_t len) {
    int value = 0;
    size_t copy_len = len < sizeof(int) ? len : sizeof(int);
    for (size_t i = 0; i < copy_len; i++) {
        value |= buf[i] << (8 * i);
    }
    return value;
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

struct gameDataStruct {
    int active;              // 0 = inactive, 1 = active, 2 = awaits for other player
    pthread_mutex_t lock;    // Mutex for thread-safe access
};

struct gameDataStruct gameData[MAX_GAMES];

int getNewGameData() {
    for (int index = 0; index < MAX_GAMES; index++) {
        pthread_mutex_lock(&gameData[index].lock);
        if (gameData[index].active == 0) {
            gameData[index].active = 2;
            pthread_mutex_unlock(&gameData[index].lock);
            return index;
        }
        pthread_mutex_unlock(&gameData[index].lock);
    }
    return -1;
}

struct playerThreadData {
    int sockedFd;
    int gameDataIndex;
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

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(newSocket, &readfds);

        timeout.tv_sec = 5; timeout.tv_usec = 0; // 5sec
        int ready = select(newSocket + 1, &readfds, NULL, NULL, &timeout);
        if (ready < 0) {
            perror("select error");
            break;
        } else if (ready == 0) {
            continue;
        } else {
            int n = recv(newSocket, buff, sizeof(buff), 0);
            if (n <= 0) {
                break;
            }

            char **words;
            buff[strcspn(buff, "\r\n")] = '\0';
            int num_words = split_by_space(buff, &words);

            /*printf("Found %d words:\n", num_words);
            for (int i = 0; i < num_words; i++) {
                printf("[%d]: %s\n", i, words[i]);
            }*/

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
                }
            } else if (gameDataIndex == -1 && num_words >= 2 && strcmp(words[0], "join") == 0) {
                int partyId = buffer_to_int(words[1], sizeof(words[1]));
                
                if (!(partyId >= 0 && partyId < MAX_GAMES)) {
                    bzero(buff, BUFFER_SIZE);
                    strcpy(buff, "-1");
                    write(newSocket, buff, sizeof(buff));
                }

                pthread_mutex_lock(&gameData[partyId].lock);
                if (gameData[partyId].active == 2) {
                    gameData[partyId].active = 1;
                } else {
                    partyId = -1;
                }
                pthread_mutex_unlock(&gameData[partyId].lock);
                    

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
            }
            
            for (int i = 0; i < num_words; i++) {
                free(words[i]);
            }
            free(words);
        }

    }
    
    free(data);
    close(newSocket);
    pthread_exit(NULL);
}

int main(){
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