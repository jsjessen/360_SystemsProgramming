NAME = print_disk 

FILES = main.c util.h \
		super.c gd.c bmap.c imap.c inode.c dir.c \
		get_block.c

OBJS = main.o \
	   super.o gd.o bmap.o imap.o inode.o dir.o \
	   get_block.o

CC = gcc
#DEBUG = -Wall -g -O0

run : $(OBJS)
	$(CC) -o $(NAME) $(OBJS) $(FLAGS) $(DEBUG) 

clean :
	-rm $(OBJS)

tar :
	tar cfv $(NAME).tar $(FILES)
