
TARGET = my_ext2fs

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

###### Files ######
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

# Build .o's from .c's
# $<: name of the prerequisite of the rule(.c file) 
# $@: name of the target of the rule (.o file) 
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(TARGET)

tar :
	tar -zcfv $(TARGET).tar.gz *.c *.h makefile

submit : run clean tar
	./script/upload.sh $(TARGET).tar.gz

# The makedepend program reads each sourcefile in sequence and parses it like a C-preprocessor, 
# processing all #include, #define, #undef, #ifdef, #ifndef, #endif, #if, #elif and #else directives 
# so that it can correctly tell which #include, directives would be used in a compilation. 
# Any #include, directives can reference files having other #include directives, 
# and parsing will occur in these files as well.  
depend: $(SOURCES)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

