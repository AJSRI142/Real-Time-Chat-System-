all: server

server: server.o
	g++ -o server server.cpp -lpthread

client: client.o 
	g++ -o client client.cpp -lpthread

client.o: client.cpp client.h
	g++ -c client.cpp

sever.o: server.cpp server.h
	g++ -c server.cpp

clean: 
	rm server client