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

#include <cfloat>
#include <cmath>

#include "v3ctor.h"

/*!
*	Constructor; sets components to zero.
*/

v3ctor::v3ctor()
{
	x = y = z = 0.0;
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
*	Allows element-wise access to vector.
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
			throw "Invalid access.";
	}
}

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
			throw "Invalid access.";
	}
}

/*!
*	Normalizes a vector.
*/

v3ctor v3ctor::normalize() const
{
	if(length() == 0)
		return(*this);
	else
		return(operator/(length()));
}

/*!
*	Computes standard Euclidean length, i.e., the norm, of a vector.
*/

double v3ctor::length() const
{
	return(sqrt(x*x+y*y+z*z));
}
