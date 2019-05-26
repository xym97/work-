
all:client server

client: client.cc
	g++ -o $@ $^ -pthread

server: server.cc 
	g++ -o $@ $^ -std=c++11 -pthread

.PHONY:clean

clean:
	rm -f client server
