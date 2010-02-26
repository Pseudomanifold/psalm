/*!
*	@file	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <GL/glut.h>

#include <fstream>
#include <sstream>
#include <iomanip>

#include <ctime>
#include <cmath>
#include <cassert>

#include "mesh.h"

using namespace std;

/*!
*	Default constructor. Not used at the moment.
*/

mesh::mesh()
{
}

/*!
*	Destructor for mesh. Calls destroy() function (if the user did not
*	already delete data).
*/

mesh::~mesh()
{
	destroy();
}

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
	cout << "Exporting mesh to \"" << filename << "\"...\n";

	ofstream out(filename);
	if(!out.good())
		return(false);

	// header information
	out	<< "ply\n"
		<< "format ascii 1.0\n"
		<< "element vertex " << V.size() << "\n"
		<< "property float x\n"
		<< "property float y\n"
		<< "property float z\n"
		<< "element face " << F.size() << "\n"
		<< "property list uchar int vertex_indices\n"
		<< "end_header\n";

	// write vertex list (separated by spaces)
	for(size_t i = 0; i < V.size(); i++)
	{
		out << fixed << setprecision(8) << V[i]->get_position()[0] << " "
						<< V[i]->get_position()[1] << " "
						<< V[i]->get_position()[2] << "\n";
	}

	// write face list (separated by spaces)
	for(size_t i = 0; i < F.size(); i++)
	{
		out << F[i].num_vertices() << " ";
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			out << F[i].get_vertex(j)->get_id();
			if(j < F[i].num_vertices()-1)
				out << " ";
		}

		out << "\n";
	}

	cout << "...finished.\n";
	return(true);
}

/*!
* Draws the mesh using standard OpenGL drawing routines.
*/

void mesh::draw()
{
	glBegin(GL_POINTS);
	for(size_t i = 0; i < V.size(); i++)
	{
		const v3ctor& p = V[i]->get_position();

		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(p[0], p[1], p[2]);
	}
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINES);
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		edge* e = edge_table.get(i);

		//vertex* v1 = e.u;
		//vertex* v2 = e.v;
		//vertex& v1 = get_vertex(e.u);
		//vertex& v2 = get_vertex(e.v);

		glVertex3f(e->get_u()->get_position()[0], e->get_u()->get_position()[1], e->get_u()->get_position()[2]);
		glVertex3f(e->get_v()->get_position()[0], e->get_v()->get_position()[1], e->get_v()->get_position()[2]);
	}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	for(size_t i = 0; i < F.size(); i++)
	{
		// FIXME: Remove once debugging is done
		// 1 == F-face
		// 2 == E-face
		// 3 == V-face
		if(F[i].type == 1)
			glColor3f(1.0, 0.0, 0.0);
		else if(F[i].type == 2)
			glColor3f(0.0, 1.0, 0.0);
		else if(F[i].type == 3)
			glColor3f(0.0, 0.0, 1.0);
		else
			glColor3f(1.0, 1.0, 1.0);

		v3ctor midpoint;
		// FIXME
		//glBegin(GL_POLYGON);
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			const v3ctor& p = F[i].get_vertex(j)->get_position();
			//glVertex3f(p[0], p[1], p[2]);
			midpoint += p;
		}
		//glEnd();

		midpoint /= F[i].num_vertices();
	//	glPushAttrib(GL_DEPTH_TEST);
	//	glDisable(GL_DEPTH_TEST);
		glPushMatrix();
		glColor3f(0.0, 1.0, 1.0);
		glTranslatef(midpoint[0], midpoint[1], midpoint[2]);
		ostringstream converter;
		converter << F[i].get_id();
		glRasterPos2f(0, 0);
		for(unsigned int i = 0; i < converter.str().size(); i++)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, converter.str()[i]);
		glPopMatrix();
	//	glPopAttrib();

	}
}

/*!
*	Destroys the mesh and all related data structures and frees up used
*	memory.
*/

