
all:
	clang src/client.c -o client
	clang src/server.c -o server -I./include
