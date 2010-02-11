/*!
*	@file 	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <GL/glut.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>

#include "v3ctor.cpp"
#include "mesh.h"

using namespace std;

/*!
*	Loads a mesh from a .PLY file.
*
*	@param	filename Mesh filename
*	@return	true if the mesh could be loaded, else false
*/

bool mesh::load_ply(const char* filename)
{
	ifstream in(filename);
	if(in.bad())
		return(false);

	string data;
	
	// Read the headers: Only ASCII format is accepted, but the version is
	// ignored

	cout << "Parsing PLY header...\n";
		
	getline(in, data);
	if(data != "ply")
	{
		cerr << "Error: \"" << filename << "\" is not a PLY file.\n";
		return(false);
	}

	getline(in, data);
	if(data.find("format ascii") == string::npos)
	{
		cerr << data;
		cerr << "Error: \"" << filename << "\" is not an ASCII PLY file.\n";
		return(false);
	}

	/*
		Parsing further element properties is quick and dirty: It is
		assumed that face data is declared _after_ the vertex data.
		Properties are assumed to come in the natural order, i.e.:

			x
			y
			z

		for vertex data.
	*/

	size_t num_vertices 	= 0;
	size_t num_faces 	= 0;

	const short MODE_PARSE_HEADER			= 0;
	const short MODE_PARSE_VERTEX_PROPERTIES 	= 1;
	const short MODE_PARSE_FACE_PROPERTIES	 	= 2;
	
	short mode = MODE_PARSE_HEADER;
	while(!in.eof()) 
	{
		getline(in, data);
		
		/*
			Lines contaning "comment" or "obj_info" are skipped.
			Not sure whether obj_info is allowed to appear at all.
		*/
		if(	data.find("comment")  != string::npos ||
			data.find("obj_info") != string::npos)
			continue;
		else if(data.find("end_header") != string::npos)
		{
			cout << "...finished parsing.\n";
			break;
		}

		switch(mode)
		{
			case MODE_PARSE_VERTEX_PROPERTIES:

				if(data.find("property") != string::npos)
				{
					/*
						Ignore. Some special handlings
						for more properties could be
						added here.
					*/

					continue;
				}
				else if(data.find("element face") != string::npos)
				{
					mode = MODE_PARSE_FACE_PROPERTIES;

					string dummy; // not necessary, but more readable
					istringstream converter(data);
					converter >> dummy >> dummy >> num_faces;

					if(num_faces == 0)
					{
						cerr << "Unable to read number of faces from PLY file.\n";
						return(false);
					}

					cout << "* Number of faces: " << num_faces << "\n"; 

					mode = MODE_PARSE_FACE_PROPERTIES;
				}
				else
				{
					cerr << "Error: Got \"" << data << "\", expected \"property\"\n";
					return(false);
				}
				
				break;
			
			case MODE_PARSE_FACE_PROPERTIES:

				if(data.find("property list") == string::npos)
				{
					cerr << "Warning: Got \"" << data << "\". "
					<< "This property is unknown and might lead "
					<< "to problems when parsing the file.\n";
				}

				break;

			// Expect "element vertex" line
			case MODE_PARSE_HEADER:

				if(data.find("element vertex") != string::npos)
				{
					mode = MODE_PARSE_VERTEX_PROPERTIES;

					string dummy; // not necessary, but more readable
					istringstream converter(data);
					converter >> dummy >> dummy >> num_vertices;

					if(num_vertices == 0)
					{
						cerr << "Unable to read number of vertices from PLY file.\n";
						return(false);
					}

					cout << "* Number of vertices: " << num_vertices << "\n";
					
					mode = MODE_PARSE_VERTEX_PROPERTIES;
				}
				else
				{
					cerr 	<< "Got \"" << data 
						<< "\", but expected \"element vertex\" "
						<< "or \"element face\"\n";
					return(false);
				}
				
				break;

			default:
				break;
		}
	}

	cout << "Reading vertex and edge data...\n";
	
	clock_t start = clock();

	size_t line 	= 0;
	size_t k 	= 0; // number of vertices for face

	double x, y, z;
	while(!in.eof())
	{
		if(line < num_vertices)
		{
			in >> x >> y >> z;
			add_vertex(x, y, z);
		}
		else
		{
			k = 0;
			in >> k;
			if(k == 0)
				break;

			// Store vertices of face in proper order and add a new
			// face.
			
			vector<size_t> vertices;
			size_t v = 0;
			for(size_t i = 0; i < k; i++)
			{
				in >> v;
				vertices.push_back(v);
			}

			add_face(vertices);
		}

		line++;
	}

	clock_t end = clock();
	
	cout << "...finished in " << (end-start)/static_cast<double>(CLOCKS_PER_SEC) << "s\n";
	return(true);
}

/*!
*	Saves the currently loaded mesh to a .PLY file.
*
*	@param	filename File for storing the mesh
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_ply(const char* filename)
{
	return(true);
}

/*!
* Draws the mesh using standard OpenGL drawing routines.
*/

void mesh::draw()
{
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for(size_t i = 0; i < V.size(); i++)
		glVertex3f(V[i].p[0], V[i].p[1], V[i].p[2]);
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		edge& e = edge_table.get(i);
		vertex& v1 = get_vertex(e.u);
		vertex& v2 = get_vertex(e.v);

		glVertex3f(v1.p[0], v1.p[1], v1.p[2]);
		glVertex3f(v2.p[0], v2.p[1], v2.p[2]);
	}
	glEnd();
}

void mesh::add_face(vector<size_t> vertices)
{
	size_t u = 0;
	size_t v = 0;
	size_t face_index = F.size()+1;

	if(vertices.size() == 0)
		return;

	face f;
	edge e;

	u = vertices[0];
	for(size_t i = 1; i <= vertices.size(); i++)
	{
		// Handle last vertex; should be the edge v--u
		if(i == vertices.size())
		{
			u = vertices[i-1];
			v = vertices[0];
		}
		// Normal case
		else
			v = vertices[i];

		e.u = u;
		e.v = v;

		// Add it to list of edges for face
		edge_query result = edge_table.add(e);
		f.add_edge(result);

		// Edge already known; update second adjacent face
		if(result.inverted)
			face_table.set_f2(result.e, face_index);
		// New edge; update first adjacent face
		else
			face_table.set_f1(result.e, face_index);

		// Set next start vertex; the orientation should be correct
		// here
		u = v;
	}

	F.push_back(f);
}

vertex& mesh::get_vertex(size_t v)
{
	return(V[v]);
}

edge& mesh::get_edge(size_t e)
{
	return(edge_table.get(e));
}



void mesh::add_vertex(double x, double y, double z)
{
	vertex v;
	v.p[0] = x;
	v.p[1] = y;
	v.p[2] = z;
	
	V.push_back(v);
}
