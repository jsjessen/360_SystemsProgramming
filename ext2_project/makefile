
TARGET = myext2 

#
# 'make'		build and execute
#
# 'make clean'	remove all .o files
#
# 'make tar'	adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'	recompiles everything, cleans dir, makes tar, uploads
#

DEBUG  = -g -Wall -O0
# -g 		add debugging info
# -Wall		enable most compiler warnings
# -O0		reduce compilation time and make debugging produce the expected results

###### Libraries ######
LIBS     = #-lmylib -lm
#   if I want to link in libraries (libx.so or libx.a) I use the -llibname 
#   option, something like (this will link in libmylib.so and libm.so:

###### Directories ######
CMD_DIR  = ../cmd
CMD1_DIR = ../cmd/level_1
CMD2_DIR = ../cmd/level_2
CMD3_DIR = ../cmd/level_3
UTIL_DIR = ../util
OBJ_DIR  = ../obj
DISK_DIR = ../disk

INCLUDES = -I$(CMD1_DIR) \
		   -I$(CMD2_DIR) \
		   -I$(CMD3_DIR) \
		   -I$(UTIL_DIR) \
		   -I$(OBJ_DIR)  \
		   -I$(DISK_DIR) \

#OBJECTS = main.o \
	#	   input.o util.o print.o \
	#	   search.o memory.o transfer.o \
	#	   cmd.o
#
#run : $(OBJECTS)
#	$(CC) -o $(TARGET) $(OBJECTS) $(CFLAGS)
#
#
#main.o     : global.h search.h print.h cmd.h
#util.o	   : global.h
#print.o    : global.h transfer.h
#search.o   : global.h input.h transfer.h memory.h print.h
#memory.o   : global.h util.h transfer.h
#transfer.o : global.h
#
#cmd.o 	   : global.h print.h cmd.c level1.c level2.c level3.c
#	$(CC) -o $@ cmd.c level1.c level2.c level3.c $(CFLAGS)

SOURCES = main.c \
		  input.c print.c util.c memory.c transfer.c search.c \
		  menu.c cmd.c sync.c quit.c \
		  list_dir.c change_dir.c pwd.c mkdir.c rmdir.c creat.c \
		  link.c unlink.c symlink.c rm.c chmod.c chown.c stat.c touch.c \
		  open.c close.c pfd.c lseek.c access.c read.c write.c cat.c cp.c mv.c \
		  mount.c umount.c cs.c 

OBJECTS = $(SOURCES:.c=.o)

CC     = gcc
CFLAGS = $(DEBUG)
LFLAGS = #-L../lib include libraries not in /usr/lib


.PHONY : default clean tar submit depend

default: $(TARGET)
	@echo Compiled: $(TARGET)

$(TARGET): $(OBJECTS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJECTS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

tar :
	tar -zcfv $(TARGET).tar.gz *.c *.h makefile

submit : run clean tar
	ul_ssh $(TARGET).tar.gz

depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

