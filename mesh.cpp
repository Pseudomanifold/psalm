/*!
*	@file	mesh.cpp
*	@brief	Functions for representing a mesh
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <limits>
#include <stdexcept>

#include <ctime>
#include <cmath>
#include <cassert>
#include <cerrno>
#include <cstring>

#include "mesh.h"

namespace psalm
{

/*!
*	Sets some default values.
*/

mesh::mesh()
{
	id_offset = 0;
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

bool mesh::load(const std::string& filename, file_type type)
{
	status result = STATUS_UNDEFINED;

	std::ifstream in;
	if(filename.length() > 0)
	{
		errno = 0;
		in.open(filename.c_str());
		if(errno)
		{
			std::string error = strerror(errno);
			std::cerr	<< "psalm: Could not load input file \""
					<< filename << "\": "
					<< error << "\n";

			return(false);
		}
	}

	this->destroy();

	// Filename given, data type identification by extension
	if(filename.length() >= 4 && type == TYPE_EXT)
	{
		std::string extension = filename.substr(filename.find_last_of('.')); // TODO: Handle errors
		std::transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) tolower);

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

		std::istream& input_stream = ((filename.length() > 0) ? in : std::cin);
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

			case TYPE_EXT: // to shut up the compiler
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
			result = (load_ply(std::cin) ? STATUS_OK : STATUS_ERROR);
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

bool mesh::save(const std::string& filename, file_type type)
{
	status result = STATUS_UNDEFINED;

	std::ofstream out;
	if(filename.length() > 0)
	{
		errno = 0;
		out.open(filename.c_str());
		if(errno)
		{
			std::string error = strerror(errno);
			std::cerr	<< "psalm: Could not save to file \""
					<< filename << "\": "
					<< error << "\n";

			return(false);
		}
	}

	// Filename given, data type identification by extension
	if(filename.length() >= 4 && type == TYPE_EXT)
	{
		std::string extension = filename.substr(filename.find_last_of('.')); // TODO: Handle errors
		std::transform(extension.begin(), extension.end(), extension.begin(), (int(*)(int)) tolower);

		if(extension == ".ply")
			result = (save_ply(out) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".obj")
			result = (save_obj(out) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".off")
			result = (save_off(out) ? STATUS_OK : STATUS_ERROR);
		else if(extension == ".hole")
			result = (save_hole(out) ? STATUS_OK : STATUS_ERROR);

		// Unknown extension, so we fall back to PLY files (see below)
	}

	// Data type specified
	else if(type != TYPE_EXT)
	{
		// Check whether file name has been specified. If no file name
		// has been specified, use standard output to write data.

		std::ostream& output_stream = ((filename.length() > 0) ? out : std::cout);

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

			case TYPE_EXT: // to shut up the compiler
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
			result = (save_ply(std::cout) ? STATUS_OK : STATUS_ERROR);
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

bool mesh::load_ply(std::istream& in)
{
	if(!in.good())
		return(false);

	std::string data;

	// Read the headers: Only ASCII format is accepted, but the version is
	// ignored

	std::getline(in, data);
	if(data != "ply")
	{
		std::cerr << "psalm: I am missing a \"ply\" header for the input data.\n";
		return(false);
	}

	std::getline(in, data);
	if(data.find("format ascii") == std::string::npos)
	{
		std::cerr << "psalm: Expected \"format ascii\", got \"" << data << "\" instead.\n";
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

	enum modes
	{
		PARSE_HEADER,
		PARSE_VERTEX_PROPERTIES,
		PARSE_FACE_PROPERTIES
	};

	modes mode = PARSE_HEADER;
	while(!in.eof())
	{
		getline(in, data);

		/*
			Lines contaning "comment" or "obj_info" are skipped.
			Not sure whether obj_info is allowed to appear at all.
		*/
		if(	data.find("comment")  != std::string::npos ||
			data.find("obj_info") != std::string::npos)
			continue;
		else if(data.find("end_header") != std::string::npos)
			break;

		switch(mode)
		{
			case PARSE_VERTEX_PROPERTIES:

				if(data.find("property") != std::string::npos)
				{
					/*
						Ignore. Some special handlings
						for more properties could be
						added here.
					*/

					continue;
				}
				else if(data.find("element face") != std::string::npos)
				{
					mode = PARSE_FACE_PROPERTIES;

					std::string dummy; // not necessary, but more readable
					std::istringstream converter(data);
					converter >> dummy >> dummy >> num_faces;

					if(num_faces == 0)
					{
						std::cerr	<< "psalm: Can't parse number of faces from \""
								<< data
								<< "\".\n";
						return(false);
					}

					mode = PARSE_FACE_PROPERTIES;
				}
				else
				{
					std::cerr << "psalm: Expected \"property\", but got \"" << data << "\" instead.\n";
					return(false);
				}

				break;

			case PARSE_FACE_PROPERTIES:

				if(data.find("property list") == std::string::npos)
				{
					std::cerr << "Warning: Got \"" << data << "\". "
					<< "This property is unknown and might lead "
					<< "to problems when parsing the file.\n";
				}

				break;

			// Expect "element vertex" line
			case PARSE_HEADER:

				if(data.find("element vertex") != std::string::npos)
				{
					mode = PARSE_VERTEX_PROPERTIES;

					std::string dummy; // not necessary, but more readable
					std::istringstream converter(data);
					converter >> dummy >> dummy >> num_vertices;

					if(num_vertices == 0)
					{
						std::cerr	<< "psalm: Can't parse number of vertices from \""
								<< data
								<< "\".\n";

						return(false);
					}

					mode = PARSE_VERTEX_PROPERTIES;
				}
				else
				{
					std::cerr	<< "psalm: Got \""
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

	size_t cur_line	= 0;
	size_t k	= 0; // number of vertices for face

	double x, y, z;
	std::string line;
	while(std::getline(in, line))
	{
		std::istringstream parser(line);
		if(cur_line < num_vertices)
		{
			parser >> x >> y >> z;
			add_vertex(x, y, z);
		}
		else
		{
			k = 0;
			parser >> k;
			if(k == 0)
				break;

			// Store vertices of face in proper order and add a new
			// face.

			std::vector<vertex*> vertices;
			size_t v = 0;
			for(size_t i = 0; i < k; i++)
			{
				parser >> v;
				vertices.push_back(get_vertex(v));
			}

			add_face(vertices);
		}

		cur_line++;
	}

	/*
	   FIXME

	// Mark boundary vertices if the user has chosen to preserve them.
	// Else, we do not need the additional information.
	if(preserve_boundaries)
		mark_boundaries();

	*/

	return(true);
}

/*!
*	Saves the currently loaded mesh in PLY format.
*
*	@param	out Stream for data output
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_ply(std::ostream& out)
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
		<< "property uchar red\n"
		<< "property uchar green\n"
		<< "property uchar blue\n"
		<< "element face " << F.size() << "\n"
		<< "property list uchar int vertex_indices\n"
		<< "end_header\n";

	// write vertex list (separated by spaces)
	for(size_t i = 0; i < V.size(); i++)
	{
		out << std::fixed << std::setprecision(8) << V[i]->get_position()[0] << " "
						<< V[i]->get_position()[1] << " "
						<< V[i]->get_position()[2];

		// XXX
		if(V[i]->is_on_boundary())
			out << " 255 0 0\n";
		else
			out << " 0 255 0\n";
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

bool mesh::load_obj(std::istream &in)
{
	if(!in.good())
		return(false);

	std::string line;
	std::string keyword;
	std::istringstream converter;

	// These are specify the only keywords of the .OBJ file that the parse
	// is going to understand

	const std::string OBJ_KEY_VERTEX	= "v";
	const std::string OBJ_KEY_FACE		= "f";

	while(!std::getline(in, line).eof())
	{
		converter.str(line);
		converter >> keyword;

		if(keyword == OBJ_KEY_VERTEX)
		{
			double x, y, z;
			converter >> x >> y >> z;

			if(converter.fail())
			{
				std::cerr	<< "psalm: I tried to parse vertex coordinates from line \""
						<< line
						<<" \" and failed.\n";
				return(false);
			}

			add_vertex(x,y,z);
		}
		else if(keyword == OBJ_KEY_FACE)
		{
			std::vector<vertex*> vertices;

			// Check whether it is a triplet data string
			if(line.find_first_of('/') != std::string::npos)
			{
				while(!converter.eof())
				{
					std::string index_str;
					converter >> index_str;

					if(index_str.length() == 0)
						continue;

					size_t slash_pos = index_str.find_first_of('/'); // only interested in first occurrence

					// Contains the index as a string (not
					// including the '/'). The other
					// attributes (normals, textures
					// coordinates) are _removed_ here.
					std::istringstream index_conv(index_str.substr(0, slash_pos));

					long index = 0;
					index_conv >> index;

					if(index < 0)
					{
						std::cerr << "psalm: Handling of negative indices not yet implemented.\n";
						return(false);
					}
					else
						vertices.push_back(get_vertex(index-1));
				}

				add_face(vertices);
			}
			else
			{
				long index = 0;
				while(!converter.eof())
				{
					index = 0;
					converter >> index;

					if(index == 0)
					{
						std::cerr	<< "psalm: I cannot parse face data from line \""
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
							std::cerr	<< "psalm: Invalid backwards vertex reference "
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

bool mesh::save_obj(std::ostream& out)
{
	if(!out.good())
		return(false);

	for(std::vector<vertex*>::const_iterator it = V.begin(); it != V.end(); it++)
	{
		v3ctor position = (*it)->get_position();
		out << "v "	<< position[0] << " "
				<< position[1] << " "
				<< position[2] << "\n";
	}

	for(std::vector<face*>::const_iterator it = F.begin(); it != F.end(); it++)
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

bool mesh::load_off(std::istream& in)
{
	if(!in.good())
		return(false);

	std::string line;
	std::istringstream converter;

	/*
		Read "header", i.e.,

			OFF
			num_vertices num_faces num_edges

		where num_edges is ignored.
	*/

	std::getline(in, line);
	if(line != "OFF")
	{
		std::cerr << "psalm: I am missing a \"OFF\" header for the input data.\n";
		return(false);
	}

	size_t num_vertices, num_faces, num_edges;
	size_t cur_line_num = 0; // count line numbers (after header)

	std::getline(in, line);
	converter.str(line);
	converter >> num_vertices >> num_faces >> num_edges;

	if(converter.fail())
	{
		std::cerr << "psalm: I cannot parse vertex, face, and edge numbers from \"" << line << "\"\n";
		return(false);
	}

	converter.clear();
	line.clear();

	// These are specify the only keywords of the .OBJ file that the parse
	// is going to understand

	while(!std::getline(in, line).eof())
	{
		converter.str(line);

		if(cur_line_num < num_vertices)
		{
			double x, y, z;
			converter >> x >> y >> z;

			if(converter.fail())
			{
				std::cerr	<< "psalm: I tried to parse vertex coordinates from line \""
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

			std::vector<vertex*> vertices;
			for(size_t i = 0; i < k; i++)
			{
				converter >> index;
				if(converter.fail())
				{
					std::cerr	<< "psalm: Tried to parse face data in line \""
							<< line
							<< "\", but failed.\n";
					return(false);
				}

				if(index >= V.size())
				{
					std::cerr	<< "psalm: Index " << index << "in line \""
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
			std::cerr << "psalm: Got an unexpected data line \"" << line << "\".\n";
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

bool mesh::save_off(std::ostream& out)
{
	if(!out.good())
		return(false);

	out	<< "OFF\n"
		<< V.size() << " " << F.size() << " " << "0\n"; // For programs that actually interpret edge data,
								// the last parameter should be changed

	for(std::vector<vertex*>::const_iterator it = V.begin(); it != V.end(); it++)
	{
		v3ctor position = (*it)->get_position();
		out	<< position[0] << " "
			<< position[1] << " "
			<< position[2] << "\n";
	}

	for(std::vector<face*>::const_iterator it = F.begin(); it != F.end(); it++)
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
*	Saves the mesh in a special format for holes. The file format is
*	reminiscent of Wavefront OBJ: First, a list of non-boundary vertices is
*	written in the form "v x y z". Afterwards, faces are written out as "f
*	i j k", where i,j,k refers to the vertices that form the face. A
*	negative vertex index indicates a boundary vertex that has been left
*	out of the vertex list. A positive vertex index, starting at 0, refers
*	to the vertices that were written to the file.
*
*	@param	out Stream for data output
*	@return	true if the mesh could be stored, else false.
*/

bool mesh::save_hole(std::ostream& out)
{
	size_t num_boundary_vertices = 0;
	for(std::vector<vertex*>::const_iterator v_it = V.begin(); v_it < V.end(); v_it++)
	{
		vertex* v = *v_it;
		if(v->is_on_boundary())
			num_boundary_vertices++;
		else
			out << "v " << v->get_position();
	}

	// num_boundary_vertices is used to adjust the offsets of vertices when
	// writing the indexed faces
	for(std::vector<face*>::const_iterator f_it = F.begin(); f_it < F.end(); f_it++)
	{
		out << "f ";
		for(size_t i = 0; i < (*f_it)->num_vertices(); i++)
		{
			vertex* v = (*f_it)->get_vertex(i);
			if(v->is_on_boundary())
				out << "-" << v->get_id();
			else
				// If the offset is subtracted, all new vertices are in
				// the range of [num_boundary_vertices, ...]. Hence, the
				// range is adjusted by subtracting num_boundary_vertices
				// again.
				out << (v->get_id() - id_offset - num_boundary_vertices);

			// No trailing spaces for the last entry
			if(i < (*f_it)->num_vertices()-1)
				out << " ";
		}
		out << "\n";
	}

	return(true);
}

/*!
*	Destroys the mesh and all related data structures and frees up used
*	memory.
*/

void mesh::destroy()
{
	for(std::vector<vertex*>::iterator it = V.begin(); it != V.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	for(std::vector<edge*>::iterator it = E.begin(); it != E.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	for(std::vector<face*>::iterator it = F.begin(); it != F.end(); it++)
	{
		if(*it != NULL)
			delete(*it);
	}

	V.clear();
	E.clear();
	F.clear();

	E_M.clear();
}

/*!
*	Replaces the current mesh with another one. The other mesh will
*	be deleted/cleared by this operation.
*
*	@param	M Mesh to replace current mesh with
*/

void mesh::replace_with(mesh& M)
{
	this->destroy();

	this->V		= M.V;
	this->F		= M.F;
	this->E		= M.E;
	this->E_M	= M.E_M;

	// Options will _not_ be overwritten by this operation; previously this
	// was the case.

	// Clear old mesh
	M.V.clear();
	M.F.clear();
	M.E.clear();
	M.E_M.clear();
}

/*!
*	Calculates the density of a triangular mesh by dividing the number of
*	vertices by the area of the mesh.
*
*	@return	density = num_vertices / area or 0.0 if the area of the mesh is
*		zero
*/

double mesh::get_density() // XXX: Should be a `const` function
{
	double area = 0.0;
	for(size_t i = 0; i < num_faces(); i++)
	{
		const face* f = get_face(i);

		v3ctor a = f->get_vertex(1)->get_position() - f->get_vertex(0)->get_position();
		v3ctor b = f->get_vertex(2)->get_position() - f->get_vertex(0)->get_position();

		area += 0.5*(a|b).length();
	}

	if(area != 0.0)
		return(num_vertices()/area);
	else
		return(0.0);
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
*	@param ignore_orientation_warning Instructs the function to ignore any
*	warnings that _may_ indicate the wrong orientation. The reason for this
*	switch is that an algorithm might _remove_ existing faces. If new faces
*	(using a subset of the edges of the removed face) are added, the
*	function will complain because this would seem like _changing_ the
*	existing orientation. If the flag is set to true, this warning will not
*	appear.
*
*	@warning The orientation of the vertices around the face is _not_
*	checked, but left as a task for the calling function.
*
*	@returns Pointer to new face
*/

face* mesh::add_face(std::vector<vertex*> vertices, bool ignore_orientiation_warning)
{
	static bool warning_shown = false;
	if(ignore_orientiation_warning)
		warning_shown = true;

	vertex* u = NULL;
	vertex* v = NULL;

	if(vertices.size() == 0)
		return(NULL);

	face* f = new face;

	std::vector<vertex*>::iterator it = vertices.begin();
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
			Potential problem: I am assuming that the edges are
			ordered properly. Hence, an edge is supposed to appear
			only _once_ in a fixed direction. If this is not the
			case, the lookup below will _fail_ or an already stored
			face might be overwritten!
		*/

		// Edge already known; update second adjacent face
		if(edge.inverted)
		{
			if(edge.e->get_g() == NULL && edge.e->get_f() != NULL)
				edge.e->set_g(f);

			// If the face has been replaced by another face, the
			// _first_ face might be set to NULL. This is relevant
			// for Liepa's subdivision scheme, for example.
			else if(edge.e->get_f() == NULL)
				edge.e->set_f(f);

			// In this case, we cannot proceed -- the mesh would become degenerate
			else if(edge.e->get_g() != NULL)
			{
				std::cerr << "psalm: Error: mesh::add_face(): Attempted overwrite of the face references of an edge\n";
				return(NULL);
			}

			u->add_face(f);
		}

		// (Possibly) new edge; update the first adjacent face and adjacent
		// vertices
		else
		{
			if(edge.new_edge)
			{
				edge.e->set_f(f);
				u->add_edge(edge.e);
				v->add_edge(edge.e);
			}
			else
			{
				if(!warning_shown)
				{
					std::cerr << "psalm: Warning: Wrong orientation in mesh--results may be inconsistent.\n";
					warning_shown = true;
				}

				if(edge.e->get_f())
					edge.e->set_g(f);

				// If the face has been replaced by another
				// face, the _first_ face might be set to NULL.
				else
					edge.e->set_f(f);
			}

			u->add_face(f);
		}

		// Set next start vertex; the orientation should be correct
		// here
		u = v;
	}

	F.push_back(f);
	return(f);
}

/*!
*	Removes a given face from the mesh. This deletes _all_ pointers to the
*	face from adjacent vertices or adjacent faces. As a last step, the face
*	is removed from the face vector of the mesh.
*
*	@param	f Face that is going to be removed from the mesh
*
*	@throws std::runtime_error if the face could not be found in the face
*	vector. This signifies a very degenerate situation.
*/

void mesh::remove_face(face* f)
{
	// Remove face from face vector

	std::vector<face*>::iterator face_pos = std::find(F.begin(), F.end(), f);
	if(face_pos == F.end())
		throw(std::runtime_error("mesh::remove_face(): Unable to find face in face vector"));
	else
		F.erase(face_pos);

	// Remove references to face from edges

	for(size_t i = 0; i < f->num_edges(); i++)
	{
		directed_edge& d_e = f->get_edge(i);
		if(d_e.e->get_f() == f)
			d_e.e->set_f(NULL);
		else if(d_e.e->get_g() == f)
			d_e.e->set_g(NULL);
		else
			throw(std::runtime_error("mesh::remove_face(): Unable to find reference to face in edge vector"));
	}

	// Remove references to face from vertices
	for(size_t i = 0; i < f->num_vertices(); i++)
		f->get_vertex(i)->remove_face(f);
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

directed_edge mesh::add_edge(vertex* u, vertex* v)
{
	directed_edge result;

	/*
		The vertex IDs are combined into an std::pair. These pairs are
		then stored in an std::map container.

		Previously, the Cantor pairing function had been used, but this
		yielded integer overflows with normal 32bit integers. Hence,
		the std::string attempt is better suited to this task, although
		it requires a conversion step.
	*/

	std::pair<size_t, size_t> id = calc_edge_id(u, v);

	// Check whether edge exists
	std::map<std::pair<size_t, size_t>, edge*>::iterator edge_it;
	if((edge_it = E_M.find(id)) == E_M.end())
	{
		// Edge not found, create an edge from the _original_ edge and
		// add it to the map
		edge* new_edge = new edge(u, v);
		E.push_back(new_edge);
		E_M[id] = new_edge;

		result.e = new_edge;
		result.inverted = false;
		result.new_edge = true;
	}
	else
	{
		// Edge has been found, check whether the proper direction has
		// been stored.
		if(edge_it->second->get_u() != u)
			result.inverted = true;
		else
			result.inverted = false;

		result.new_edge = false;
		result.e = edge_it->second;
	}

	return(result);
}

/*!
*	Removes a given edge from the mesh. This deletes _all_ pointers to the
*	edge from adjacent vertices. As a last step, the edge is removed from
*	the edge map of the mesh.
*
*	@param	e Edge that is going to be removed from the mesh
*
*	@throws	std::runtime_error if degenerate situations occur (edge cannot
*		be found, edge is still referenced in faces etc.).
*
*	@warning	This function does not update the adjacent faces of the
*			edge. It is assumed that both the first and the second
*			face of the edge are already NULL. If this is not the
*			case, an exception is thrown.
*
*/

void mesh::remove_edge(edge* e)
{
	// Check feasibility of edge removal. If references still exist, the
	// edge may not be removed.
	if(e->get_f() || e->get_g())
		throw(std::runtime_error("mesh::remove_edge(): Edge is still referenced in faces"));

	// Remove edge from edge vector

	std::vector<edge*>::iterator edge_pos = std::find(E.begin(), E.end(), e);
	if(edge_pos == E.end())
		throw(std::runtime_error("mesh::remove_edge(): Unable to find edge in edge vector"));
	else
		E.erase(edge_pos);

	// Remove edge from edge map

	vertex* u = const_cast<vertex*>(e->get_u()); // XXX: Can this be solved better?
	vertex* v = const_cast<vertex*>(e->get_v());

	std::pair<size_t, size_t> edge_id = calc_edge_id(u, v);

	std::map<std::pair<size_t,size_t>, edge*>::iterator edge_map_pos = E_M.find(edge_id);
	if(edge_map_pos == E_M.end())
		throw(std::runtime_error("mesh::remove_edge(): Unable to find edge in edge map"));
	else
		E_M.erase(edge_id);

	// Remove reference of edge from start and end vertex. This is
	// necessary to avoid stale pointers.

	u->remove_edge(e);
	v->remove_edge(e);
}

/*!
*	Performs several pruning operations on the current mesh:
*
*		- Removal of faces with n sides
*		- Removal of faces with n sides
*
*	@param remove_faces	Contains a set of numbers. If a face with n
*				sides is found and n matches one of these
*				numbers, the face will be removed from the
*				mesh.
*
*	@param remove_vertices	Ditto; removes vertices with valency n.
*/

void mesh::prune(const std::set<size_t>& remove_faces, const std::set<size_t>& remove_vertices)
{
	if(remove_faces.size() == 0 && remove_vertices.size() == 0)
		return;

	for(std::vector<face*>::iterator it = F.begin(); it != F.end(); it++)
	{
		if(remove_faces.find((*it)->num_edges()) != remove_faces.end())
			it = F.erase(it);
	}

	for(std::vector<vertex*>:: iterator it = V.begin(); it != V.end(); it++)
	{
		if(remove_vertices.find((*it)->num_adjacent_faces()) != remove_vertices.end())
		{
			// Remove all adjacent faces
			for(size_t i = 0; i < (*it)->num_adjacent_faces(); i++)
			{
				const face* f = (*it)->get_face(i);
				std::vector<face*>::iterator it_f = std::find(F.begin(), F.end(), f);
				if(it_f != F.end())
					it_f = F.erase(it_f);
			}

			// Do not remove vertex; otherwise all vertex IDs would
			// need to be changed
		}
	}
}

/*!
*	Relaxes an edge by performing the following test:
*
*		-	Only run if the edge is adjacent to two triangles
*		-	Check that each of the two non-mutual vertices of
*			the triangles lie outside the circumcircle of the
*			opposing triangle
*		-	If this test fails, swap the edge
*
*	@param e Edge to relax
*	@returns Whether the edge has been relaxed
*/

bool mesh::relax_edge(edge* e)
{
	if(!e->get_f() || !e->get_g())
		return(false);

	face* faces[3];
	faces[0] = e->get_f();
	faces[1] = e->get_g();
	faces[2] = e->get_f();	// repeated so that faces[i+1] is well-defined
				// within the for-loop

	// Ensure that faces are triangles; otherwise, do not relax the edge.
	// We do not throw an exception here because edge relaxation may be
	// performed locally.
	if(	faces[0]->num_edges() != 3 ||
		faces[1]->num_edges() != 3)
		return(false);

	// These are the corresponding vertices of the _other_ face, i.e. v1 is
	// the vertex that is part of face g, but not part of face f, and vice
	// versa for vertex v2.
	vertex* v1 = NULL;
	vertex* v2 = NULL;

	bool swap = false;
	for(size_t i = 0; i < 2; i++)
	{
		// Compute circumcircle of triangle

		const v3ctor& A = faces[i]->get_vertex(0)->get_position();
		const v3ctor& B = faces[i]->get_vertex(1)->get_position();
		const v3ctor& C = faces[i]->get_vertex(2)->get_position();

		v3ctor a = A-C;
		v3ctor b = B-C;

		double theta = acos(a.normalize()*b.normalize());	// interior angle between a and b
		double r = (A-B).length()/(2*sin(theta));		// circumradius

		if(r == std::numeric_limits<double>::infinity() ||
			std::isnan(r) ||
			theta == 0.0)
		{
			return(false);
		}

		v3ctor d = (a|b);	// vector perpendicular to a and b; used in
					// the formula below
		double d_len = d.length();
		if(d_len == 0.0)
			return(false);

		// Compute circumcenter
		v3ctor c = (b*a.length()*a.length() - a*b.length()*b.length()) | d;
		c /= 2*d_len*d_len;
		c += C;

		// Find remaining vertex...
		for(size_t j = 0; j < 3; j++)
		{
			vertex* v = faces[i+1]->get_vertex(j);
			if(v != e->get_u() && v != e->get_v())
			{
				// ...and check whether it is outside the circumcircle
				swap = (v->get_position() - c).length() < r - 5*std::numeric_limits<double>::epsilon();	// Why we used a factor of 5 times epsilon is left as an easy
															// exercise to the reader

				// Set new vertices
				if(v1)
					v2 = v;
				else
					v1 = v;

				break;
			}
		}
	}

	// If this occurs, the mesh has become degenerate and no edge swap may
	// be performed.
	if(v1 == v2)
	{
		std::cerr << "psalm: Error: mesh::relax_edge(): Mesh is degenerate -- cannot swap edge\n";
		return(false);
	}

	if(!swap)
		return(false);

	// XXX: Check if this might lead to problems

	// Check whether the edge that is going to be swapped already exists.
	// In this case, the edge swap is also denied, as it would overwrite
	// existing faces
	if(E_M.find(calc_edge_id(v1, v2)) != E_M.end()) // TODO: Optimize
		return(false);

	// Remove both of the old faces and the corresponding edge...

	face* old_face_1 = e->get_f();
	face* old_face_2 = e->get_g();

	remove_face(old_face_1);
	remove_face(old_face_2);

	remove_edge(e);

	// ...find the remaining pair of vertices and create the new faces...

	std::pair<vertex*, vertex*> vertices_1st_face = find_remaining_vertices(e->get_v(), old_face_1);
	std::pair<vertex*, vertex*> vertices_2nd_face = find_remaining_vertices(e->get_u(), old_face_2);

	add_face(vertices_1st_face.first, vertices_1st_face.second, v1, true);
	add_face(vertices_2nd_face.first, vertices_2nd_face.second, v2, true);

	// ...and free some memory.

	delete(old_face_1);
	delete(old_face_2);
	delete(e);

	return(true);
}

/*!
*	Given a vertex and a triangular face (where the edge is supposed to be
*	part of the face), return the two vertices that remain after removing
*	the given vertex in correct order. Correct order means that the
*	vertices are pushed in a vector depending on the edges of the face,
*	i.e. if an edge (u,v) is part of the face f, vertex u will be the
*	directed predecessor of the vertex v in the results vector.
*
*	@param v Vertex that is assumed to be removed from the face
*	@param f Face of which vertex v is a part of
*
*	@returns A pair of vertices, sorted in correct order.
*
*	@throws	std::runtime_error if the function is applied to non-triangular
*		faces.
*/

std::pair<vertex*, vertex*> mesh::find_remaining_vertices(const vertex* v, const face* f)
{
	std::pair<vertex*, vertex*> res;
	res.first	= NULL;
	res.second	= NULL;

	if(v == NULL || f == NULL)
		return(res);

	if(f->num_edges() != 3)
		throw(std::runtime_error("mesh::find_remaining_vertices(): Number of edges != 3"));

	for(size_t i = 0; i < 3; i++)
	{
		// Check for candidate edges, i.e. edges that do _not_ contain
		// the vertex that is going to be removed
		const directed_edge& d_e = f->get_edge(i);
		if(d_e.e->get_v() != v && d_e.e->get_u() != v)
		{
			if(d_e.inverted)
			{
				res.first	= const_cast<vertex*>(d_e.e->get_v()); // XXX: Can this be solved better?
				res.second	= const_cast<vertex*>(d_e.e->get_u());
			}
			else
			{
				res.first	= const_cast<vertex*>(d_e.e->get_u());
				res.second	= const_cast<vertex*>(d_e.e->get_v());
			}

			break;
		}
	}

	return(res);
}

/*!
*	Marks all boundary vertices and edges in the mesh.
*/

void mesh::mark_boundaries()
{
	for(size_t i = 0; i < V.size(); i++)
	{
		for(size_t j = 0; j < V[i]->valency(); j++)
		{
			edge* e = V[i]->get_edge(j);

			// checking using get_g() is sufficient as get_f()
			// returns the _first_ face an edge is part of, and all
			// edges are assumed to be part of at least one face.
			if(e->get_g() == NULL)
			{
				e->set_on_boundary();
				e->get_f()->set_on_boundary();

				V[i]->set_on_boundary();
			}
		}
	}
}

/*!
*	Creates a mesh from raw input data. This means that all coordinates and
*	vertex IDs are stored in arrays instead of files. If the `coordinates`
*	pointer is NULL, the function will not change anything. Otherwise, the
*	current mesh will be _destroyed_.
*
*	@param num_vertices	Number of vertices
*
*	@param vertex_IDs	Array of vertex IDs. If the pointer is NULL,
*				the mesh will assign IDs automatically,
*				starting with 0. In this case, the ID offset is
*				_not_ calculating. This behaviour is ideal for
*				processing files: Since vertices are numbered
*				sequentially, the mesh can be written to a file
*				again.
*
*	@param coordinates	Array of vertex coordinates (coordinates for the
*				i-th vertex are stored at 3*i, 3*i+1, 3*i+2)
*
*	@param scale_attributes	Array of scale attributes for each vertex, i.e.
*				the average length of edges incident on the
*				vertex.
*
*	@param normals		Array of normal coordinates (stored just like
*				the `coordinates` array)
*
*	@returns true if data could be loaded, else false.
*/

bool mesh::load_raw_data(int num_vertices, long* vertex_IDs, double* coordinates, double* scale_attributes, double* normals)
{
	if(!coordinates)
		return(false);

	destroy();
	long max_id = 0;
	for(int i = 0; i < num_vertices; i++)
	{
		double nx, ny, nz;
		if(normals)
		{
			nx = normals[3*i];
			ny = normals[3*i+1];
			nz = normals[3*i+2];
		}
		else
			nx = ny = nz = 0.0; // set default values if no normals are available

		long id;
		if(vertex_IDs)
		{
			id = vertex_IDs[i];
			if(id == 0)
			{
				std::cerr	<< "psalm: mesh::load_raw_data(): Vertex ID is 0 -- this will lead to problems. Aborting..."
						<< std::endl;

				return(false);
			}
		}
		else
			id = i;

		vertex * v = add_vertex(	coordinates[3*i],
						coordinates[3*i+1],
						coordinates[3*i+2],
						nx,
						ny,
						nz,
						id);

		// Set scale attributes if present. Otherwise, the scale
		// attributes will be calculated by the subdivision algorithm.
		if(scale_attributes)
			v->set_scale_attribute(scale_attributes[i]);

		// Only update vertex IDs if the user explicitly specified an
		// array. Otherwise, IDs will be assigned sequentially.
		if(vertex_IDs && vertex_IDs[i] > max_id)
			max_id = vertex_IDs[i];
	}

	// Offset is only set if vertex IDs are present
	if(vertex_IDs)
	{
		// The IDs of new vertices must be larger than the IDs of their
		// predecessors. Otherwise, ID clashes will occur. The id_offset is
		// used for every mesh::add_vertex() operation.
		id_offset = static_cast<size_t>(max_id);
	}
	else
		id_offset = 0; // forces vertices to be numbered sequentially

	return(true);
}

/*!
*	Saves the current mesh in a raw format, i.e. all coordinate and
*	connectivity information is stored in arrays. The caller of this method
*	is notified about the number of new vertices and new faces created by
*	this method. New vertices are recognized by checking their boundary
*	flags. All boundary vertices are old vertices and will _not_ be
*	reported by this function.
*
*	The function assumes that the mesh consists of triangular faces only
*	and will only return triangles.
*
*	@param num_new_vertices	Number of new (i.e. non-boundary) vertices
*	@param new_coordinates	Array of new coordinates
*	@param num_faces	Number of faces in the mesh
*	@param vertex_IDs	Face connectivity information -- a negative ID
*				signifies an old vertex. This has to be taken
*				into account by the caller.
*
*	@returns true if data could be saved, else false.
*/

bool mesh::save_raw_data(int* num_new_vertices, double** new_coordinates, int* num_faces, long** vertex_IDs)
{
	size_t num_boundary_vertices = 0;		// count boundary vertices to obtain correct IDs
	std::vector<const vertex*> new_vertices;	// stores new vertices

	// Count new vertices and store them

	for(std::vector<vertex*>::const_iterator v_it = V.begin(); v_it < V.end(); v_it++)
	{
		vertex* v = *v_it;

		// Boundary vertex, hence an _old_ vertex, i.e. one that is
		// already known by the caller
		if(v->is_on_boundary())
			num_boundary_vertices++;

		// New vertex
		else
			new_vertices.push_back(v);
	}

	*num_new_vertices = new_vertices.size();
	*new_coordinates = new double[3*new_vertices.size()];
	for(size_t position = 0; position < new_vertices.size(); position++)
	{
		const v3ctor& v = new_vertices[position]->get_position();

		(*new_coordinates)[position*3]		= v[0];
		(*new_coordinates)[position*3+1]	= v[1];
		(*new_coordinates)[position*3+2]	= v[2];
	}

	// Allocate storage for new faces and store them -- the IDs of their
	// vertices need to be changed

	*num_faces = F.size();
	*vertex_IDs = new long[3*F.size()];
	for(size_t face_index = 0; face_index < F.size(); face_index++)
	{
		face* f = F[face_index];

		// Hole is assumed to consist of triangular faces only
		if(f->num_vertices() != 3)
		{
			std::cerr	<< "psalm: mesh::save_raw_data(): Unable to handle non-triangular faces"
					<< std::endl;
			return(false);
		}

		for(size_t i = 0; i < 3; i++)
		{
			vertex* v = f->get_vertex(i);

			// Store negative IDs for old vertices
			if(v->is_on_boundary())
				(*vertex_IDs)[3*face_index+i] = static_cast<long>(-1*v->get_id());
			else
			{
				// Store zero-indexed IDs for new vertices. For
				// this purpose, the offset needs to be
				// subtracted. This yields vertex IDs in the
				// range of [num_boundary, ...]. Hence, the
				// range needs to be adjusted by subtracting
				// num_boundary_vertices

				(*vertex_IDs)[3*face_index+i] = static_cast<long>(v->get_id() - id_offset - num_boundary_vertices);
			}
		}
	}

	return(true);
}

} // end of namespace "psalm"