void mesh::destroy()
{
	cout << "Cleaning up...\n";

	cout << V.size() << "\n";
	for(vector<vertex*>::iterator it = V.begin(); it != V.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	V.clear();
	cout << "* Removed vertex data\n";

	edge_table.destroy();
	cout << "* Removed edge data\n";

	face_table.destroy();
	cout << "* Removed face data\n";

	F.clear();
}

/*!
*	Assignment operator for meshes. Instead of performing a _copy_ of
*	pointers, which will inevitably lead to serious errors, the pointer
*	_data_ is duplicated.
*
*	@param	M Mesh data to assign to the current mesh.
*	@return Reference to current mesh.
*/

mesh& mesh::operator=(const mesh& M)
{
	this->destroy();

	for(vector<vertex*>::const_iterator it = M.V.begin(); it != M.V.end(); it++)
	{
		vertex* v = new vertex;
		*v = *(*it);

		// UPDATE EDGES...
		// ...UPDATES EDGE POINTERS...

		V.push_back(v);
	}

	this->edge_table	= M.edge_table;
	this->face_table	= M.face_table;
	this->F			= M.F;

	// FIXME: Need to remove old version.
	for(vector<face*>::const_iterator it = M.G.begin(); it != M.G.end(); it++)
	{
		face* f = new face;
		*f = *(*it);

		G.push_back(f);
	}

	return(*this);
}

/*!
*	Replaces the current mesh with another one. The other mesh will
*	be deleted/cleared by this operation.
*
*	@param	M Mesh to replace current mesh with.
*	@return	Reference to current mesh.
*/

mesh& mesh::replace_with(mesh& M)
{
	this->destroy();
	this->V = M.V;
	this->F = M.F;
	this->G = M.G;
	this->edge_table = M.edge_table;
	this->face_table = M.face_table;

	M.V.clear();
	M.F.clear();
	M.G.clear();
	M.edge_table.destroy(false);

	return(*this);
}

void mesh::add_face(vector<size_t> vertices, size_t type)
{
	size_t u = 0;
	size_t v = 0;
	size_t face_index = F.size();

	if(vertices.size() == 0)
		return;

	face f;
	edge e;

	// FIXME: Need to delete old code.
	face* g = new face;

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

		e.set(get_vertex(u), get_vertex(v));

		// Add vertex to face; only the first vertex of the edge needs
		// to be considered here
		//f.V.push_back(u);
		f.add_vertex(get_vertex(u));
		g->add_vertex(get_vertex(u));

		// Add it to list of edges for face
		directed_edge edge = edge_table.add(e);
		f.add_edge(edge);
		g->add_edge(edge);

		// FIXME: Need to remove the old code concerning edge
		// updates (face_table.set_f1 etc.).

		/*
			GIANT FIXME: We are assuming that the edges are ordered
			properly. Hence, an edge is supposed to appear only
			_once_ in a fixed direction. If this is not the case,
			the lookup below will _fail_ or an already stored face
			might be overwritten!
		*/

		// Edge already known; update second adjacent face
		if(edge.inverted)
		{
			face_table.set_f2(edge.e, face_index);
			edge.e->set_g(g);

			//V[u]->add_edge(edge.e);
			V[u]->add_face(g);
		}

		// New edge; update first adjacent face and adjacent vertices
		else
		{
		//	if(face_table.get(result.e).f2 < SIZE_T_MAX && face_table.get(result.e).f2 > 0)
		//		cout << "WTF 1?n";

		//	if(face_table.get(result.e).f1 < SIZE_T_MAX && face_table.get(result.e).f1 > 0)
		//		cout << "WTF 2?\n";


			face_table.set_f1(edge.e, face_index);
			face_table.set_f2(edge.e, SIZE_T_MAX);	// TODO: Better place this in the constructor.


			// FIXME: This is ugly...and probably wrong?
			if(edge.new_edge)
			{
				edge.e->set_f(g);
				V[u]->add_edge(edge.e);
				// TODO: Check whether it's ok to do this...or if it
				// can be removed and done for the edge.inverted ==
				// true case
				V[v]->add_edge(edge.e);
			}
			else
				edge.e->set_g(g);

			V[u]->add_face(g); // FIXME: Make g the new f ;-) 
		}

		// Set next start vertex; the orientation should be correct
		// here
		u = v;
	}

	// FIXME: Remove!
	f.type = type;
	g->type = type;

	g->set_id(G.size());
	f.set_id(F.size());

	G.push_back(g);
	F.push_back(f);
}

