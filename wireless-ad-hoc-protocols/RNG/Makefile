C_LINK = -lpthread -lsocket -lnsl -lm

CC = gcc

all:    RNG_A 
	@echo "Make Successful"
	@echo "*********************************************************"
	@echo "usage: ./RNG_A <seed> <maximum delay> <source name>"
	@echo "*********************************************************"

clean:
	rm -f *.o

RNG_A:  RNG_A.c

RNG_A:
	$(CC) -o RNG_A RNG_A.c $(C_LINK)
