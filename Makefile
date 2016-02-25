CC     = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall
HWK    = /c/cs223/Hwk3

Subst16: Subst16.o
	$(CC) $(CFLAGS) -o Subst16 Subst16.o getLine.c
