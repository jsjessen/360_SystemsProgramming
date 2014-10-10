NAME  = myClient myServer 
FILES = client.c \
		server.c \
		input.c input.h \
		parse.c parse.h \
		cmd.c 	cmd.h \
		util.c  util.h
OBJS  = input.o parse.o cmd.o util.o
CC    = gcc
#FLAGS =
DEBUG = -g -Wall -Wextra \
		-Wstrict-prototypes \
		-Wwrite-strings \
		-Wunreachable-code \
		-O0

client: $(OBJS) client.o
	$(CC) -o myClient client.o $(OBJS) $(FLAGS) $(DEBUG) 

server: $(OBJS) server.o 
	$(CC) -o myServer server.o $(OBJS) $(FLAGS) $(DEBUG) 

client.o: input.h parse.h cmd.h util.h
server.o: input.h parse.h cmd.h util.h

clean :
	-rm $(OBJS) client.o server.o

tar :
	tar cfv $(NAME).tar $(FILES)
