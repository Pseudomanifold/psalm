/*!
*	@file	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <GL/glut.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>

#include <ctime>
#include <cmath>
#include <cassert>

#include "mesh.h"

using namespace std;

// Initialization of some static member variables

const short mesh::TYPE_EXT		= 0;
const short mesh::TYPE_PLY		= 1;
const short mesh::TYPE_OBJ		= 2;
const short mesh::TYPE_OFF		= 3;

const short mesh::STATUS_OK		= 0;
const short mesh::STATUS_ERROR		= 1;
const short mesh::STATUS_UNDEFINED	= 2;

/*!
*	Default constructor.
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
*	Tries to load data (presumably mesh data) that from an input source the
*	user specified. The type of the data is determined by the following
*	process:
*
*	1)	If the user did not specify a type:
*
*		1.1)	If the user specified a filename, the function tries to
*		identify the file by its extension.
*
*			1.1.1)	If this identification fails, the function
*			tries to load a PLY file.
*
*		1.2)	If the user did not specify a filename, the function
*		tries to load PLY data from standard input.
*
*	2)	If the user specified a type:
*
*		2.1)	If the user specified a filename, the function tries to
*		load the file with the appropriate type set, regardless of its
*		extension.
*
*		2.2)	If the user did not specify a filename, the function
*		tries to load data with the specified type from standard input.
*
*	@param filename Filename of data file. An empty filename signals that
*	the function tries to read data from standard input.
*
*	@param type Type of mesh data to load. By default, the function tries
*	to guess the data type using filename extensions (if the user specified
*	a filename).
*
*	@return	true if the mesh could be loaded, else false
*/

bool mesh::load(const std::string& filename, const short type)
{
	short result = STATUS_UNDEFINED;

	ifstream in;
	in.open(filename.c_str());

	// Filename given, data type identification by extension
	if(filename.length() >= 4 && type == TYPE_EXT)
	{
		string extension = filename.substr(filename.length()-4);
		transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) tolower);


		if(extension == ".ply")
			result = (load_ply(in) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".obj")
			result = (load_obj(in) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".off")
			result = (load_off(in) ? STATUS_OK : STATUS_ERROR);

		// Unknown extension, so we fall back to PLY files (see below)
	}

	// Data type specified
	else if(type != TYPE_EXT)
	{
		// Check whether file name has been specified. If no file name
		// has been specified, use standard input to read data.

		istream& input_stream = ((filename.length() > 0) ? in : cin);
		switch(type)
		{
			case TYPE_PLY:
				result = (load_ply(input_stream) ? STATUS_OK : STATUS_ERROR);
				break;

			case TYPE_OBJ:
				result = (load_obj(input_stream) ? STATUS_OK : STATUS_ERROR);
				break;

			case TYPE_OFF:
				result = (load_off(input_stream) ? STATUS_OK : STATUS_ERROR);
				break;
		}
	}

	// Last resort: If a nonempty file name has been specified, try to
	// parse a PLY file. Else, try to read a PLY file from standard input.
	if(result == STATUS_UNDEFINED)
	{
		if(filename.length() > 0)
			result = (load_ply(in) ? STATUS_OK : STATUS_ERROR);
		else
			result = (load_ply(cin) ? STATUS_OK : STATUS_ERROR);
	}

	in.close();
	return(result);
}

