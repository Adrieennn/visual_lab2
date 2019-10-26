#######################################
# Makefile PBM                        #
#                                     #
# E.B.                                #
#######################################


PROG = main

all : $(PROG)

# Variables for file compilation
CC        =  gcc
CFLAGS    =  -g -Wall
CPPFLAGS  =  -DDEBUG
LDFLAGS   =  -g

#pxmtopxm : pxmtopxm.o Util.o

main : main.o proc.o Util.o -lm

clean :
	@rm -f *.o

cleanall : clean
	@rm -f $(PROG)

