
FLAG=-std=c++11 -lboost_filesystem -lboost_system -I /home/ym/cppjieba/include -ljsoncpp

.PHONY:all

all:libsearcher.so test

test:test.cc searcher.cc searcher.h
	g++ -o $@ $^  $(FLAG) -g

libsearcher.so:searcher.h searcher.cc
	g++ -o $@ searcher.cc -shared -fPIC $(FLAG)

.PHONY:clean

clean:
	rm -f test libsearcher.so