/*!
*	Tries to save the current mesh data to a user-specified output (a file
*	or an output stream).  The format of the mesh data is determined by the
*	following process:
*
*	1)	If the user did not specify a type:
*
*		1.1)	If the user specified a filename, the function tries to
*		identify the file by its extension.
*
*			1.1.1)	If this identification fails, the function
*			tries to save a PLY file.
*
*		1.2)	If the user did not specify a filename, the function
*		tries to save PLY data to standard output.
*
*	2)	If the user specified a type:
*
*		2.1)	If the user specified a filename, the function tries to
*		save the file with the appropriate type set, regardless of its
*		extension.
*
*		2.2)	If the user did not specify a filename, the function
*		tries to save data with the specified type to standard output.
*
*	@param filename Filename for storing the current mesh. An empty
*	filename signals that standard output is to be used.
*
*	@param type Format in which to save the mesh data. By default, the
*	function tries to guess the data type using filename extensions (if the
*	user specified a filename).
*
*	@warning The data file will be overwritten if it exists. The user will
*	not be notified of this.
*
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save(const std::string& filename, const short type)
{
	short result = STATUS_UNDEFINED;

	ofstream out;
	out.open(filename.c_str());

	// Filename given, data type identification by extension
	if(filename.length() >= 4 && type == TYPE_EXT)
	{
		string extension = filename.substr(filename.length()-4);
		transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) tolower);

		if(extension == ".ply")
			result = (save_ply(out) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".obj")
			result = (save_obj(out) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".off")
			result = (save_off(out) ? STATUS_OK : STATUS_ERROR);

		// Unknown extension, so we fall back to PLY files (see below)
	}

	// Data type specified
	else if(type != TYPE_EXT)
	{
		// Check whether file name has been specified. If no file name
		// has been specified, use standard output to write data.

		ostream& output_stream = ((filename.length() > 0) ? out : cout);

		switch(type)
		{
			case TYPE_PLY:
				result = (save_ply(output_stream) ? STATUS_OK : STATUS_ERROR);
				break;

			case TYPE_OBJ:
				result = (save_obj(output_stream) ? STATUS_OK : STATUS_ERROR);
				break;

			case TYPE_OFF:
				result = (save_off(output_stream) ? STATUS_OK : STATUS_ERROR);
				break;
		}
	}

	// Last resort: If a nonempty filename has been specified, try to
	// write a PLY file. Else, try to write a PLY file to standard output.
	if(result == STATUS_UNDEFINED)
	{
		if(filename.length() > 0)
			result = (save_ply(out) ? STATUS_OK : STATUS_ERROR);
		else
			result = (save_ply(cout) ? STATUS_OK : STATUS_ERROR);
	}

	out.close();
	return(result);
}

/*!
*	Tries to load mesh data in PLY format from an input stream.
*
*	@param	in Input stream (file, standard input)
*	@return	true if the mesh could be loaded, else false
*/

bool mesh::load_ply(istream& in)
{
	if(!in.good())
		return(false);

	string data;

	// Read the headers: Only ASCII format is accepted, but the version is
	// ignored

	getline(in, data);
	if(data != "ply")
	{
		cerr << "Error: I am missing a \"ply\" header for the input data.\n";
		return(false);
	}

	getline(in, data);
	if(data.find("format ascii") == string::npos)
	{
		cerr << "Error: Expected \"format ascii\", got \"" << data << "\" instead.\n";
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

	size_t num_vertices	= 0;
	size_t num_faces	= 0;

	const short MODE_PARSE_HEADER			= 0;
	const short MODE_PARSE_VERTEX_PROPERTIES	= 1;
	const short MODE_PARSE_FACE_PROPERTIES		= 2;

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
			break;

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
						cerr	<< "Error: Can't parse number of faces from \""
							<< data
							<< "\".\n";
						return(false);
					}

					mode = MODE_PARSE_FACE_PROPERTIES;
				}
				else
				{
					cerr << "Error: Expected \"property\", but got \"" << data << "\" instead.\n";
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
						cerr	<< "Error: Can't parse number of vertices from \""
							<< data
							<< "\".\n";

						return(false);
					}

					mode = MODE_PARSE_VERTEX_PROPERTIES;
				}
				else
				{
					cerr	<< "Error: Got \""
						<< data
						<< "\", but expected \"element vertex\" "
						<< "or \"element face\" instead. I cannot continue.\n";
					return(false);
				}

				break;

			default:
				break;
		}
	}

	size_t line	= 0;
	size_t k	= 0; // number of vertices for face

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

			vector<vertex*> vertices;
			size_t v = 0;
			for(size_t i = 0; i < k; i++)
			{
				in >> v;
				vertices.push_back(get_vertex(v));
			}

			add_face(vertices);
		}

		line++;
	}

	return(true);
}

