

all: exe

exe: node.o backend.o queue.o bookorder.o
	gcc -lpthread -o order node.o backend.o queue.o bookorder.o	

node.o: node.c node.h
	gcc -c node.c 

backend.o: backend.c backend.h
	gcc -c backend.c 

queue.o: queue.c queue.h
	gcc -c queue.c

bookorder.o: bookorder.c
	gcc -c bookorder.c	

clean:
	rm -f *.o
	rm -f order
