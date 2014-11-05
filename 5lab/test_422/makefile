NAME = test 

FILES = test.c

OBJS = test.o

CC = gcc
DEBUG = -Wall -g -O0
FLAGS = $(DEBUG)

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS)

shiny : run clean

clean :
	-rm $(OBJS)

tar :
	tar cfv $(NAME).tar $(FILES) makefile
