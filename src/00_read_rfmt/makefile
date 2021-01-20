#The recommended C compiler is gcc.
CC = gcc -ansi

#Edit LIBS and INCLUDE to the path to the NetCDF lib and include directories.

LIBS = -L/usr/local/lib -lnetcdf
INCLUDE = -I/usr/local/include/

CODE = $(filter %.c,$^)
CFLAGS= -m64 -o $@

all:cryosat20hz

cryosat20hz:cryosat20hz.c cryosat20hz.h
	$(CC) $(CODE) $(CFLAGS) $(INCLUDE) $(LIBS) -g -o cryosat20hz

clean:
	-rm -f *.o

distclean:
	-rm -f *.o cryosat20hz


