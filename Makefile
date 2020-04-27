all : main.c scheduler.c process.h
	gcc main.c scheduler.c -o main
