OBJS = print.c print.h t.c s.s
CC = cc
DEBUG = -g
CFLAGS = 
LFLAGS = -m32

run : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) $(DEBUG)
	./a.out

clean:
	\rm *.o *~ run
