# Makefile

NAME = my_ext2fs

LAB  = LAB8

DISK = $(DISK_DIR)/make_disk 
NUM_BLOCKS = 1440

VALGRIND_MODE = --leak-check=full

DEBUG = -g -Wall -O0
# -g 		adds debugging info
# -Wall		enables most compiler warnings
# -O0		reduces compilation time and makes debugging produce the expected results
  
##################################################
#
# 'make'			compiles everything
#
# 'make run'		runs the program	
#
# 'make check_mem'	runs the program with Valgrind to check for memory leaks
#
# 'make disk'		creates a fresh disk
#
# 'make clean'		removes all object files, ~files, and the executable
#
# 'make tar'		adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'		uploads tar to KC's server into directory defined by LAB 
#
##################################################


.PHONY : all run check_mem disk clean tar submit


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
INCLUDES = -I $(CUR_DIR)  \
		   -I $(UTIL_DIR) \
		   -I $(CMD_DIR)  \

# VPATH specifies a list of directories that make should search
VPATH = $(UTIL_DIR) $(CMD_DIR)


CC = gcc
CFLAGS = -std=c99 $(INCLUDES) $(DEBUG)
# The c99 standard allows variables to be declared anywhere
# i.e, for(int i = 0; ...)
# Making for must cleaner and easier to understand code.

OBJECTS = main.o input.o memory.o print.o search.o transfer.o util.o cmd.o


#################### DEPENDENCIES #################### 

all: $(OBJ_DIR) $(NAME) 

$(NAME): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/main.o     : main.c init.c input.h transfer.h memory.h search.h print.h cmd.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/input.o    : input.c input.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/memory.o   : memory.c memory.h util.h transfer.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/print.o    : print.c print.h util.h transfer.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/search.o   : search.c search.h input.h transfer.h memory.h print.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/transfer.o : transfer.c transfer.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/util.o     : util.c util.h transfer.h memory.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/cmd.o      : cmd.c cmd.h $(CMD1_DIR)/*.c $(CMD2_DIR)/*.c $(CMD3_DIR)/*.c global.h
	$(CC) -c $(CFLAGS) -I $(CMD1_DIR) -I $(CMD2_DIR) -I $(CMD3_DIR) $< -o $@
		   
$(OBJ_DIR):
	@mkdir -p $@


#################### COMMANDS #################### 

run:
	$(NAME) $(DISK)

check_mem:
	valgrind $(VALGRIND_MODE) $(NAME) $(DISK)

disk:
	touch $(DISK)
	sudo mkfs.ext2 -F $(DISK) $(NUM_BLOCKS) 

clean:
	@$(RM) *~ $(NAME) $(OBJ_DIR)/*.o 

tar:
	tar -zcf $(NAME).tar.gz $(UTIL_DIR) $(CMD_DIR) *.c *.h makefile 

submit: tar 
	script/upload.sh $(LAB) $(NAME).tar.gz
	@$(RM) $(NAME).tar.gz
