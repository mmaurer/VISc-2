#ifndef CVOXEL_H
#define CVOXEL_H

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

namespace nmMultiresolution
{
	template<typename T>
	class CVoxel
	{
		public:
			CVoxel() { x = y = z = 0; };
			CVoxel(T xp, T yp, T zp) { x = xp; y = yp; z = zp; };
			CVoxel(const CVoxel &v) { x = v.x; y = v.y; z = v.z; };

			CVoxel &operator=(CVoxel &v)
			{
				if (this != &v)
				{
					this->x = v.x;
					this->y = v.y;
					this->z = v.z;
				}
				return *this;
			}

			T x;
			T y;
			T z;
	};
}

#endif
