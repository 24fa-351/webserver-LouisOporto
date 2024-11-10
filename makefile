all: build link

build:
	gcc -c webserver.c

link:
	gcc -o webserver webserver.o