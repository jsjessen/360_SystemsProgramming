
PROG = my_ext2fs

LAB  = LAB7

#
# 'make'		compiles everything
#
# 'make clean'	removes all object files
#
# 'make tar'	adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'	uploads tar to KC's server into directory defined by LAB 
#

.PHONY : all clean tar submit

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
CFLAGS = -std=c99 $(INCLUDES) $(DEBUG)
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

# TEMP HACK 
# Research: http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
#all : $(OBJPROG)
all : clean $(OBJPROG)

$(OBJECTS) : | $(OBJ_DIR)

$(OBJ_DIR) :
	@mkdir -p $@

###### Dependencies ######
$(OBJPROG) : $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ $(LFLAGS) $(LIBS) -o $(PROG)

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	@$(RM) -f $(PROG) $(PROG).tar.gz $(OBJ_DIR)/*.o *~ 

tar :
	tar -zcf $(PROG).tar.gz $(UTIL_DIR) $(CMD_DIR) *.c *.h makefile 

submit : tar 
	script/upload.sh $(LAB) $(PROG).tar.gz
	@$(RM) -f $(PROG).tar.gz
