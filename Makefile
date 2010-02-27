CC          = g++
CCFLAGS     = -g -Wall -c
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     = -lglut -lGL -lGLU 

MYOBJECTS   = subdivision.o v3ctor.o mesh.o face.o vertex.o edge.o t_edge_hash.o zpr.o
BIN         = subdivision

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS) -o $(BIN)
	./$(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o *.core $(BIN)
