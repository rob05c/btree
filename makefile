CC=g++
OPTIONS=-std=c++11 -g -Wfatal-errors
LINK_OPTIONS=-ltbb
all: btree
btree: main.o btree.o
	$(CC) $(OPTIONS) main.o btree.o -o btree $(LINK_OPTIONS)
main.o: 
	$(CC) $(OPTIONS) -c main.cpp -o main.o
btree.o: 
	$(CC) $(OPTIONS) -c btree.cpp -o btree.o
clean:
	rm -f *o btree
