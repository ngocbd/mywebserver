all:
	g++ -std=c++11 -Wall -g -c webserver.cpp
	g++ -Wall -g webserver.o -levent -o webserver
clean:
	rm -f webserver webserver.o
