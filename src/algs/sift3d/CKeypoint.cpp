#include "CKeypoint.h"

namespace sift3dlib
{
	CKeypoint::CKeypoint(const CKeypoint &keypoint)
	{
      //if (this != &keypoint)
      {
         m_x     = keypoint.getX();
         m_y     = keypoint.getY();
         m_z     = keypoint.getZ();
         m_orientation1  = keypoint.getOrientation1();
         m_orientation2  = keypoint.getOrientation2();
         m_imgScale      = keypoint.getImgScale();
         m_scale         = keypoint.getScale();
         m_hasFV         = keypoint.getHasFV();
         m_featureVector = keypoint.getFeatureVector();
      }
	}

	// Keypoint constructor.
	//
	// image: The smoothed gaussian image the keypoint was located in.
	// x, y: The subpixel level coordinates of the keypoint.
	// imgScale: The scale of the gaussian image, with 1.0 being the original
	//    detail scale (source image), and doubling at each octave.
	// kpScale: The scale of the keypoint.
	// orientation: Orientation degree in the range of [-PI ; PI] of the
	//    keypoint.
	//
	// First add a keypoint, then use 'MakeDescriptor' to generate the local
	// image descriptor for this keypoint.
	CKeypoint::CKeypoint (double x, double y, double z, double imgScale, double kpScale, double orientation1, double orientation2) :
      m_x(x),
      m_y(y),
      m_z(z),
      m_imgScale(imgScale),
      m_scale(kpScale),
      m_orientation1(orientation1),
      m_orientation2(orientation2)
	{
	}

	CKeypoint &CKeypoint::operator= (const CKeypoint &key)
	{
		if (this != &key)
		{
			m_x = key.getX();
			m_y = key.getY();
			m_z = key.getZ();
			m_scale = key.getScale();
			m_imgScale = key.getImgScale();
			m_orientation1 = key.getOrientation1();
			m_orientation2 = key.getOrientation2();
			m_hasFV = key.getHasFV();

			if (key.getHasFV())
				CreateVector(key.getXDim(), key.getYDim(), key.getZDim(), key.getHistWidth(), key.getHistWidth(), key.getODim());
		}

		return *this;
	}

	double CKeypoint::FVGet(int x, int y, int z, int xb, int yb)
	{
		int pos = (x + (m_xDim * y) + (m_xDim * m_yDim * z)) * (m_histWidth * m_histHeight) + xb + yb * m_histWidth;
		return m_featureVector[pos];
	}

	void CKeypoint::FVSet(int x, int y, int z, int xb, int yb, double value)
	{
		int pos = (x + (m_xDim * y) + (m_xDim * m_yDim * z)) * (m_histWidth * m_histHeight) + xb + yb * m_histWidth;
		m_featureVector[pos] = value;
	}
}