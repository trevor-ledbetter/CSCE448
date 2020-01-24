CXX = g++ -g -std=c++11

all: server client

server: crsd.cpp
	$(CXX) crsd.cpp -o server -lpthread -lrt

client: crc.cpp
	$(CXX) crc.cpp -o client

clean:
	rm -f client server