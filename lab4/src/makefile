CC=gcc 
CFLAGS=-I.

all: psum

psum: parallel_sum.c libpar.a
	gcc -Wall -Wextra -pthread parallel_sum.c -L. -lpar -o psum

libpar.a: par.o
	ar rcs libpar.a par.o

par.o: par.c par.h
	gcc -Wall -Wextra -pthread -c par.c -o par.o

clean:
	rm -f psum par.o libpar.a
