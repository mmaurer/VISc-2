/*
  VISc 2 is a simple volume renderer.

  Copyright 2012 Mauricio Maurer (mauricio.maurer@gmail.com)

  This file is part of VISc 2.

  VISc 2 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  VISc 2 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with VISc 2.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtOpenGL>

#include "CVirtualTrackball.h"

CVirtualTrackball::CVirtualTrackball()
{
	this->width = this->height = this->initx = this->inity = this->lastx = this->lasty = 0;
	max = (this->width > this->height) ? this->width : this->height;
	currentRotation.identity();
}
CVirtualTrackball::CVirtualTrackball(int w, int h)
{
	this->width = w;
	this->height = h;
	max = (this->width > this->height) ? this->width : this->height;
	this->initx = this->inity = this->lastx = this->lasty = 0;
	currentRotation.identity();
}

void CVirtualTrackball::setViewportDimensions(int w, int h)
{
	this->width = w;
	this->height = h;
	max = (w > h) ? w : h;
}

void CVirtualTrackball::setInitialPoint(int x, int y)
{
	this->initx = x;
	this->inity = y;
}

void CVirtualTrackball::setLastPoint(int x, int y)
{
	this->lastx = x;
	this->lasty = y;
}

void CVirtualTrackball::rotate(void)
{
	CVector initaxis, lastaxis;

	// Initial axis
	float xi = (2 * initx) - float(width);
	float yi = ((2 * (float(height) - inity)) - float(height));
	xi /= max;
	yi /= max;

	if ( sqrt(xi*xi + yi*yi) > 1.0)
		initaxis.setZ(0.0);
	else
		initaxis.setZ(sqrt(1 - xi*xi - yi*yi));

	initaxis.setX(xi);
	initaxis.setY(yi);

	// Last axis
	xi = (2 * lastx) - float(width);
	yi = ((2 * (float(height) - lasty)) - float(height));
	xi /= max;
	yi /= max;

	if ( sqrt(xi*xi + yi*yi) > 1.0)
		lastaxis.setZ(0.0);
	else
		lastaxis.setZ( sqrt(1 - xi*xi - yi*yi));

	lastaxis.setX(xi);
	lastaxis.setY(yi);

	// Rotating axis
	initaxis.normalize();
	lastaxis.normalize();

	CVector rotatingaxis;
	rotatingaxis = initaxis.crossProduct(lastaxis);

	rotatingaxis.normalize();

	// Angle of rotation
	double angle = acos( initaxis.dotProduct(lastaxis) );

	// Rotating matrix
	CQuaternion newRotation = CQuaternion(rotatingaxis, float(angle));
	CQuaternion tmp;
	tmp = currentRotation;
	currentRotation = tmp * newRotation;
}

void CVirtualTrackball::multiply(void)
{
	CVector	axis;		// temp axis angle representation
	float angle;

	currentRotation.getAxisAngle(axis, angle);		// get angle axis representation

	double ax, ay, az;
	ax = axis.getX();
	ay = axis.getY();
	az = axis.getZ();
	angle = static_cast<float>(VISc::MathUtils::RadiansToDegrees(angle));

	glRotatef(angle, static_cast<GLfloat>(ax), static_cast<GLfloat>(ay), static_cast<GLfloat>(az));		// multiply into matrix

}

void CVirtualTrackball::reset()
{
	currentRotation.identity();
}