/*!
*	Returns vertex for a certain ID. The ID is supposed to be the number of
*	the vertex, starting from 0.
*/

vertex* mesh::get_vertex(size_t id)
{
	/*
		TODO:
			- Check for invalid ranges
			- Is it a good idea to assume that the ID is the place
			  of the vertex?
	*/

	return(V[id]);
}

edge* mesh::get_edge(size_t e)
{
	return(edge_table.get(e));
}

/*!
*	Adds a vertex to the mesh. Vertex ID is assigned automatically.
*
*	@param x x position of vertex
*	@param y y position of vertex
*	@param z z position of vertex
*
*	@return Pointer to new vertex. The pointer remains valid during the
*	lifecycle of the mesh.
*/

vertex* mesh::add_vertex(double x, double y, double z)
{
	vertex* v = new vertex(x,y,z, V.size());
	V.push_back(v);

	return(v);
}

/*!
*	Performs one Loop subdivision step on the current mesh.
*/

void mesh::subdivide_loop()
{
	// FIXME
	mesh M_;

	// Construct vertex points
	vertex v;
	for(size_t i = 0; i < V.size(); i++)
	{
		// Find neighbours

		size_t n = V[i]->valency();

		v3ctor vertex_point;
		for(size_t j = 0; j < n; j++)
		{
			const edge* e = V[i]->get_edge(j);

			/*
				i is the index of the current vertex; if the
				start vertex of the edge is _not_ the current
				vertex, it must be the neighbouring vertex.
			*/

			const vertex* neighbour = (e->get_u()->get_id() != V[i]->get_id()? e->get_u() : e->get_v());
			vertex_point += neighbour->get_position();
		}

		double s = 0.0;
	//	if(n > 3)
			s = (1.0/n*(0.625-pow(0.375+0.25*cos(2*M_PI/n), 2)));
	//	else
	//		s = 0.1875;

		vertex_point *= s;
		vertex_point += V[i]->get_position()*(1.0-n*s);

		// FIXME: Provide interface of add_vertex that accepts v3ctor
		// variables and not just coordinates
		V[i]->vertex_point = M_.add_vertex(vertex_point[0], vertex_point[1], vertex_point[2]);
	}

	// Create edge points
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		v3ctor edge_point;
		edge* e = edge_table.get(i);

		// Find remaining vertices of the adjacent faces of the edge
		const vertex* v1 = find_remaining_vertex(e, e->get_f());
		const vertex* v2 = find_remaining_vertex(e, e->get_g());

		if(v1 == NULL)
			cout << "v1 == NULL\n";
		if(v2 == NULL)
			cout << "v2 == NULL\n";

		// TODO: Need special case when v2 is NULL (edge is on
		// boundary).

		edge_point =	(e->get_u()->get_position()+e->get_v()->get_position())*0.375+
				(v1->get_position()+v2->get_position())*0.125;

		// FIXME: Provide interface of add_vertex that accepts v3ctor
		// variables and not just coordinates
		e->edge_point = M_.add_vertex(edge_point[0], edge_point[1], edge_point[2]);
	}

	// Create topology for new mesh
	for(size_t i = 0; i < F.size(); i++)
	{
		// ...go through all vertices of the face
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			/*
				F[i].V[j] is the current vertex of a face. We
				now need to find the _two_ adjacent edges for
				the face. This yields one new triangle.
			*/

			size_t n = F[i].num_edges(); // number of edges in face
			bool assigned_first_edge = false;

			directed_edge d_e1; // first adjacent edge (for vertex & face)
			directed_edge d_e2; // second adjacent edge (for vertex & face)

	//		// ...find an edge that contains the current vertex
	//		for(size_t k = 0; k < n; k++)
	//		{
	//			const edge& e = edge_table.get(F[i].E[k].e);
	//			
	//			// Vertex is start vertex of edge
	//			if(e.u == F[i].V[j])
	//			{
	//				/*
	//					If our vertex is the start
	//					vertex of the edge, and the
	//					edge has been inverted, we need
	//					to consider the next edge.
	//					Situation: v--u--u--w.

	//					If the edge has not been
	//					inverted, we need to consider
	//					the previous edge. Situation:
	//					w--u--u--v

	//					Just the other way round if the
	//					vertex is the end vertex.
	//				*/

	//				//e1 = F[i].E[k].e;
	//				e1 = k;
	//				if(F[i].E[k].inverted)
	//				{
	//					//e2 = F[i].E[(k+1)%n].e;
	//					e2 = (k+1)%n;
	//				}
	//				else
	//				{
	//					//e2 = F[i].E[(k-1)%n].e;
	//					e2 = (k-1)%n;
	//				}

	//				break;
	//			}
	//			// Vertex is end vertex of edge
	//			else if(e.v == F[i].V[j])
	//			{
	//				//e1 = F[i].E[k].e;
	//				e1 = k;
	//				if(F[i].E[k].inverted)
	//				{
	//					//e2 = F[i].E[(k-1)%n].e;
	//					e2 = (k-1)%n;
	//				}
	//				else
	//				{
	//					//e2 = F[i].E[(k+1)%n].e;
	//					e2 = (k+1)%n;
	//				}

	//				break;
	//			}
	//		}

	//		if(	(edge_table.get(e1).u != F[i].V[j] && edge_table.get(e1).v != F[i].V[j]) ||
	//			(edge_table.get(e2).u != F[i].V[j] && edge_table.get(e2).v != F[i].V[j]))
	//			cout << "Error in mesh.\n";
	
			// brute-force search for the two edges
			//
			// TODO: Optimize!
			for(size_t k = 0; k < n; k++)
			{
				directed_edge d_edge  = F[i].get_edge(k);
				if(	d_edge.e->get_u()->get_id() == F[i].get_vertex(j)->get_id() ||
					d_edge.e->get_v()->get_id() == F[i].get_vertex(j)->get_id())
				{
					if(!assigned_first_edge)
					{
						d_e1 = d_edge;
						assigned_first_edge = true;
					}
					else
					{
						d_e2 = d_edge;
						break;
					}
				}
			}


			const vertex* v1 = F[i].get_vertex(j)->vertex_point;
			const vertex* v2 = d_e1.e->edge_point;
			const vertex* v3 = d_e2.e->edge_point;

			// Create vertices for _new_ face. It is important to
			// determine the proper order of the edges here. The
			// new edges should run "along" the old ones.
			vector<size_t> vertices;
			vertices.push_back(v1->get_id());

			// Check whether the current vertex is the _start_
			// vertex of the first edge. This is the case if
			// _either_ the edge is not inverted and the current
			// vertex is equal to the vertex u (start vertex) of
			// the edge _or_ the edge is inverted and the current
			// vertex is equal to the vertex v (end vertex) of the
			// edge.
			if(	(d_e1.e->get_u()->get_id() == F[i].get_vertex(j)->get_id() && d_e1.inverted == false) ||
				(d_e1.e->get_v()->get_id() == F[i].get_vertex(j)->get_id() && d_e1.inverted))
			{
				vertices.push_back(v2->get_id());
				vertices.push_back(v3->get_id());
			}
			else
			{
				vertices.push_back(v3->get_id());
				vertices.push_back(v2->get_id());
			}

			M_.add_face(vertices);
		}

		// Create face from all three edge points of the face; since
		// the edges are stored in the proper order when adding the
		// face, the order in which the edge points are set will be
		// correct.
		vector<size_t> vertices;
		for(size_t j = 0; j < F[i].num_edges(); j++)
			vertices.push_back(F[i].get_edge(j).e->edge_point->get_id());

		M_.add_face(vertices);
	}

	// FIXME: Make this more elegant. Perhaps a "replace" function?
	this->replace_with(M_);

