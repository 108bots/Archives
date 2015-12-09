C_LINK = -lpthread -lsocket -lnsl -lm

CC = gcc

all:    vectorClocks 
	@echo "Make Successful"
	@echo "Created by : Hemanth Srinivasan"
	@echo "**************************************"
	@echo "usage: vectorClocks <Process ID> <Config File>"
	@echo "**************************************"

clean:
	rm -f *.o

vectorClocks:  vectorClocks.c

vectorClocks:
	$(CC) -o vectorClocks vectorClocks.c $(C_LINK)

