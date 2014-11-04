TARGET = myext2 
LAB = LAB6

LIBS   = 
CC     = gcc
DEBUG  = -O0

#OBJDIR = obj
INC1 = ../level_1
INC2 = ../level_2
INC3 = ../level_3
INCDIRS = -I$(INC1) -I$(INC2) -I$(INC3)

CFLAGS = -Wall -g $(INCDIRS) $(DEBUG)
LFLAGS = 

.PHONY : default all clean tar submit 

OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) \
	   $(patsubst %.c,%.o,$(wildcard $(INC1)/*.c)) \
	   $(patsubst %.c,%.o,$(wildcard $(INC2)/*.c)) \
	   $(patsubst %.c,%.o,$(wildcard $(INC3)/*.c)) \

#OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(wildcard *.c)) \
#		  $(patsubst $(INC1)/%.c,$(OBJDIR)/%.o,$(wildcard $(INC1)/*.c)) \
#		  $(patsubst $(INC2)/%.c,$(OBJDIR)/%.o,$(wildcard $(INC1)/*.c)) \
#		  $(patsubst $(INC3)/%.c,$(OBJDIR)/%.o,$(wildcard $(INC1)/*.c)) \

default: $(TARGET)
all: default

#$(OBJS): | $(OBJDIR)

#$(OBJDIR):
#	mkdir $(OBJDIR)

HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	    $(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJS)

$(TARGET): $(OBJS)
	    $(CC) $(OBJS) -Wall $(LIBS) -o $@

#OBJS = main.o \
#	   input.o util.o print.o \
#	   search.o memory.o transfer.o \
#	   cmd.o
#
#run : $(OBJS)
#	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)
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


clean :
	-rm -f *.o
	-rm -f *~

pristine : clean
	-rm $(TARGET)

tar :
	tar cfv $(TARGET).tar *.c *.h makefile

submit: run clean tar
	ul_ssh $(LAB) $(TARGET).tar