//	cout << "[E_t,F_t]\t= " << edge_table.size() << "," << face_table.T.size() << "\n";
//	cout << "[V,F]\t\t= " << V.size() << "," << F.size() << "\n";
//
//	cout 	<< "Loop subdivision step finished:\n"
//		<< "* Number of vertices: " 	<< V.size() << "\n"
//		<< "* Number of faces: "	<< F.size() << "\n";
}

/*!
*	Performs one step of Doo-Sabin subdivision on the current mesh.
*/

void mesh::subdivide_doo_sabin()
{
	// FIXME
	mesh M_;

	// Create new points
	for(size_t i = 0; i < F.size(); i++)
	{
		// Find centroid of face
		v3ctor centroid;
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			const vertex* v = F[i].get_vertex(j);
			centroid += v->get_position();
		}
		centroid *= 1.0/F[i].num_vertices();

		// For a fixed vertex of the face, find the two edges that are
		// incident on this vertex and calculate their midpoints.
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			const vertex* v = F[i].get_vertex(j);

			const edge* e1 = NULL;
			const edge* e2 = NULL;
			for(size_t k = 0; k < F[i].num_edges(); k++)
			{
				if(	F[i].get_edge(k).e->get_u() == v ||
					F[i].get_edge(k).e->get_v() == v)
				{
					if(e1 == NULL)
						e1 = F[i].get_edge(k).e;
					else
					{
						e2 = F[i].get_edge(k).e;
						break;
					}
				}
			}

			assert(e1 != NULL && e2 != NULL);

			// Calculate midpoints of the edges and the position of
			// face vertex

			v3ctor midpoint1;
			v3ctor midpoint2;

			midpoint1 = (e1->get_u()->get_position()+e1->get_v()->get_position())/2;
			midpoint2 = (e2->get_u()->get_position()+e2->get_v()->get_position())/2;

			v3ctor v_f = (midpoint1+midpoint2+centroid+v->get_position())/4;

			// Add new vertex to the face. The lookup using the
			// vertex's ID is necessary because the face only
			// supplies const pointers.

			// FIXME: Need a better interface for the "add_vertex" function

			vertex* face_vertex = M_.add_vertex(v_f[0], v_f[1], v_f[2]);
			F[i].add_face_vertex(face_vertex);
			G[i]->add_face_vertex(face_vertex); // FIXME: Need to remove F
		}
	}

	// Create new F-faces by connecting the appropriate vertex points
	// (generated above) of the face
	for(size_t i = 0; i < F.size(); i++)
	{
		// Since the vertex points are visited in the order of the old
		// vertices, this step is orientation-preserving

		vector<size_t> vertices; // FIXME: Use pointers.
		for(size_t j = 0; j < F[i].num_vertices(); j++)
			vertices.push_back(F[i].get_face_vertex(j)->get_id());

		// FIXME: 1 == F-face
		M_.add_face(vertices, 1);
	}

	// Create quadrilateral E-faces
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		vector<size_t> vertices; // FIXME: Use pointers.
		edge* e = edge_table.get(i);

		/*
			The situation is as follows:

			---------- v ----------
			|          |          |
			|    F     |     G    |
			|          |          |
			|	   |          |
			---------- u ----------

			Since F is the first face that we encountered when
			traversing the edge in its natural direction, we know
			that the orientation is preserved if the corresponding
			face points are connected like:

			u_F -- u_G -- v_G -- v_F -- u_F

		*/

		// FIXME: Check when/why this can be violated
		assert(e->get_f() != NULL && e->get_g() != NULL);

		const vertex* v1 = find_face_vertex(e->get_f(), e->get_u());
		const vertex* v2 = find_face_vertex(e->get_g(), e->get_u());
		const vertex* v3 = find_face_vertex(e->get_g(), e->get_v());
		const vertex* v4 = find_face_vertex(e->get_f(), e->get_v());

		// FIXME: Need a better interface for this.

		vertices.push_back(v1->get_id());
		vertices.push_back(v2->get_id());
		vertices.push_back(v3->get_id());
		vertices.push_back(v4->get_id());

		// FIXME: 2 == E-face
		M_.add_face(vertices, 2);
	}

	// Create V-faces by connecting the face vertices of all faces that are
	// adjacent to a fixed vertex.
	for(size_t i = 0; i < V.size(); i++)
	{
		assert(V[i]->num_adjacent_faces() > 0);

		cout << V[i]->num_adjacent_faces() << "\n";

		// FIXME:
		// function could be removed?
		vector<const face*> faces = sort_faces(V[i]);

		vector<size_t> vertices;
		for(size_t j = 0; j < V[i]->num_adjacent_faces(); j++)
		{
			//cout << (find_face_vertex(V[i]->get_face(j), V[i])->get_id()) << "\n";
			//vertices.push_back(find_face_vertex(V[i]->get_face(j), V[i])->get_id());
			vertices.push_back(find_face_vertex(faces[j], V[i])->get_id());
		}


		// FIXME: 3 == V-face
		M_.add_face(vertices, 3);
	}

	this->replace_with(M_);
}

