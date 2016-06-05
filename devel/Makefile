
LIBS = -L../lib  -L/usr/X11R6/lib -lEZ -lX11 -lXext  -lm  -lc

CC= gcc 

CFLAG = -g $(INCLUDE)

OBJS = sigraph.o

#----------------------------------------------------------------------------

all: sigraph

sigraph:	$(OBJS)


.c.o:
	$(CC) $(CFLAG) -c $<

.o:
	$(CC) $(CFLAG) $<  -o $@  $(LIBS) 

.c:
	$(CC)  $(CFLAG)  $< -o $@  $(LIBS)

#----------------------------------------------------------------------------



