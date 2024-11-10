all: build link

build:
	gcc -c webserver.c http_message.c

link:
	gcc -o webserver webserver.o http_message.o