/*!
*	Saves the currently loaded mesh in PLY format.
*
*	@param	out Stream for data output
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_ply(ostream& out)
{
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
		out << F[i]->num_vertices() << " ";
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
		{
			out << F[i]->get_vertex(j)->get_id();
			if(j < F[i]->num_vertices()-1)
				out << " ";
		}

		out << "\n";
	}

	return(true);
}

/*!
*	Loads a mesh in Wavefront OBJ format from an input stream. Almost all
*	possible information from the input stream will be ignored gracefully
*	because the program is only interested in the raw geometrical data.
*
*	@param	in Input stream (file, standard input)
*	@return	true if the mesh could be loaded, else false
*/

bool mesh::load_obj(istream &in)
{
	if(!in.good())
		return(false);

	string line;
	string keyword;
	istringstream converter;

	// These are specify the only keywords of the .OBJ file that the parse
	// is going to understand

	const string OBJ_KEY_VERTEX	= "v";
	const string OBJ_KEY_FACE	= "f";

	while(!getline(in, line).eof())
	{
		converter.str(line);
		converter >> keyword;

		if(keyword == OBJ_KEY_VERTEX)
		{
			double x, y, z;
			converter >> x >> y >> z;

			if(converter.fail())
			{
				cerr	<< "Error: I tried to parse vertex coordinates from line \""
					<< line
					<<" \" and failed.\n";
				return(false);
			}

			add_vertex(x,y,z);
		}
		else if(keyword == OBJ_KEY_FACE)
		{
			// Check whether it is a triplet data string
			if(line.find_first_of('/') != string::npos)
			{
				// FIXME: NYI
			}
			else
			{
				vector<vertex*> vertices;

				long index = 0;
				while(!converter.eof())
				{
					index = 0;
					converter >> index;

					if(index == 0)
					{
						cerr	<< "Error: I cannot parse face data from line \""
							<< line
							<< "\".\n";
						return(false);
					}

					// Handle backwards references...
					else if(index < 0)
					{
						// ...and check the range
						if((V.size()+index) >= 0)
							vertices.push_back(get_vertex(V.size()+index));
						else
						{
							cerr	<< "Error: Invalid backwards vertex reference "
								<< "in line \""
								<< line
								<< "\".\n";
							return(false);
						}
					}
					else
						vertices.push_back(get_vertex(index-1)); // Real men 0-index their variables.
				}

				add_face(vertices);
			}
		}

		keyword.clear();
		line.clear();
		converter.clear();
	}

	return(true);
}

/*!
*	Saves the currently loaded mesh in Wavefront OBJ format. Only raw
*	geometrical data will be output.
*
*	@param	out Stream for data output
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_obj(ostream& out)
{
	if(!out.good())
		return(false);

	for(vector<vertex*>::const_iterator it = V.begin(); it != V.end(); it++)
	{
		v3ctor position = (*it)->get_position();
		out << "v "	<< position[0] << " "
				<< position[1] << " "
				<< position[2] << "\n";
	}

	for(vector<face*>::const_iterator it = F.begin(); it != F.end(); it++)
	{
		out << "f ";
		for(size_t i = 0; i < (*it)->num_vertices(); i++)
		{
			out << ((*it)->get_vertex(i)->get_id()+1); // OBJ is 1-indexed, not 0-indexed
			if(i < (*it)->num_vertices()-1)
				out << " ";
		}
		out << "\n";
	}

	return(true);
}

/*!
*	Loads a mesh in ASCII Geomview format from an input stream.
*
*	@param	in Input stream (file, standard input)
*	@return	true if the mesh could be loaded, else false
*/

