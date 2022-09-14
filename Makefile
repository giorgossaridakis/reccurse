#   Reccurse Makefile with Macros
#   - Author: Giorgos Saridakis

# definitions
CC      = $(shell which g++)
CFLAGS  = -Wall
LDLIBS  = -lncurses -lm
# list of sources
SOURCES = rcscr.cc rcutil.cc rcpc.cc rcfre.cc rcsc.cc rccompar.cc reccurse.cc
EXEC    = reccurse

#convert to a list of objects
OBJECTS = $(SOURCES:.cc=.o)

reccurse: $(OBJECTS)
	$(CC) -o$@ $(OBJECTS) $(CFLAGS) $(LDLIBS)

clean:
	rm -f $(OBJECTS)
