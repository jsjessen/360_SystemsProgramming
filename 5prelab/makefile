FILES = main.c \
		super.c gd.c bmap.c imap.c inode.c dir.c \
		print_ext2.c print_ext2.h \
		util_ext2.c util_ext2.h

OBJS = main.o \
	   super.o gd.o bmap.o imap.o inode.o dir.o \
	   print_ext2.o \
	   util_ext2.o

SHARED = print_ext2.o util_ext2.o

CC = gcc
FLAGS = -Wall -g -O0

run : $(OBJS)
	$(CC) -o print_disk main.o $(SHARED) $(FLAGS)
	$(CC) -o super super.o $(SHARED) $(FLAGS)
	$(CC) -o gd    gd.o    $(SHARED) $(FLAGS)
	$(CC) -o bmap  bmap.o  $(SHARED) $(FLAGS)
	$(CC) -o imap  imap.o  $(SHARED) $(FLAGS)
	$(CC) -o inode inode.o $(SHARED) $(FLAGS)
	$(CC) -o dir   dir.o   $(SHARED) $(FLAGS)

main.o  : print_ext2.h util_ext2.h
super.o : print_ext2.h util_ext2.h
gd.o    : print_ext2.h util_ext2.h
bmap.o  : print_ext2.h util_ext2.h
imap.o  : print_ext2.h util_ext2.h
inode.o : print_ext2.h util_ext2.h
dir.o   : print_ext2.h util_ext2.h

clean :
	-rm $(OBJS)