bool mesh::load_off(istream& in)
{
	if(!in.good())
		return(false);

	string line;
	istringstream converter;

	/*
		Read "header", i.e.,

			OFF
			num_vertices num_faces num_edges

		where num_edges is ignored.
	*/

	getline(in, line);
	if(line != "OFF")
	{
		cerr << "Error: I am missing a \"OFF\" header for the input data.\n";
		return(false);
	}

	size_t num_vertices, num_faces, num_edges;
	size_t cur_line_num = 0; // count line numbers (after header)

	getline(in, line);
	converter.str(line);
	converter >> num_vertices >> num_faces >> num_edges;

	if(converter.fail())
	{
		cerr << "Error: I cannot parse vertex, face, and edge numbers from \"" << line << "\"\n";
		return(false);
	}

	converter.clear();
	line.clear();

	// These are specify the only keywords of the .OBJ file that the parse
	// is going to understand

	while(!getline(in, line).eof())
	{
		converter.str(line);

		if(cur_line_num < num_vertices)
		{
			double x, y, z;
			converter >> x >> y >> z;

			if(converter.fail())
			{
				cerr	<< "Error: I tried to parse vertex coordinates from line \""
					<< line
					<<" \" and failed.\n";
				return(false);
			}

			add_vertex(x,y,z);
		}
		else if((cur_line_num-num_vertices) < num_faces)
		{
			size_t k	= 0;
			size_t index	= 0;

			converter >> k;

			vector<vertex*> vertices;
			for(size_t i = 0; i < k; i++)
			{
				converter >> index;
				if(converter.fail())
				{
					cerr	<< "Error: Tried to parse face data in line \""
						<< line
						<< "\", but failed.\n";
					return(false);
				}

				if(index >= V.size())
				{
					cerr	<< "Error: Index " << index << "in line \""
						<< line
						<< "\" is out of bounds.\n";
					return(false);
				}

				vertices.push_back(get_vertex(index));
			}

			add_face(vertices);
		}
		else
		{
			cerr << "Error: Got an unexpected data line \"" << line << "\".\n";
			return(false);
		}

		cur_line_num++;

		converter.clear();
		line.clear();
	}

	return(true);
}

/*!
*	Saves the currently loaded mesh in ASCII Geomview object file format
*	(OFF).
*
*	@param	out Stream for data output
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_off(ostream& out)
{
	if(!out.good())
		return(false);

	out	<< "OFF\n"
		<< V.size() << " " << F.size() << " " << "0\n"; // For programs that actually interpret edge data,
								// the last parameter should be changed

	for(vector<vertex*>::const_iterator it = V.begin(); it != V.end(); it++)
	{
		v3ctor position = (*it)->get_position();
		out	<< position[0] << " "
			<< position[1] << " "
			<< position[2] << "\n";
	}

	for(vector<face*>::const_iterator it = F.begin(); it != F.end(); it++)
	{
		out	<< (*it)->num_vertices()
			<< " ";

		for(size_t i = 0; i < (*it)->num_vertices(); i++)
		{
			out << (*it)->get_vertex(i)->get_id();
			if(i < (*it)->num_vertices()-1)
				out << " ";
		}
		out << "\n";
	}

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
	for(vector<edge*>::iterator e = E.begin(); e != E.end(); e++)
	{
		glVertex3f(	(*e)->get_u()->get_position()[0],
				(*e)->get_u()->get_position()[1],
				(*e)->get_u()->get_position()[2]);
		glVertex3f(	(*e)->get_v()->get_position()[0],
				(*e)->get_v()->get_position()[1],
				(*e)->get_v()->get_position()[2]);
	}
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	for(size_t i = 0; i < F.size(); i++)
	{
		glBegin(GL_POLYGON);
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
		{
			const v3ctor& p = F[i]->get_vertex(j)->get_position();
			glVertex3f(p[0], p[1], p[2]);
		}
		glEnd();
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

	for(vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	E.clear();
	cout << "* Removed edge data\n";

	for(vector<face*>::iterator it = F.begin(); it != F.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	F.clear();
	cout << "* Removed face data\n";
}

/*!
*	Replaces the current mesh with another one. The other mesh will
*	be deleted/cleared by this operation.
*
*	@param	M Mesh to replace current mesh with
*/

