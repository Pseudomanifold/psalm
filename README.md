[![Build Status](https://travis-ci.org/Submanifold/psalm.svg?branch=master)](https://travis-ci.org/Submanifold/psalm)

# NAME

**psalm** - pretty subdivision algorithms on meshes

## SYNOPSIS

**psalm** [*arguments*] [*file*]

## DESCRIPTION

**psalm** is a mesh compiler, using meshes in several data formats
(`OBJ`, `OFF`, `OBJ`) as its input, and generating _subdivided_ meshes
in turn.  Currently, Catmull-Clark, Doo-Sabin, and Loop subdivision
schemes have been implemented.

The user may change several parameters of the subdivision algorithm,
such as the *weights* that are used for extraordinary vertices of the
mesh. Furthermore, rudimentary pruning capabilitites have been
implemented.

**psalm** can work without any parameters. It will expect input from
`STDIN` and write its data to `STDOUT`. For displaying the status
(subdivision might take long), `STDERR` is used. The user may specify
`STDOUT` and `STDIN` by using a single dash, `-`, as the argument for
output and input operations.

If an input file has been specified, but not output file, the suffix
`.subdivided` will be added to the input file. For example, subdividing
a file `example.ply` without specifying an output file results in
`example.subdivided.ply`.

## OPTIONS

There are three categories of parameters: General, tuning, and pruning.
The general parameters are used for *psalm*'s basic operations. Tuning
parameters affect the internal workings of the algorithms. Pruning
parameters enable simple removal operations for meshes.

### General

- *-a, --algorithm* *&lt;algorithm&gt;*

	Selects the subdivision algorithm to use on the input mesh.
	Valid values for *algorithm* are:

	* `catmull-clark`, `catmull`, `clark`, `cc` [default]
	* `doo-sabin`, `doo`, `sabin`, `ds`
	* `loop`, `l`

- *-o, --output* *&lt;file&gt;*

	Sets output file. If not present, `STDOUT` will be used. A
	single dash will be interpreted as `STDOUT`.

- *-t, --type* *&lt;type&gt;*

	Sets type of input data. If not present, the type of input and
	output data is guessed from the file extension. Valid values for
	*&lt;type&gt;* are:

	* `obj` (Wavefront OBJ files)
	* `off` (Geomview object files)
	* `ply` (Stanford PLY files)

- *-n, --steps* *&lt;n&gt;*

	Sets number of subdivision steps to perform on the input mesh.
	This parameter is `0` by default.

- *-s, --statistics*

	Prints statistics and progress bars to `STDERR`.

- *-h, --help*

	Shows a help screen.

### Tuning

- *-b, --b-spline-weights*

	Instructs algorithms to use the normal B-spline weights for the
	regular subdivision case even if the current weight scheme would
	apply different weights. *This parameter affects the shape of
	the resulting surfaces.*

- *-c, --handle-creases*

	Algorithms try to subdivide crease and boundary edges whenever
	this is possible.

- *-p, --parametric*

	Forces algorithms to calculate new points using parametrical
	methods instead of geometrical methods. May affect the running
	time of the algorithm.

- *-w, --weights* *&lt;weights&gt;*

	Selects type of weights that are used for the subdivision
	algorithm.  Algorithms may choose to ignore this parameter.
	Valid values for
	*&lt;weights&gt;* are:

	* `catmull-clark`, `catmull`, `clark`, `cc`
	* `default`
	* `degenerate`
	* `doo-sabin`, `doo`, `sabin`, `ds`

- *-e, --extra-weights* *&lt;file&gt;*

	Overrides default weight of subdivision schemes by reading them from
	*&lt;file&gt;*. The precise format of this file depends on the subdivision
	algorithm that is used.

### Pruning

- *--remove-faces* *&lt;n1&gt;, &lt;n2&gt;, ...*

	Removes faces whose number of sides matches one of the numbers
	in the list. Use commas to separate list values.

- *--remove-vertices* *&lt;n1&gt;, &lt;n2&gt;, ...*

	Removes vertices whose valency matches one of the numbers in the
	list.  Use commas to separate list values.

## EXAMPLES

By default, no subdivision is performed. Thus, the following command may
be used to convert between `obj` and `ply`, for example: 

	psalm -o output.ply input.obj

Perform three steps of Catmull-Clark subdivision, writing to an output
file:

	psalm [-a cc] -n 3 -o output.ply input.ply

Ditto, but using Doo-Sabin weights:

	psalm [-a cc] -n 3 -w doo-sabin -o output.ply input.ply

Ditto, but using degenerate and B-spline weights, while removing all
triangular faces from the result: 

	psalm [-a cc] -n 3 -w degenerate -b --remove-faces 3 -o output.ply input.ply

Perform three steps of Doo-Sabin subdivision, writing to `STDOUT`:

	psalm -a ds -n 3 -o - input.ply

Ditto, but reading from `STDIN`:

	psalm -a ds -n 3 -o -

Ditto, but using parametrical point creation and handling creases:

	psalm -a ds -c -p -n 3 -o - input.ply

Using extra weights:

	psalm -a ds -e W_DS_Reif_1.txt -n 3 -o - input.ply

BUILDING PSALM
--------------

The following packages are required for building libpsalm:

* Recent version of cmake
* libboostX.XX-all-dev
* libboostX.XX-all


BUGS
----

*	The type of input and output files cannot be specified
	separately. This is by design.

*	*psalm* abuses `STDERR` for its status reports. Otherwise,
	`STDOUT` could not be used for the real output.

AUTHOR
------

*psalm* is developed by Bastian Rieck (onfgvna@evrpx.eh; use `rot13` to
descramble).

FILES
-----

*psalm* is shipped with several example meshes:

- `W_DS_Reif_[1-5].txt`: Extra weights from U. Reif's publication *A
  unified approach to subdivision algorithms near extraordinary
  vertices*
- `Hexahedron.off`: A cube in `OFF` format.
- `Hexahedron.ply`: A cube in `PLY` format. The mesh is used courtesy of
  [John Burkardt](http://people.sc.fsu.edu/~jburkardt).
- `Hole_[3,6].ply`: A 3-sided and a 6-sided hole used for testing the
  limit behaviour of the algorithms.
- `Icosahedron.ply`: An icosahedron in `PLY` format. The mesh is used
  courtesy of [John Burkardt](http://people.sc.fsu.edu/~jburkardt).
- `Icosahedron.obj`: The `PLY` file converted to `Wavefront OBJ` format
  by [MeshLab](http://meshlab.sourceforge.net).
- `Klein_Bottle.obj`: A Klein bottle in `Wavefront OBJ` format (using
  quadrangles). The mesh is provided courtesy of *Pierre Alliez* by the
  [AIM@SHAPE shape repository](http://shapes.aimatshape.net).
- `Klein_Bottle.ply`: A triangulated version of the Klein bottle mesh,
  created by [MeshLab](http://meshlab.sourceforge.net).
- `Surface.obj`: A simple regular surface.
- `Tetrahedron.obj`: A tetrahedron in `OBJ` format. The mesh is used
  courtesy of [John Burkardt](http://people.sc.fsu.edu/~jburkardt).

COPYRIGHT AND LICENCE
---------------------

Copyright 2010, Bastian Rieck. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

-	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.
- 	Redistributions in binary form must reproduce the above
	copyright notice, this list of conditions and the following
	disclaimer in the documentation and/or other materials provided
	with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
