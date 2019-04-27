
all:client server

client: client.c
	g++ -o $@ $^ -std=c++11 -g -pthread

server: server.cc 
	g++ -o $@ $^ -std=c++11 -g -pthread

.PHONY:clean

clean:
	rm -f client server
