# makefile for librand

SRCS=randbyte.c unix_truerand.c nt_truerand.c qshs.c
OBJS=randbyte.o unix_truerand.o qshs.o
CC=gcc
CFLAGS=
# No -O in CFLAGS! On some compilers, this optimizes out the counter...

librand.a: $(OBJS)
	ar rcv librand.a $(OBJS)
	ranlib librand.a

librand.shar: README makefile $(SRCS) 
	shar README makefile $(SRCS) > librand.shar

clean:
	rm -f $(OBJS) librand.a
