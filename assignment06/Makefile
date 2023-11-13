CC = gcc

.c.o: 
	$(CC) -c $<

all: name6

name6: name6.o bst.o
	$(CC) -o $@ name6.o bst.o
	
clean:
	rm -f *.o
	rm -f name6
