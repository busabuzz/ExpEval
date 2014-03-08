# makefile, Gerard Renardel, 21 January 2014
# makefile for programs for expression trees

CC     = gcc
CFLAGS = -ansi -Wall -g

pref:	scanner.c recognizeExp.c evalExp.c prefixExp.c mainPref.c
	$(CC) $(CFLAGS) scanner.c recognizeExp.c evalExp.c prefixExp.c mainPref.c -lm -o $@

infix:	scanner.c recognizeExp.c evalExp.c infixExp.c mainInfix.c
	$(CC) $(CFLAGS) scanner.c recognizeExp.c evalExp.c infixExp.c mainInfix.c -lm -o $@
	
clean:	
	rm -f *~
	rm -f #*#
	rm -f *.o

