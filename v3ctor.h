/*!
*	@file	v3ctor.h
*	@brief	3D vector class (named by J. Portl)
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

#ifndef __V3CTOR_H__
#define __V3CTOR_H__

#include <ostream>

/*!
*	@class v3ctor
*	@brief Simple 3-dimensional vector class implementation. Allows some
*	common manipulations such as the scalar product or the cross product.
*/

class v3ctor
{
	public:
		v3ctor();
		v3ctor(double x, double y, double z);

		v3ctor  operator+ (const v3ctor& b) const;
		v3ctor& operator+=(const v3ctor& b);
		v3ctor  operator- (const v3ctor& b) const;
		v3ctor& operator-=(const v3ctor& b);
		v3ctor  operator* (const double& a) const;
		v3ctor& operator*=(const double& a);
		v3ctor  operator/ (const double& a) const;
		v3ctor& operator/=(const double& a);
		v3ctor& operator= (const v3ctor& b);
		v3ctor  operator| (const v3ctor& b) const;

		double		operator*(const v3ctor& a) const;
		double&		operator[](short i);
		const double&	operator[](short i) const;

		v3ctor normalize() const;
		double length() const;

		friend std::ostream& operator<<(std::ostream& o, const v3ctor& v);

	private:
		double x;
		double y;
		double z;
};

double distance_to_plane(const v3ctor& a, const v3ctor& b, const v3ctor& c, const v3ctor & x);
v3ctor perpendicular_foot(const v3ctor& a, const v3ctor& b, const v3ctor& c, const v3ctor& x);

double distance_to_line(const v3ctor& a, const v3ctor& b, const v3ctor& x);
v3ctor perpendicular_foot(const v3ctor& a, const v3ctor& b, const v3ctor& x);

#endif
