CC            = gcc
DEFINES       =
CFLAGS        = -O2 -std=gnu99 -Wall $(DEFINES)
INCPATH       = 
LINK          = gcc
LFLAGS        = 
LIBS          =


loader: loader.o
	$(LINK) $(LFLAGS) -o loader loader.o $(LIBS)

loader.o: loader.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o loader.o loader.c

clean:
	rm -rf *.o