/*!
*	Performs one step of Catmull-Clark subdivision on the current mesh.
*/

void mesh::subdivide_catmull_clark()
{
	// FIXME
	mesh M_;

	// Create face points
	for(size_t i = 0; i < G.size(); i++)
	{
		v3ctor centroid;
		for(size_t j = 0; j < G[i]->num_vertices(); j++)
			centroid += G[i]->get_vertex(j)->get_position();

		centroid /= G[i]->num_vertices();

		// FIXME: Better interface
		G[i]->face_point = M_.add_vertex(centroid[0], centroid[1], centroid[2]);
	}

	// Create edge points
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		edge* e = edge_table.get(i);
		v3ctor edge_point = (	e->get_u()->get_position()+
					e->get_v()->get_position()+
					e->get_f()->face_point->get_position()+
					e->get_g()->face_point->get_position())*0.25;

		// FIXME: Better interface
		e->edge_point = M_.add_vertex(edge_point[0], edge_point[1], edge_point[2]);
	}

	// Create vertex points
	for(size_t i = 0; i < V.size(); i++)
	{
		// This follows the original terminology as described by
		// Catmull and Clark

		v3ctor Q;
		v3ctor R;
		v3ctor S;

		size_t n = V[i]->valency();
		assert(n >= 3);

		// Q is the average of the new face points of all faces
		// adjacent to the old vertex point
		for(size_t j = 0; j < V[i]->num_adjacent_faces(); j++)
			Q += V[i]->get_face(j)->face_point->get_position();

		Q /= V[i]->num_adjacent_faces();

		// R is the average of the midpoints of all old edges incident
		// on the current vertex
		for(size_t j = 0; j < n; j++)
		{
			const edge* e = V[i]->get_edge(j);
			R += (e->get_u()->get_position()+e->get_v()->get_position())*0.5;
		}

		R /= n;

		// S is the current vertex
		S = V[i]->get_position();

		// FIXME: Better interface
		v3ctor vertex_point =(Q+R*2+S*(n-3))/n;
		V[i]->vertex_point = M_.add_vertex(vertex_point[0], vertex_point[1], vertex_point[2]);
	}

	/*
		Create new topology of the mesh by connecting

			vertex -- edge -- face -- edge

		points.
	*/

	for(size_t i = 0; i < V.size(); i++)
	{
		for(size_t j = 0; j < V[i]->num_adjacent_faces(); j++)
		{
			const face* f = V[i]->get_face(j);

			// Find the two incident edges of the current vertex
			// that are also part of the current adjacent face

			const edge* e1 = NULL;
			const edge* e2 = NULL;

			for(size_t k = 0; k < V[i]->valency(); k++)
			{
				const edge* e = V[i]->get_edge(k);
				if(e->get_f() == f || e->get_g() == f)
				{
					if(e1 == NULL)
						e1 = e;
					else
					{
						e2 = e;
						break;
					}
				}
			}

			/*
				Check which edge needs to be used first in
				order to orient the new face properly. The
				rationale behind this is to ensure that e1 is
				the _first_ edge that needs to be visited in
				order to get CCW orientation.
			*/

			if(	(e1->get_u()->get_id() == V[i]->get_id() && e1->get_g() == f) ||
				(e1->get_v()->get_id() == V[i]->get_id() && e1->get_f() == f) ||
				(e2->get_u()->get_id() == V[i]->get_id() && e2->get_g() == f) ||
				(e2->get_u()->get_id() == V[i]->get_id() && e2->get_g() == f))
				swap(e1, e2);

			// FIXME: Better interface
			vector<size_t> vertices;
			vertices.push_back(V[i]->vertex_point->get_id());
			vertices.push_back(e1->edge_point->get_id());
			vertices.push_back(f->face_point->get_id());
			vertices.push_back(e2->edge_point->get_id());

			M_.add_face(vertices);
		}
	}

	this->replace_with(M_);
}

