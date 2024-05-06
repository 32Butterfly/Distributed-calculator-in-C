all: server client

server: server.c
	gcc -o server server.c -lm

client: client.c
	gcc -o client client.c -lm

clean:
	rm -f server client

