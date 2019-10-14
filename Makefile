#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG1 = pgmfilter

all : $(PROG1)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g

#pxmtopxm : pxmtopxm.o Util.o

pgmfilter : pgmfilter.o Util.o -lm

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

