/*!
*	@file 	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <GL/glut.h>

#include <fstream>
#include <sstream>
#include <iomanip>

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

	// Read the headers; this is quick and dirty for now: All lines
	// are skipped until "end_header" appears. Afterwards, the
	// vertices followed by the faces will be read.
		
	string data;

	in >> data;
	if(data != "ply")
		return(false);
	
	size_t num_vertices 	= 12; // FIXME: This should be read from file.
	size_t num_faces 	= 20;

	while(in.good() && data != "end_header")
		in >> data;

	cout 	<< num_vertices << " vertices read\n"
		<< num_faces	<< " faces read\n";
	
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
			edge e;
			face f;

			if(k == 0)
				break;

			size_t v = 0;

			// Store vertices of face in proper order and add a new
			// face.
			vector<size_t> vertices;
			for(size_t i = 0; i < k; i++)
			{
				in >> v;
				vertices.push_back(v);
			}

			add_face(vertices);
		}

		line++;
	}

//	for(size_t i = 0; i < F.size(); i++)
//	{
//		cout 	<< "Face [" << setw(4) << i << "]:\n\n"
//			<< "\t\t";
//		for(size_t j = 0; j < F[i].E.size(); j++)
//		{
//			// FIXME
//			edge e = edge_table.get(F[i].E[j].e);
//			if(F[i].E[j].inverted)
//				cout << e.v << "--" << e.u << " ";
//			else
//				cout << e.u << "--" << e.v << " ";
//		//	edge e = edge_table.vec_edge[F[i].E[j].e-1];
//		//	if(F[i].E[j].flip)
//		//		cout << e.v << "--" << e.u << " ";
//		//	else
//		//		cout << e.u << "--" << e.v << " ";
//		}
//
//		cout << "\n\n";
//	}

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
