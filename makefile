#Ghost Shell
all: gsh

gsh: main.c
	gcc -g main.c process_manager.c -o gsh


