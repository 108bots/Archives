C_LINK = -lpthread -lsocket -lnsl -lm

CC = gcc

all:    p2pfs 
	@echo "Make Successful"
	@echo "Created by : Hemanth Srinivasan"
	@echo "**************************************"
	@echo "Usage: p2pfs <node ID> <Active p2p nodes> <Shared Files Data>"
	@echo "**************************************"

clean:
	rm -f *.o

p2pfs:  p2pfs.c

p2pfs:
	$(CC) -o p2pfs p2pfs.c $(C_LINK)

