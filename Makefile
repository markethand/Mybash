all: list.o tree.o exec.o main4.o 
	gcc main4.o list.o tree.o exec.o -o mybash  

exec.o: exec.c exec.h
	gcc -c exec.c

tree.o: tree.c tree.h
	gcc -c tree.c

list.o: list.c list.h
	gcc -c list.c

main4.o: main4.c 
	gcc -c main4.c







