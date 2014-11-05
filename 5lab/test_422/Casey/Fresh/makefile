NAME = test 

FILES = test.c

OBJS = test.o

CC = gcc
DEBUG = -Wall -g -O0
FLAGS = $(DEBUG)

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS)

shiny : run clean tar

clean :
	-rm -f $(OBJS)

tar : clean
	tar -zcf showblock_test.tar.gz *
