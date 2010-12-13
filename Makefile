CC          = g++
CCFLAGS     = -g -Wall -c -pedantic -O11 -DNO_EXCEPTIONS
LIBS        = -L /usr/X11/lib -L /usr/local/lib
INCLUDES    = -I /usr/local/include
LDFLAGS     =
MYOBJECTS   = psalm.o v3ctor.o mesh.o face.o vertex.o edge.o directed_edge.o
LIBOBJECTS  = libpsalm.o v3ctor.o mesh.o face.o vertex.o edge.o directed_edge.o hole.o
BIN         = psalm

# Path to "meshlabserver" binary, which is used for creating test data.
MLS 	    = /home/bastian/tmp/meshlab-snapshot-svn3524/meshlab/src/meshlabserver/meshlabserver

$(BIN): $(MYOBJECTS) Makefile
	$(CC) $(MYOBJECTS) $(LIBS) $(LDFLAGS) -o $(BIN)

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) $<

# Create libpsalm
libpsalm: $(LIBOBJECTS) Makefile
	ar rcs libpsalm.a $(LIBOBJECTS)

clean:
	rm -f *.o *.core *.html $(BIN)
	rm -rf Doxygen
	echo "Removing test data..."
	rm -f Meshes/ICO_CC_*.*
	rm -f Meshes/ICO_DS_*.*

# Create subdivided test data using "meshlab"
test: $(BIN)
	echo "Catmull-Clark Subdivision..."
	./$(BIN) -a cc -n 1 -o Meshes/ICO_CC_01.ply Meshes/Icosahedron.ply
	./$(BIN) -a cc -n 2 -o Meshes/ICO_CC_02.ply Meshes/Icosahedron.ply
	./$(BIN) -a cc -n 3 -o Meshes/ICO_CC_03.ply Meshes/Icosahedron.ply
	echo "Doo-Sabin Subdivision..."
	./$(BIN) -a ds -n 1 -o Meshes/ICO_DS_01.ply Meshes/Icosahedron.ply
	./$(BIN) -a ds -n 2 -o Meshes/ICO_DS_02.ply Meshes/Icosahedron.ply
	./$(BIN) -a ds -n 3 -o Meshes/ICO_DS_03.ply Meshes/Icosahedron.ply
	echo "Converting..."
	$(MLS) -i Meshes/ICO_CC_01.ply -o Meshes/ICO_CC_01.stl
	$(MLS) -i Meshes/ICO_CC_02.ply -o Meshes/ICO_CC_02.stl
	$(MLS) -i Meshes/ICO_CC_03.ply -o Meshes/ICO_CC_03.stl
	$(MLS) -i Meshes/ICO_DS_01.ply -o Meshes/ICO_DS_01.stl
	$(MLS) -i Meshes/ICO_DS_02.ply -o Meshes/ICO_DS_02.stl
	$(MLS) -i Meshes/ICO_DS_03.ply -o Meshes/ICO_DS_03.stl
	echo "Done."

# Valgrind test run for memory leaks
memcheck:
	valgrind --suppressions=psalm.supp --leak-check=full --show-reachable=yes --track-origins=yes ./psalm -n 2 Tetrahedron.ply
	rm Tetrahedron_subdivided.ply
