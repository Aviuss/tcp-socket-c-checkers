# tcp-socket-c-checkers

clear && gcc -w -Wall server.c checkers.c checkersInputParsing.c -o server && ./server
clear && gcc -w -Wall client.c checkers.c checkersInputParsing.c -o client && ./client