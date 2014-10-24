NAME = showblock
DISK = fdimage
FILE = /Z/hugefile

FILES = main.c \
		input.c    input.h    \
		util.c     util.h     \
		print.c    print.h	  \
		search.c   search.h   \
		memory.c   memory.h   \
		transfer.c transfer.h \

OBJS = main.o \
	   input.o util.o print.o \
	   search.o memory.o transfer.o 

CC = gcc
DEBUG = -Wall -g -O0
FLAGS = $(DEBUG)

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS)

main.o     : util.h search.h print.h
print.o    : util.h transfer.h
search.o   : util.h input.h transfer.h memory.h print.h
memory.o   : util.h transfer.h
transfer.o : util.h

shiny : run clean

clean :
	-rm $(OBJS)

tar :
	tar cfv $(NAME).tar $(FILES)
