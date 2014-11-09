
PROG = my_ext2fs

LAB  = LAB6

#
# 'make'		compiles everything
#
# 'make clean'	removes all object files
#
# 'make tar'	adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'	uploads tar to KC's server into directory defined by LAB 
#

.PHONY : all clean tar submit depend

# Add any additional utility files here
SOURCE = main.c cmd.c \
		 util.c input.c print.c memory.c transfer.c search.c
	
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

# -Idir
# Add the directory dir to the head of the list of directories to be searched for header files. 
# This can be used to override a system header file, substituting your own version, 
# since these directories are searched before the system header file directories. 
INCLUDES = -I$(CUR_DIR)  \
		   -I$(UTIL_DIR) \
		   -I$(CMD_DIR)  \
		   -I$(CMD1_DIR) \
		   -I$(CMD2_DIR) \
		   -I$(CMD3_DIR) \

# VPATH specifies a list of directories that make should search
VPATH = $(UTIL_DIR) $(CMD_DIR) $(OBJ_DIR)

OBJECTS = $(SOURCE:.c=.o)

OBJPROG = $(addprefix $(OBJ_DIR)/, $(PROG))

# make
all : clean $(OBJPROG)

$(OBJECTS) : | $(OBJ_DIR)

$(OBJ_DIR) :
	@mkdir -p $@

###### Dependencies ######
$(OBJPROG) : $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(LFLAGS) $(LIBS) -o $(PROG)

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

#$(OBJ_DIR)/input.o	  : 
#$(OBJ_DIR)/util.o	  : global.h
#$(OBJ_DIR)/transfer.o : global.h
#$(OBJ_DIR)/print.o	  : global.h util.h transfer.h
#$(OBJ_DIR)/memory.o	  : global.h util.h transfer.h
#$(OBJ_DIR)/main.o	  : global.h input.h print.h memory.h cmd.h
#$(OBJ_DIR)/search.o	  : global.h input.h transfer.h memory.h print.h
#$(OBJ_DIR)/cmd.o	  : global.h print.h util.h search.h $(CMD1_DIR)/%.c $(CMD1_DIR)/%.c $(CMD1_DIR)/%.c


clean :
	@$(RM) -f $(PROG) $(OBJ_DIR)/*.o *~ 

tar :
	tar -zcfv $(PROG).tar.gz $(UTIL_DIR) $(CMD_DIR) *.c *.h makefile 

submit : tar
	script/upload.sh $(LAB) $(PROG).tar.gz

# The makedepend program reads each sourcefile in sequence and parses it like a C-preprocessor, 
# processing all #include, #define, #undef, #ifdef, #ifndef, #endif, #if, #elif and #else directives 
# so that it can correctly tell which #include, directives would be used in a compilation. 
# Any #include, directives can reference files having other #include directives, 
# and parsing will occur in these files as well.  

# Not working

#depend : 
#makedepend -- $(CFLAGS) -I/usr/include/linux/ -I/usr/lib/gcc/i686-redhat-linux/4.8.3/include/ -- $(SOURCE)

# DO NOT DELETE THIS LINE -- make depend needs it
