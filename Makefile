#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = k-means

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g

#pxmtopxm : pxmtopxm.o Util.o

k-means : k-means.o proc.o Util.o -lm

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

