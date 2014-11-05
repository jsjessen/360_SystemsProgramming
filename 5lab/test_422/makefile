NAME = test 

FILES = test.c \
		compare.c compare.h \
		input.c input.h \
		fdimage makefile \
		bug_showblock \
		kc_showblock.bin

OBJS = test.o compare.o input.o

CC = gcc
DEBUG = -Wall -g -O0
FLAGS = $(DEBUG)

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS)

test.o    : compare.h
compare.o : input.h

shiny : run clean tar

clean :
	-rm -f $(OBJS)

tar : clean
	tar -zcf showblock_test.tar.gz $(FILES) 
