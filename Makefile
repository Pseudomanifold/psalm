CC          = g++
CCFLAGS     = -g -Wall -O11 -c
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     = -lglut -lGL -lGLU 

MYOBJECTS   = subdivision.o mesh.o face.o vertex.o edge.o t_edge.o t_edge_hash.o t_face.o v3ctor.o
BIN         = subdivision

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS) -o $(BIN)
	./$(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

#.c.o:
#	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o *.core $(BIN)
