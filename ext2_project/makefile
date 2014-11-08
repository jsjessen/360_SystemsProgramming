
PROG = my_ext2fs

#
# 'make'		build and execute
#
# 'make clean'	remove all .o files
#
# 'make tar'	adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'	uploads tar
#

.PHONY : all clean tar submit depend

DEBUG    = -g -Wall -O0
# -g 		add debugging info
# -Wall		enable most compiler warnings
# -O0		reduce compilation time and make debugging produce the expected results

###### Libraries ######
LIBS     = #-lmylib -lm

CC     = gcc
CFLAGS = $(INCLUDES) $(DEBUG)
LFLAGS = #-L../lib include libraries not in /usr/lib

###### Directories ######
CUR_DIR  = .
UTIL_DIR = util
CMD_DIR  = cmd
CMD1_DIR = cmd/level_1
CMD2_DIR = cmd/level_2
CMD3_DIR = cmd/level_3
OBJ_DIR  = obj
DISK_DIR = disk

INCLUDES = -I$(CUR_DIR)  \
		   -I$(UTIL_DIR) \
		   -I$(CMD_DIR)  \
		   -I$(CMD1_DIR) \
		   -I$(CMD2_DIR) \
		   -I$(CMD3_DIR) \

# VPATH specifies a list of directories that make should search
VPATH = $(UTIL_DIR) $(CMD_DIR) $(OBJ_DIR)

#vpath %.c $(CMD1_DIR) $(CMD2_DIR) $(CMD3_DIR)


SOURCE = main.c \
		 input.c print.c util.c memory.c transfer.c search.c \
		 cmd.c
		 #menu.c cmd.c quit.c \
		 #list_dir.c change_dir.c pwd.c mkdir.c rmdir.c creat.c \
		 #link.c unlink.c symlink.c rm.c chmod.c chown.c stat.c touch.c \
		 #open.c close.c pfd.c lseek.c access.c read.c write.c cat.c cp.c mv.c \
		 #mount.c umount.c cs.c sync.c

OBJECTS = $(SOURCE:.c=.o)

OBJPROG = $(addprefix $(OBJ_DIR)/, $(PROG))

all : $(OBJPROG)

$(OBJECTS) : | $(OBJ_DIR)

$(OBJ_DIR) :
	@mkdir -p $@

$(OBJPROG) : $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(LFLAGS) $(LIBS) -o $(PROG)

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	@$(RM) -f  $(PROG) $(OBJ_DIR)/*.o *~ 

tar :
	tar -zcfv $(PROG).tar.gz $(UTIL_DIR) $(CMD_DIR) *.c *.h makefile 

submit : tar
	script/upload.sh $(PROG).tar.gz

# The makedepend program reads each sourcefile in sequence and parses it like a C-preprocessor, 
# processing all #include, #define, #undef, #ifdef, #ifndef, #endif, #if, #elif and #else directives 
# so that it can correctly tell which #include, directives would be used in a compilation. 
# Any #include, directives can reference files having other #include directives, 
# and parsing will occur in these files as well.  
depend : %.c 
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it

