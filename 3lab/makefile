NAME = mysh 
FILES = main.c \
		input.c input.h \
		parse.c parse.h \
		shell.c shell.h
OBJS = main.o input.o parse.o shell.o
CC = gcc
#DEBUG = -Wall -g -O0
FLAGS =

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS) $(DEBUG) 

main.o   : input.h shell.h
shell.o  : parse.h 

clean :
	-rm $(OBJS)

tar :
	tar cfv $(NAME).tar $(FILES)
