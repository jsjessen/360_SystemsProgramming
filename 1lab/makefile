NAME = lab
FILES = main.c input.h input.c parse.h \
		parse.c cmd.h cmd.c tree.h tree.c
OBJS = main.o input.o parse.o cmd.o tree.o 
CC = gcc
DEBUG = -Wall
FLAGS =

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS) $(DEBUG) 

main.o : input.h parse.h cmd.h tree.h
cmd.o  : tree.h

clean :
	-rm $(NAME) $(OBJS)

tar :
	tar cfv $(NAME).tar $(FILES)
