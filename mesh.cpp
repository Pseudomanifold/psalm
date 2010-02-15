/*!
*	@file 	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <GL/glut.h>

#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cmath>
#include <climits>

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
		vertex* v1 = NULL;
		vertex* v2 = NULL;

		e->get(v1, v2);

		//vertex* v1 = e.u;
		//vertex* v2 = e.v;
		//vertex& v1 = get_vertex(e.u);
		//vertex& v2 = get_vertex(e.v);

		glVertex3f(v1->get_position()[0], v1->get_position()[1], v1->get_position()[2]);
		glVertex3f(v2->get_position()[0], v2->get_position()[1], v2->get_position()[2]);
	}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_TRIANGLES);
	for(size_t i = 0; i < F.size(); i++)
	{
		for(size_t j = 0; j < F[i].num_vertices(); j++)
		{
			const v3ctor& p = F[i].get_vertex(j)->get_position();
			glVertex3f(p[0], p[1], p[2]);
		}
	}
	glEnd();
}

void mesh::add_face(vector<size_t> vertices)
{
	size_t u = 0;
	size_t v = 0;
	size_t face_index = F.size();

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

		e.set(get_vertex(u), get_vertex(v));

		// Add vertex to face; only the first vertex of the edge needs
		// to be considered here
		//f.V.push_back(u);
		f.add_vertex(get_vertex(u));

		// Add it to list of edges for face
		directed_edge edge = edge_table.add(e);
		f.add_edge(edge);

		/*
			GIANT FIXME: We are assuming that the edges are ordered
			properly. Hence, an edge is supposed to appear only
			_once_ in a fixed direction. If this is not the case,
			the lookup below will _fail_ or an already stored face
			might be overwritten!
		*/

		// Edge already known; update second adjacent face
		if(edge.inverted)
			face_table.set_f2(edge.e, face_index);
		
		// New edge; update first adjacent face and adjacent vertices
		else
		{
		//	if(face_table.get(result.e).f2 < SIZE_T_MAX && face_table.get(result.e).f2 > 0)
		//		cout << "WTF 1?n";
	
		//	if(face_table.get(result.e).f1 < SIZE_T_MAX && face_table.get(result.e).f1 > 0)
		//		cout << "WTF 2?\n";

			face_table.set_f1(edge.e, face_index);
			face_table.set_f2(edge.e, SIZE_T_MAX);	// TODO: Better place this in the constructor.

			V[u]->add_incident_edge(edge.e);
			V[v]->add_incident_edge(edge.e);
		}

		// Set next start vertex; the orientation should be correct
		// here
		u = v;
	}

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



void mesh::add_vertex(double x, double y, double z)
{
	vertex* v = new vertex(x,y,z, V.size());
	V.push_back(v);
}

/*!
*	Performs one Loop subdivision step on the current mesh.
*/

