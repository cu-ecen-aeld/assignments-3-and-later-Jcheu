CC=$(CROSS_COMPILE)gcc
CFLAGS = -g -Wall #-Werror

all: aesdsocket

aesdsocket:
	$(CC)  $(CFLAGS) aesdsocket.c -o aesdsocket
	
clean:
	rm -f aesdsocket
	rm -f aesdsocket.o