void mesh::replace_with(mesh& M)
{
	*this = M;

	// Clear old mesh
	M.V.clear();
	M.F.clear();
	M.E.clear();
	M.E_M.clear();
}

/*!
*	Assigns another mesh to the current mesh.
*
*	@param M Mesh to assign to the current mesh
*	@return Reference to replaced mesh
*/

mesh& mesh::operator=(const mesh& M)
{
	this->destroy();

	this->V		= M.V;
	this->F		= M.F;
	this->E		= M.E;
	this->E_M	= M.E_M;

	return(*this);
}

/*!
*	Given a vector of pointers to vertices, where the vertices are assumed
*	to be in counterclockwise order, construct a face and add it to the
*	mesh.
*
*	@param vertices Vector of vertices for face. The vertices are connected
*	in the order they appear in the vector. A last edge from the last
*	vertex to the first vertex is added.
*
*	@warning The orientation of the vertices around the face is _not_
*	checked, but left as a task for the calling function.
*/

void mesh::add_face(std::vector<vertex*> vertices)
{
	vertex* u = NULL;
	vertex* v = NULL;

	if(vertices.size() == 0)
		return;

	face* f = new face;

	vector<vertex*>::iterator it = vertices.begin();
	u = *it;

	for(it = vertices.begin(); it != vertices.end(); it++)
	{
		// Handle last vertex; should be the edge v--u
		if((it+1) == vertices.end())
		{
			u = *it;
			v = *vertices.begin();
		}
		// Normal case
		else
			v = *(it+1);

		// Add vertex to face; only the first vertex of the edge needs
		// to be considered here
		f->add_vertex(u);

		// Add it to list of edges for face
		directed_edge edge = add_edge(u, v);
		f->add_edge(edge);

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
			edge.e->set_g(f);
			//V[u]->add_edge(edge.e);
			u->add_face(f);
		}

		// (Possibly) new edge; update first adjacent face and adjacent
		// vertices
		else
		{
			// FIXME: This is ugly...and probably wrong?
			if(edge.new_edge)
			{
				edge.e->set_f(f);
				u->add_edge(edge.e);
				// TODO: Check whether it's ok to do this...or if it
				// can be removed and done for the edge.inverted ==
				// true case
				v->add_edge(edge.e);
			}
			else
				edge.e->set_g(f);

			u->add_face(f);
		}

		// Set next start vertex; the orientation should be correct
		// here
		u = v;
	}

	F.push_back(f);
}

/*!
*	Adds a triangular face to the mesh. This function allows the caller to
*	specify 3 vertices that will form the new triangle. Thus, specifying a
*	vector of pointers is not necessary.
*
*	@param v1 Pointer to 1st vertex of new face
*	@param v2 Pointer to 2nd vertex of new face
*	@param v3 Pointer to 3rd vertex of new face
*
*	@warning The vertex pointers are not checked for consistency.
*/

inline void mesh::add_face(vertex* v1, vertex* v2, vertex* v3)
{
	vector<vertex*> vertices;

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);

	add_face(vertices);
}

/*!
*	Adds a quadrangular face to the mesh. This function allows the caller
*	to specify 4 vertices that will form the new quadrangle. Thus,
*	specifying a vector of pointers is not necessary.
*
*	@param v1 Pointer to 1st vertex of new face
*	@param v2 Pointer to 2nd vertex of new face
*	@param v3 Pointer to 3rd vertex of new face
*	@param v4 Pointer to 4th vertex of new face
*
*	@warning The vertex pointers are not checked for consistency and
*	planarity.
*/

