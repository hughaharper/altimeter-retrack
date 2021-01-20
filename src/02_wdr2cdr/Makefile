PROG = cdr_read
CSRCS =cdr_read.c die.c

FSRCS = 
CC = gcc

OBJS =  $(CSRCS:.c=.o) 

INC = -I.
#CLIBS = -L./ -lm
#FLIBS =
CFLAGS = -O2 -Bstatic -m64 
FFLAGS = -O2 -Bstatic -m64 

$(PROG): $(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(INC) $(CLIBS) 

.c.o:
	$(CC) $(CFLAGS) -c $*.c -o $*.o 

clean:
	-rm -f $(OBJS) tags core

#install:
#	mv $(PROG) ../../../bin
