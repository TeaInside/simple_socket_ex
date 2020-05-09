CC=clang
ifeq (, $(shell which clang))
$(error "No clang in $(PATH), using gcc")
CC=gcc
endif



all:
	$(CC) src/client.c -o client -I./include
	$(CC) src/server.c -o server -I./include

clean:
	rm server
	rm client