inline void mesh::add_face(vertex* v1, vertex* v2, vertex* v3, vertex* v4)
{
	vector<vertex*> vertices;

	vertices.push_back(v1);
	vertices.push_back(v2);
	vertices.push_back(v3);
	vertices.push_back(v4);

	add_face(vertices);
}

/*!
*	Tries to add an edge to the current mesh. The edge is described by
*	pointers to its start and end vertices.
*
*	@param u Pointer to start vertex
*	@param v Pointer to end vertex
*
*	@return A directed edge, i.e., an edge with a certain start and end
*	vertex and a flag whether the edge has been inverted or not. If the
*	edge has been inverted, start and end vertex change their meaning. This
*	is required because from the point of the medge, edge (u,v) and edge
*	(v,u) are _the same_. In order to store only an edge only one time, the
*	mesh checks whether the edge already exists.
*/

directed_edge mesh::add_edge(const vertex* u, const vertex* v)
{
	directed_edge result;

	/*
		Calculate ID of edge by using the Cantor pairing function. If
		necessary, the IDs of the edge's vertices are swapped so that
		k1 will always be the less or equal to k2. This is done in
		order to provide a natural sorting order for the edges.
	*/

	size_t k1, k2;
	if(u->get_id() < v->get_id())
	{
		k1 = u->get_id();
		k2 = v->get_id();
	}
	else
	{
		k1 = v->get_id();
		k2 = u->get_id();
	}

	size_t k = static_cast<size_t>(0.5*(k1+k2)*(k1+k2+1)+k2);

	// Check whether edge exists
	std::tr1::unordered_map<size_t, edge*>::iterator it;
	if((it = E_M.find(k)) == E_M.end())
	{
		// Edge not found, create an edge from the _original_ edge and
		// add it to the map
		edge* new_edge = new edge(u, v);
		E.push_back(new_edge);
		E_M[k] = new_edge;

		result.e = new_edge;
		result.inverted = false;
		result.new_edge = true;
	}
	else
	{
		// Edge has been found, check whether the proper direction has
		// been stored.
		if(it->second->get_u()->get_id() != u->get_id())
			result.inverted = true;
		else
			result.inverted = false;

		result.new_edge = false;
		result.e = it->second;
	}

	return(result);
}

/*!
*	Given a vertex ID, return the appropriate vertex. This function is
*	meant to serve as an interface for any vertex queries, regardless of
*	what storage container the mesh uses.
*
*	@param	Vertex ID, which is supposed to be set on allocating/creating a
*	new vertex.
*
*	@return If the ID is correct, a pointer to the corresponding vertex is
*	returned. Else, a NULL pointer will be rereturned.
*/

vertex* mesh::get_vertex(size_t id)
{
	if(id >= V.size())
		return(NULL);
	else
		return(V[id]);
}

/*!
*	Adds a vertex to the mesh. The vertex ID is assigned automatically.
*
*	@param x x position of vertex
*	@param y y position of vertex
*	@param z z position of vertex
*
*	@warning The vertices are not checked for duplicates because this
*	function is assumed to be called from internal methods only.
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
*	Adds a vertex to the mesh. The vertex ID is assigned automatically.
*
*	@param pos Position of the new vertex
*
*	@warning The vertices are not checked for duplicates because this
*	function is assumed to be called from internal methods only.
*
*	@return Pointer to new vertex. The pointer remains valid during the
*	lifecycle of the mesh.
*/

vertex* mesh::add_vertex(const v3ctor& pos)
{
	return(add_vertex(pos[0], pos[1], pos[2]));
}

/*!
*	Performs one step of Loop subdivision on the current mesh, thereby
*	replacing it with the refined mesh.
*/

