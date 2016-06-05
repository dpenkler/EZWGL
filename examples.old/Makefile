#----------------------------------------------------------------------------------
#
#      EZWGL Examples
#
#----------------------------------------------------------------------------------
#
# use gcc is highly recommended

INCLUDE = -I../include 

JUNK = 

LIBS = -L../lib   -L/usr/X11R6/lib -lEZ -lX11 -lXext -ljpeg -lpng -ltiff -lm  -lc

CC= gcc

CFLAG = -O2 $(INCLUDE)

EXAMPLES= Example1 Example2 Example3 Example4 Example5 Example6\
	  Example7 Example8 Example9 Example10 Example11 Example12S\
	  Example12T Example13S Example13T Example14 Example15\
	  Example16 ExampleTree ExampleFLB ExampleTimer ExampleInput\
          ExampleG1 ExampleG2 ExampleG3 ExampleG4 ExampleG Example\
        ExampleEHandler ExampleGGG

#----------------------------------------------------------------------------------

all: $(EXAMPLES)

strip:
	strip $(EXAMPLES)

clean:
	$(RM)  $(EXAMPLES)

../lib/libEZ.a:
	cd ../lib; make all

.c.o:
	$(CC) $(CFLAG) -c $<

.o:
	$(CC) $(CFLAG) $< $(LIBS) -o $@

.c:
	$(CC)  $(CFLAG)  $< -o $@ $(LIBS)

#----------------------------------------------------------------------------------



