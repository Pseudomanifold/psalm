CC          = g++
CCFLAGS     = -g -Wall -c -pedantic
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     = -lglut -lGL -lGLU 
MYOBJECTS   = psalm.o v3ctor.o mesh.o face.o vertex.o edge.o zpr.o
BIN         = psalm

# Path to "meshlabserver" binary, which is used for creating test data.
MLS 	    = /home/bastian/tmp/meshlab-snapshot-svn3524/meshlab/src/meshlabserver/meshlabserver

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS) -o $(BIN)
	./$(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

.c.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

clean:
	rm -f *.o *.core $(BIN)

# Create subdivided test data using "meshlab"
test: $(BIN)
	echo "Catmull-Clark Subdivision..."
	./$(BIN) -a cc -n 1 -o ICO_CC_01.ply Icosahedron_Inverted.ply
	./$(BIN) -a cc -n 2 -o ICO_CC_02.ply Icosahedron_Inverted.ply
	./$(BIN) -a cc -n 3 -o ICO_CC_03.ply Icosahedron_Inverted.ply
	echo "Doo-Sabin Subdivision..."
	./$(BIN) -a ds -n 1 -o ICO_DS_01.ply Icosahedron_Inverted.ply
	./$(BIN) -a ds -n 2 -o ICO_DS_02.ply Icosahedron_Inverted.ply
	./$(BIN) -a ds -n 3 -o ICO_DS_03.ply Icosahedron_Inverted.ply
	echo "Converting..."
	$(MLS) -i ICO_CC_01.ply -o ICO_CC_01.stl
	$(MLS) -i ICO_CC_02.ply -o ICO_CC_02.stl
	$(MLS) -i ICO_CC_03.ply -o ICO_CC_03.stl
	$(MLS) -i ICO_DS_01.ply -o ICO_DS_01.stl
	$(MLS) -i ICO_DS_02.ply -o ICO_DS_02.stl
	$(MLS) -i ICO_DS_03.ply -o ICO_DS_03.stl
	echo "Done."