/*!
*	Given an edge and a triangular face (where the edge is supposed to be
*	part of the face), return the remaining vertex of the face. This
*	function is used for Loop subdivision.
*
*	@param e Edge
*	@param f Face that is adjacent to the edge. The face is supposed to
*	have only 3 vertices.
*
*	@return Pointer to the remaining vertex of the face or NULL if the
*	vertex could not be found.
*/

const vertex* mesh::find_remaining_vertex(const edge* e, const face* f)
{
	const vertex* result = NULL;
	if(f == NULL || e == NULL)
		return(result);

	for(size_t i = 0; i < f->num_vertices(); i++)
	{
		// The IDs of the start and end vertices of the edge
		// differ from the ID of the remaining edge.
		if(	f->get_vertex(i)->get_id() != e->get_u()->get_id() &&
			f->get_vertex(i)->get_id() != e->get_v()->get_id())
		{
			result = f->get_vertex(i);
			break;
		}
	}

	return(result);
}

/*!
*	Given a vertex and a face (of which the vertex is assumed to be a
*	part), find the corresponding face vertex and return a pointer to it.
*
*	@param f Face
*	@param v Vertex, which is assumed to be a part of the face.
*
*	@return Pointer to the face vertex that corresponds to vertex v in the
*	face.
*/

