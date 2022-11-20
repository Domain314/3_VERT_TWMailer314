#############################################################################################
# Makefile
#############################################################################################
# G++ is part of GCC (GNU compiler collection) and is a compiler best suited for C++
# Compiler Flags: https://linux.die.net/man/1/g++
CC=g++
CFLAGS=-g -O -std=c++2a -I -pthread
#CFLAGS=-g -Wall -Wextra -O -std=c++2a -I -pthread

rebuild: clean all
all: server client

clean:
	clear
	rm -f ./server/server ./client/client

server:
	${CC} ${CFLAGS} -o server314/server server314/main.cpp server314/ServerLogic.cpp server314/FileOrganizer.cpp server314/ComSocket.cpp

client:
	${CC} ${CFLAGS} -o client314/client client314/main.cpp client314/ClientLogic.cpp -lreadline