void mesh::subdivide_loop()
{
	mesh M;

	// Construct vertex points
	for(size_t i = 0; i < V.size(); i++)
	{
		// Find neighbours

		size_t n = V[i]->valency();
		v3ctor vertex_point;

		// TODO: Could also be done using iterators.
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
		if(n > 3)
			s = (1.0/n*(0.625-pow(0.375+0.25*cos(2*M_PI/n), 2)));
		else
			s = 0.1875;

		vertex_point *= s;
		vertex_point += V[i]->get_position()*(1.0-n*s);

		V[i]->vertex_point = M.add_vertex(vertex_point);
	}

	// Create edge points
	for(vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
	{
		v3ctor edge_point;
		edge* e = *it;

		// Find remaining vertices of the adjacent faces of the edge
		const vertex* v1 = find_remaining_vertex(e, e->get_f());
		const vertex* v2 = find_remaining_vertex(e, e->get_g());

		// TODO: Need special case when v2 is NULL (edge is on
		// boundary).
		assert(v1 != NULL && v2 != NULL);

		edge_point =	(e->get_u()->get_position()+e->get_v()->get_position())*0.375+
				(v1->get_position()+v2->get_position())*0.125;

		e->edge_point = M.add_vertex(edge_point);
	}

	// Create topology for new mesh
	for(size_t i = 0; i < F.size(); i++)
	{
		// ...go through all vertices of the face
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
		{
			/*
				F[i].V[j] is the current vertex of a face. We
				now need to find the _two_ adjacent edges for
				the face. This yields one new triangle.
			*/

			size_t n = F[i]->num_edges(); // number of edges in face
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
				directed_edge d_edge = F[i]->get_edge(k);
				if(	d_edge.e->get_u()->get_id() == F[i]->get_vertex(j)->get_id() ||
					d_edge.e->get_v()->get_id() == F[i]->get_vertex(j)->get_id())
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

			vertex* v1 = F[i]->get_vertex(j)->vertex_point;
			vertex* v2 = d_e1.e->edge_point;
			vertex* v3 = d_e2.e->edge_point;

			/*
				 Create vertices for _new_ face. It is
				 important to determine the proper order of the
				 edges here. The new edges should run "along"
				 the old ones.

				 Hence, it is checked whether the current
				 vertex is the _start_ vertex of the first
				 edge. This is the case if _either_ the edge is
				 not inverted and the current vertex is equal
				 to the vertex u (start vertex) of the edge
				 _or_ the edge is inverted and the current
				 vertex is equal to the vertex v (end vertex)
				 of the edge.

				 If the current vertex is the start vertex of
				 the first edge, connecting the new points in
				 order v1, v2, v3 will create a face that is
				 oriented counterclockwise. Else, order v1, v3,
				 v2 will have to be used.

				 v1 can remain fixed in all cases because of
				 the symmetry.
			*/

			if((d_e1.e->get_u()->get_id() == F[i]->get_vertex(j)->get_id() && d_e1.inverted == false) ||
			   (d_e1.e->get_v()->get_id() == F[i]->get_vertex(j)->get_id() && d_e1.inverted))
				M.add_face(v1, v2, v3);

			// Swap order
			else
				M.add_face(v1, v3, v2);
		}

		// Create face from all three edge points of the face; since
		// the edges are stored in the proper order when adding the
		// face, the order in which the edge points are set will be
		// correct.

		assert(F[i]->num_edges() == 3); // TODO: Replace with something nicer.

		M.add_face(	F[i]->get_edge(0).e->edge_point,
				F[i]->get_edge(1).e->edge_point,
				F[i]->get_edge(2).e->edge_point);
	}

	this->replace_with(M);
}

/*!
*	Performs one step of Doo-Sabin subdivision on the current mesh.
*/

void mesh::subdivide_doo_sabin()
{
	mesh M;

	// Create new points
	for(size_t i = 0; i < F.size(); i++)
	{
		// Find centroid of face
		v3ctor centroid;
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
		{
			const vertex* v = F[i]->get_vertex(j);
			centroid += v->get_position();
		}
		centroid *= 1.0/F[i]->num_vertices();

		// For a fixed vertex of the face, find the two edges that are
		// incident on this vertex and calculate their midpoints.
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
		{
			const vertex* v = F[i]->get_vertex(j);

			const edge* e1 = NULL;
			const edge* e2 = NULL;
			for(size_t k = 0; k < F[i]->num_edges(); k++)
			{
				if(	F[i]->get_edge(k).e->get_u() == v ||
					F[i]->get_edge(k).e->get_v() == v)
				{
					if(e1 == NULL)
						e1 = F[i]->get_edge(k).e;
					else
					{
						e2 = F[i]->get_edge(k).e;
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

			v3ctor face_vertex_position = (midpoint1+midpoint2+centroid+v->get_position())/4;

			// Add new vertex to the face. The lookup using the
			// vertex's ID is necessary because the face only
			// supplies const pointers.

			vertex* face_vertex = M.add_vertex(face_vertex_position);
			F[i]->add_face_vertex(face_vertex);
		}
	}

	// Create new F-faces by connecting the appropriate vertex points
	// (generated above) of the face
	for(size_t i = 0; i < F.size(); i++)
	{
		// Since the vertex points are visited in the order of the old
		// vertices, this step is orientation-preserving

		vector<vertex*> vertices;
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
			vertices.push_back(F[i]->get_face_vertex(j));

		M.add_face(vertices);
	}

	// Create quadrilateral E-faces
	for(vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
	{
		edge* e = *it;

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

		vertex* v1 = find_face_vertex(e->get_f(), e->get_u());
		vertex* v2 = find_face_vertex(e->get_g(), e->get_u());
		vertex* v3 = find_face_vertex(e->get_g(), e->get_v());
		vertex* v4 = find_face_vertex(e->get_f(), e->get_v());

		M.add_face(v1, v2, v3, v4);
	}

	// Create V-faces by connecting the face vertices of all faces that are
	// adjacent to a fixed vertex.
	for(size_t i = 0; i < V.size(); i++)
	{
		// FIXME: Can this be handled better?
		assert(V[i]->num_adjacent_faces() > 0);

		// The faces need to be sorted in counterclockwise order around
		// the vertex.
		vector<face*> faces = sort_faces(V[i]);

		vector<vertex*> vertices;
		for(size_t j = 0; j < V[i]->num_adjacent_faces(); j++)
			vertices.push_back(find_face_vertex(faces[j], V[i]));

		M.add_face(vertices);
	}

	this->replace_with(M);
}

/*!
*	Performs one step of Catmull-Clark subdivision on the current mesh.
*/

void mesh::subdivide_catmull_clark()
{
	mesh M;

	// Create face points
	for(size_t i = 0; i < F.size(); i++)
	{
		v3ctor centroid;
		for(size_t j = 0; j < F[i]->num_vertices(); j++)
			centroid += F[i]->get_vertex(j)->get_position();

		centroid /= F[i]->num_vertices();

		F[i]->face_point = M.add_vertex(centroid);
	}

	// Create edge points
	for(vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
	{
		edge* e = *it;
		v3ctor edge_point = (	e->get_u()->get_position()+
					e->get_v()->get_position()+
					e->get_f()->face_point->get_position()+
					e->get_g()->face_point->get_position())*0.25;

		e->edge_point = M.add_vertex(edge_point);
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

		v3ctor vertex_point =(Q+R*2+S*(n-3))/n;
		V[i]->vertex_point = M.add_vertex(vertex_point);
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

			M.add_face(	V[i]->vertex_point,
					e1->edge_point,
					f->face_point,
					e2->edge_point);
		}
	}

	this->replace_with(M);
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

vertex* mesh::find_face_vertex(face* f, const vertex* v)
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
*	counterclockwise order around the vertex.
*
*	@param v Pointer to vertex
*	@return Sorted vector of faces
*/

vector<face*> mesh::sort_faces(vertex* v)
{
	vector<face*> faces;
	vector<edge*> edges;

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
