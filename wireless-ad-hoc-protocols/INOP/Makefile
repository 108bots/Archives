C_LINK = -lpthread -lsocket -lnsl -lm

CC = gcc

all:    inop 
	@echo "Make Successful"
	@echo "*********************************************************"
	@echo "usage: ./inop <seed> <maximum delay> <source name>"
	@echo "*********************************************************"

clean:
	rm -f *.o

inop:  inop.c

inop:
	$(CC) -o inop inop.c $(C_LINK)
