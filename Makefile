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
LDFLAGS   =  -g -lm

#pxmtopxm : pxmtopxm.o Util.o

pgmfilter : pgmfilter.o Util.o

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

