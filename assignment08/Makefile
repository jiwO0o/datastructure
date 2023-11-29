CC = gcc

.c.o: 
	$(CC) -c $<

all: name7

name7: name7.o avlt.o
	$(CC) -o $@ name7.o avlt.o
	
clean:
	rm -f *.o
	rm -f name7