const vertex* mesh::find_face_vertex(const face* f, const vertex* v)
{
	for(size_t i = 0; i < f->num_vertices(); i++)
	{
		// TODO: Speed could be increased by using lookup tables that map the "old" id to the "new id"
		if(f->get_vertex(i)->get_id() == v->get_id())
			return(f->get_face_vertex(i));
	}

	return(NULL);
}

/*!
*	Given a vertex, sort all the vertex's adjacent faces in
*	counter-clockwise order around the vertex.
*
*	@param v Vertex
*	@return Sorted vector of faces.
*/

vector<const face*> mesh::sort_faces(const vertex* v) const
{
	vector<const face*> faces;
	vector<const edge*> edges;

	for(size_t i = 0; i < v->valency(); i++)
		edges.push_back(v->get_edge(i));

	/*
		The vector of edges is sorted by the following considerations:

			(1)	All incident edges of the vertex are known
			(2)	By using the adjacent faces of an edge, a list of
				adjacent faces can be built:
				(2.1)	Start with any edge
				(2.2)	Search for any other edge which shares
					any face with the start edge. If the edge is
					found, store it after the start edge and repeat
					the process with the new edge.

		This works because there are only two edges which share one
		face. So the next edge that is found will _not_ point to any
		face that has already been processed (i.e., that will _not_ be
		touched by the algorithm anymore).
	*/

	for(size_t i = 0; i < edges.size(); i++)
	{
		for(size_t j = i+1; j < edges.size(); j++)
		{
			if(	edges[j]->get_f() == edges[i]->get_f() ||
				edges[j]->get_g() == edges[i]->get_g() ||
				edges[j]->get_f() == edges[i]->get_g() ||
				edges[j]->get_g() == edges[i]->get_f())
			{
				swap(edges[j], edges[i+1]);
				break;
			}
		}
	}

	/*
		From the sorted edges, the faces can be extracted in sorted
		order by checking which faces of the edges coincide: If edges
		e1 and e2 have adjacent faces (f1,f2), (f2,f3), f3 will be
		added to the face vector. The face vector has to be initialized
		with one face, which would be missing otherwise.
	*/

	faces.push_back(edges[0]->get_f());
	for(size_t i = 1; i < edges.size(); i++)
	{
		if(	edges[i]->get_f() == edges[i-1]->get_f() ||
			edges[i]->get_f() == edges[i-1]->get_g())
			faces.push_back(edges[i]->get_g());
		else
			faces.push_back(edges[i]->get_f());
	}


	// Check whether orientation is CW or CCW by enumerating all relevant
	// configurations.

	bool revert = false;
	if(edges[0]->get_u() == v)
	{
		if(faces[0] == edges[0]->get_f() && faces[1] == edges[0]->get_g())
				revert = true;
		else if(faces[1] != edges[0]->get_f())
				revert = true;
	}
	else
	{
		if(faces[0] == edges[0]->get_g() && faces[1] == edges[0]->get_f())
				revert = true;
		else if(faces[1] != edges[0]->get_g())
				revert = true;
	}

	if(revert)
		reverse(faces.begin(), faces.end());

	return(faces);
}
