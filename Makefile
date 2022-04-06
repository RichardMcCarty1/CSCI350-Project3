default: all

all: 
	gcc -pthread p3.c -o ccsh

clean: 
	rm -f ccsh
