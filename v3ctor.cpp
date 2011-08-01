/*
*	@file v3ctor.cpp
*	@brief Functions for 3-dimensional vector class
*
*	<HR>
*
*	Copyright 2010, Bastian Rieck. All rights reserved.
*
*	Redistribution and use in source and binary forms, with or without
*	modification, are permitted provided that the following conditions are
*	met:
*
*	-# Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*	-# Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in the
*	documentation and/or other materials provided with the distribution.
*
*	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
*	IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
*	TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
*	PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*	OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*	LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*	DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*	THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cmath>
#include <stdexcept>
#include <iomanip>

#include "v3ctor.h"

/*!
*	Constructor; sets components to zero.
*/

v3ctor::v3ctor()
{
	x = y = z = 0.0;
}

/*!
*	Initializes components with user-defined values.
*/

v3ctor::v3ctor(double x, double y, double z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}

/*!
*	Adds two vectors.
*/

v3ctor v3ctor::operator+(const v3ctor& b) const
{
	v3ctor res;

	res.x = x + b.x;
	res.y = y + b.y;
	res.z = z + b.z;

	return(res);
}

/*!
*	Adds vector to the current vector.
*/

v3ctor& v3ctor::operator+=(const v3ctor& b)
{
	x += b.x;
	y += b.y;
	z += b.z;

	return(*this);
}

/*!
*	Subtracts two vectors from one another.
*/

v3ctor v3ctor::operator-(const v3ctor& b) const
{
	v3ctor res;
	res.x = x - b.x;
	res.y = y - b.y;
	res.z = z - b.z;

	return(res);
}

/*!
*	Subtracts vector from current vector.
*/

v3ctor& v3ctor::operator-=(const v3ctor& b)
{
	x -= b.x;
	y -= b.y;
	z -= b.z;

	return(*this);
}

/*!
*	Multiplies vector by scalar.
*/

v3ctor v3ctor::operator*(const double& a) const
{
	v3ctor res;
	res.x = x*a;
	res.y = y*a;
	res.z = z*a;

	return(res);
}

/*!
*	Multiplies current vector by scalar value.
*/

v3ctor& v3ctor::operator*=(const double& a)
{
	x *= a;
	y *= a;
	z *= a;

	return(*this);
}

/*!
*	Divides vector by scalar value.
*/

v3ctor v3ctor::operator/(const double& a) const
{
	if(a == 0.0)
		throw "Attempted division by zero.\n";
	else
		return(operator*(1/a));
}

/*!
*	Divides current vector by scalar.
*/

v3ctor& v3ctor::operator/=(const double& a)
{
	if(a == 0.0)
		throw "Attempted division by zero.\n";
	else
		return(operator*=(1/a));
}

/*!
*	Assigns vector to current vector.
*/

v3ctor& v3ctor::operator=(const v3ctor& b)
{
	x = b.x;
	y = b.y;
	z = b.z;

	return(*this);
}

/*!
*	Computes standard euclidean scalar product of two vectors.
*/

double v3ctor::operator*(const v3ctor& b) const
{
	return(x*b.x + y*b.y + z*b.z);
}

/*!
*	Computes cross product of two vectors.
*/

v3ctor v3ctor::operator|(const v3ctor& b) const
{
	v3ctor res;

	res.x = y*b.z-z*b.y;
	res.y = z*b.x-x*b.z;
	res.z = x*b.y-y*b.x;

	return(res);
}

/*!
*	@param i Index of element to access.
*	@return Reference to element i of the vector. If the index is out of
*	bounds, an exception is thrown.
*/

double& v3ctor::operator[](short i)
{
	switch(i)
	{
		case 0:
			return(x);
		case 1:
			return(y);
		case 2:
			return(z);
		default:
			throw std::out_of_range("v3ctor::operator[](): Invalid element index");
	}
}

/*!
*	@param i Index of element to access.
*	@return Const reference to element i of the vector. If the index is out
*	of bounds, an exception is thrown.
*/

const double& v3ctor::operator[](short i) const
{
	switch(i)
	{
		case 0:
			return(x);
		case 1:
			return(y);
		case 2:
			return(z);
		default:
			throw std::out_of_range("v3ctor::operator[](): Invalid element index");
	}
}

/*!
*	Normalizes a vector.
*/

v3ctor v3ctor::normalize() const
{
	double len = length();
	if(len == 0)
		return(*this);
	else
		return(operator/(len));
}

/*!
*	Computes standard Euclidean length, i.e., the norm, of a vector.
*/

double v3ctor::length() const
{
	return(sqrt(x*x+y*y+z*z));
}

/*!
*	Provides a simple output capability for v3ctor objects: All
*	components of the vector are separated by spaces. Afterwards,
*	an std::endl will be added.
*
*	@warning	This function uses std::fixed and std::setprecision(8)
*			for the output stream.
*
*	@param	o	Stream for output
*	@param	v	V3ctor object for output
*
*	@return	Stream containing data of v3ctor v.
*/

std::ostream& operator<<(std::ostream& o, const v3ctor& v)
{
	return(o	<< std::fixed << std::setprecision(8)
			<< v.x << " "
			<< v.y << " "
			<< v.z << std::endl);
}

/*!
*	Calculates the distance between a plane given by three points and
*	another point.
*
*	@param a First point in plane
*	@param b Second point in plane
*	@param c Third point in plane
*	@param x Point for which the distance is to be determined
*
*	@return Absolute distance value
*/

double distance_to_plane(const v3ctor& a, const v3ctor& b, const v3ctor& c, const v3ctor & x)
{
	v3ctor normal = ((b-a)|(c-a)).normalize();
	return(fabs(normal*(x-a)));
}

/*!
*	Calculates the perpendicular foot of a point with respect to a plane
*	given by three points.
*
*	@param a First point in plane
*	@param b Second point in plane
*	@param c Third point in plane
*	@param x Point for which the perpendicular foot is to be determined
*
*	@return Position of perpendicular foot
*/

v3ctor perpendicular_foot(const v3ctor& a, const v3ctor& b, const v3ctor& c, const v3ctor& x)
{
	v3ctor normal = ((b-a)|(c-a)).normalize();
	double d = -(normal*a);

	return(normal*(-d)-normal*(x*normal)+x);
}

/*!
*	Calculates distance from a point to a line specified by two points.
*
*	@param a First point on line
*	@param b Second point on line
*	@param x Point for which the distance is to be determined
*
*	@return Absolute distance from point x to line given by a and b
*/

double distance_to_line(const v3ctor& a, const v3ctor& b, const v3ctor& x)
{
	double double_area = ((a-x)|(b-x)).length();
	double side_length = (a-b).length();

	return(double_area/side_length);
}

/*!
*	Calculates the perpendicular foot of a point with respect to a line
*	given by two points.
*
*	@param a First point on line
*	@param b Second point on line
*	@param x Point for which the perpendicular foot is to be determined
*
*	@return Position of perpendicular foot
*/

v3ctor perpendicular_foot(const v3ctor& a, const v3ctor& b, const v3ctor& x)
{
	double length = (b-a).length();
	double t = (a-x)*(b-a)/(length*length)*(-1.0);

	return(a+(b-a)*t);
}

