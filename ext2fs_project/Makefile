# Makefile

NAME = my_ext2fs

DISK 	    = $(DISK_DIR)/test_disk
NUM_BLOCKS  = 1440

INPUT_FILE  = $(TEST_DIR)/test.in
OUTPUT_FILE = $(TEST_DIR)/test.out

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
# 'make mem_run'	runs the program through Valgrind to check for memory leaks
# 
# 'make test'	    tests the program	
# 
# 'make mem_test'	tests the program through Valgrind to check for memory leaks
#
# 'make disk'		creates a fresh disk
#
# 'make clean'		removes all object files, temp files, and the executable
#
# 'make tar'		adds everything needed to recreate executable into a tar and compresses
#
# 'make submit'		uploads tar to KC's server
#
##################################################


.PHONY : all run mem_run test mem_test disk clean tar submit


###### Directories ######
CUR_DIR  = .
UTIL_DIR = util
CMD_DIR  = cmd
CMD1_DIR = cmd/level_1
CMD2_DIR = cmd/level_2
CMD3_DIR = cmd/level_3
OBJ_DIR  = obj
DISK_DIR = disk
TEST_DIR = test

# -Idir
# Add the directory dir to the head of the list of directories to be searched for header files. 
# This can be used to override a system header file, substituting your own version, 
# since these directories are searched before the system header file directories. 
INCLUDES = -I $(CUR_DIR)  \
		   -I $(UTIL_DIR) \
		   -I $(CMD_DIR)  \

# VPATH specifies a list of directories that make should search
VPATH = $(UTIL_DIR) $(CMD_DIR) $(TEST_DIR)


CC = gcc
CFLAGS = -std=c99 $(INCLUDES) $(DEBUG)
# The c99 standard allows variables to be declared anywhere
# i.e, for(int i = 0; ...)
# Making for must cleaner and easier to understand code.

OBJECTS = main.o input.o memory.o print.o search.o transfer.o util.o file.o cmd.o \
		  level1_test.o level2_test.o level3_test.o


#################### DEPENDENCIES #################### 

all: $(OBJ_DIR) $(NAME) 

$(OBJ_DIR):
	@mkdir -p $@

$(NAME): $(addprefix $(OBJ_DIR)/, $(OBJECTS))
	$(CC) $(CFLAGS) $^ -o $@ -lm

$(OBJ_DIR)/main.o     : main.c init.c $(TEST_DIR)/test.c \
						input.h transfer.h memory.h search.h print.h cmd.h global.h
	$(CC) -c $(CFLAGS) -I $(TEST_DIR) $< -o $@

$(OBJ_DIR)/input.o    : input.c input.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/memory.o   : memory.c memory.h \
						util.h transfer.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/print.o    : print.c print.h \
						util.h transfer.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/search.o   : search.c search.h \
						input.h transfer.h memory.h print.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/transfer.o : transfer.c transfer.h \
						global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/util.o     : util.c util.h \
						transfer.h memory.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/file.o     : file.c file.h \
						transfer.h search.h global.h
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/cmd.o      : cmd.c cmd.h $(CMD1_DIR)/*.c $(CMD2_DIR)/*.c $(CMD3_DIR)/*.c \
						print.h util.h search.h file.h global.h
	$(CC) -c $(CFLAGS) -I $(CMD1_DIR) -I $(CMD2_DIR) -I $(CMD3_DIR) $< -o $@


#################### TESTING #################### 

$(OBJ_DIR)/level1_test.o : $(TEST_DIR)/level_1.c $(TEST_DIR)/level_1.h 
	$(CC) -c $(CFLAGS) -I $(TEST_DIR) $< -o $@

$(OBJ_DIR)/level2_test.o : $(TEST_DIR)/level_2.c $(TEST_DIR)/level_2.h \
						   file.h print.h cmd.h global.h
	$(CC) -c $(CFLAGS) -I $(TEST_DIR) $< -o $@

$(OBJ_DIR)/level3_test.o : $(TEST_DIR)/level_3.c $(TEST_DIR)/level_3.h
	$(CC) -c $(CFLAGS) -I $(TEST_DIR) $< -o $@


#################### COMMANDS #################### 

run: all
	$(NAME) $(DISK)

mem_run: all
	valgrind $(VALGRIND_MODE) $(NAME) $(DISK) 

test: all disk
	-$(NAME) $(DISK) $(OUTPUT_FILE) #$(INPUT_FILE) > $(OUTPUT_FILE); 2>&1
	vim -O $(OUTPUT_FILE) #$(INPUT_FILE) 

mem_test: all disk
	valgrind $(VALGRIND_MODE) $(NAME) $(DISK) $(INPUT_FILE) 

disk:
	touch $(DISK)
	sudo mkfs.ext2 -F $(DISK) $(NUM_BLOCKS) 

clean:
	@$(RM) $(NAME) $(OBJ_DIR)/*.o *~

tar: all clean
	tar -zcf $(NAME).tar.gz *.c *.h $(CMD_DIR) $(TEST_DIR) $(UTIL_DIR) README.md makefile 

submit: tar 
	script/upload.sh $(LAB) $(NAME).tar.gz
	@$(RM) $(NAME).tar.gz
