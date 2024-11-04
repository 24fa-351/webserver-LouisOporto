all: build link

build:
	-c -o webserver.o webserver.c

link:
	-c -o webserver webserver.o