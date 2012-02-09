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