// FIXME
#ifdef INCLUDE_LOOP_SUBDIVISION
void mesh::subdivide_loop()
{
	mesh M_;

	// Construct vertex points
	vertex v;
	for(size_t i = 0; i < V.size(); i++)
	{
		// Find neighbours

		size_t n = V[i].size();
		
		v3ctor vertex_point;
		for(size_t j = 0; j < n; j++)
		{
			const edge& e = edge_table.get(V[i].get(j));
			const vertex& neighbour = (e.u != i? V[e.u] : V[e.v]); 	// i is index of current vertex; if the
										// start of the edge is _not_ the current
										// vertex, it must be the neighbouring
										// vertex.

			vertex_point += neighbour.p;
		}

		double s = 0.0;
	//	if(n > 3)
			s = (1.0/n*(0.625-pow(0.375+0.25*cos(2*M_PI/n), 2)));
	//	else
	//		s = 0.1875;

		vertex_point *= s; 
		vertex_point += V[i].p*(1.0-n*s);

		v.v_v = true;
		v.p = vertex_point;
		M_.V.push_back(v);
		
		V[i].v_p = M_.V.size()-1;
	}

	// Create edge points
	for(size_t i = 0; i < edge_table.size(); i++)
	{
		v3ctor edge_point;
		const face_query& result = face_table.get(i);
		edge& e = edge_table.get(i);

		// Find remaining vertex of first face
		size_t v1 = SIZE_T_MAX;
		for(size_t j = 0; j < F[result.f1].V.size(); j++)
		{
			if(	F[result.f1].V[j] != e.u &&
				F[result.f1].V[j] != e.v)
				v1 = F[result.f1].V[j];
		}
		
		//cout << F[result.f1].V.size() << " (" << e.u << "," << e.v << "," << v1 << ")\n";
		
		// Find remaining vertex of second face
		size_t v2 = SIZE_T_MAX;
		if(result.f2 != SIZE_T_MAX)
		{
			for(size_t j = 0; j < F[result.f2].V.size(); j++)
			{
				if(	F[result.f2].V[j] != e.u &&
					F[result.f2].V[j] != e.v)
					v2 = F[result.f2].V[j];
			}
		
			//cout << F[result.f2].V.size() << " (" << e.u << "," << e.v << "," << v2 << ")\n";
		}

		edge_point = (V[e.u].p+V[e.v].p)*0.375+(V[v1].p+V[v2].p)*0.125;

		v.v_v = false;
		v.p = edge_point;
		M_.V.push_back(v);

		e.e_p = M_.V.size()-1;
	}
	
	// Create topology for new mesh
	for(size_t i = 0; i < F.size(); i++)
	{
		// ...go through all vertices of the face
		for(size_t j = 0; j < F[i].V.size(); j++)
		{
			/*
				F[i].V[j] is the current vertex of a face. We
				now need to find the _two_ adjacent edges for
				the face. This yields one new triangle.
			*/

			size_t v1, v2, v3;
			size_t n = F[i].E.size(); // number of edges in face

			size_t e1 = SIZE_T_MAX;	// first adjacent edge (for vertex & face)
			size_t e2 = SIZE_T_MAX;	// second adjacent edge (for vertex & face)

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
				const edge& e = edge_table.get(F[i].E[k].e);
				if(e.u == F[i].V[j] || e.v == F[i].V[j])
				{
					if(e1 == SIZE_T_MAX)
						e1 = k;
					else
					{
						e2 = k;
						break;
					}
				}
			}
		
			v1 = V[F[i].V[j]].v_p; // vertex point of current vertex
			//v2 = edge_table.get(e1).e_p;
			//v3 = edge_table.get(e2).e_p;
			v2 = edge_table.get(F[i].E[e1].e).e_p;
			v3 = edge_table.get(F[i].E[e2].e).e_p;

			// Create vertices for _new_ face. It is important to
			// determine the proper order of the edges here. The
			// new edges should run "along" the old ones.
			vector<size_t> vertices;

			vertices.push_back(v1);

			if(	(edge_table.get(F[i].E[e1].e).u == F[i].V[j] && F[i].E[e1].inverted == false)  ||
				(edge_table.get(F[i].E[e1].e).v == F[i].V[j] && F[i].E[e1].inverted))
			{
				vertices.push_back(v2);
				vertices.push_back(v3);
			}
			else
			{
				vertices.push_back(v3);
				vertices.push_back(v2);
			}

			M_.add_face(vertices);
		}

		// Create face from all three edge points of the face
		vector<size_t> vertices;
		for(size_t j = 0; j < F[i].E.size(); j++)
			vertices.push_back(edge_table.get(F[i].E[j].e).e_p);

		M_.add_face(vertices);
	}

//	M_.F = F;
//	M_.edge_table = edge_table;
//	M_.face_table = face_table;

	*this = M_;

	cout << "[E_t,F_t]\t= " << edge_table.size() << "," << face_table.T.size() << "\n";
	cout << "[V,F]\t\t= " << V.size() << "," << F.size() << "\n";

	cout 	<< "Loop subdivision step finished:\n"
		<< "* Number of vertices: " 	<< V.size() << "\n"
		<< "* Number of faces: "	<< F.size() << "\n";
}
#endif

/*!
*	Performs one step of Doo-Sabin subdivision on the current mesh.
*/

void mesh::subdivide_doo_sabin()
{
}

/*!
*	Performs one step of Catmull-Clark subdivision on the current mesh.
*/

void mesh::subdivide_catmull_clark()
{
}
