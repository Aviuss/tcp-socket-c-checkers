# tcp-socket-c-checkers

clear && gcc -Wall main.c checkers.c checkersInputParsing.c -o game && ./game

clear && gcc -Wall server.c checkers.c checkersInputParsing.c -o server && ./server
clear && gcc -Wall client.c checkers.c checkersInputParsing.c -o client && ./client