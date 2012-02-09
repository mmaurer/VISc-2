#ifndef CKEYPOINT_H
#define CKEYPOINT_H

#include <vector>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

namespace sift3dlib
{
   class CKeypoint
   {
   public:
      CKeypoint() :
         m_x(0.0),
         m_y(0.0),
         m_z(0.0),
         m_scale(0.0),
         m_imgScale(0.0),
         m_orientation1(0.0), 
         m_orientation2(0.0),
         m_hasFV(false),
         m_xDim(0),
         m_yDim(0),
         m_zDim(0),
         m_histWidth(0),
         m_histHeight(0),
         m_oDim(0)
      { }
      CKeypoint(const CKeypoint &keypoint); // Copy-constructor
      CKeypoint (double, double, double, double, double, double, double);
      ~CKeypoint() { }

      CKeypoint &operator= (const CKeypoint &key);

      void setX(double value) { m_x = value; }
      void setY(double value) { m_y = value; }
      void setZ(double value) { m_z = value; }
      void setScale(double value) { m_scale = value; }
      void setImgScale(double value) { m_imgScale = value; }
      void setOrientation1(double value) { m_orientation1 = value; }
      void setOrientation2(double value) { m_orientation2 = value; }
      void setHasFV(bool value) { m_hasFV = value; }
      void setFeatureVector(std::vector<double> value) { m_featureVector = value; }

      double getX() const { return m_x; }
      double getY() const { return m_y; }
      double getZ() const { return m_z; }
      double getScale() const { return m_scale; }
      double getImgScale() const { return m_imgScale; }
      double getOrientation1() const { return m_orientation1; }
      double getOrientation2() const { return m_orientation2; }
      bool getHasFV() const { return m_hasFV; }
      std::vector<double> getFeatureVector() const { return m_featureVector; }
      int getXDim() const { return m_xDim; }
      int getYDim() const { return m_yDim; }
      int getZDim() const { return m_zDim; }
      int getODim() const { return m_oDim; }
      int getHistWidth() const { return m_histWidth; }
      int getHistHeight() const { return m_histHeight; }

      double getFVLinearDim() { return m_featureVector.size(); }
      double FVLinearGet(int pos) { return m_featureVector[pos]; }
      void FVLinearSet(int pos, double value) { m_featureVector[pos] = value; }

      // I am not sure that o and oDim will be used anymore
      void CreateVector(int x, int y, int z, int bx, int by, int o)
      { 
         m_xDim = x; 
         m_yDim = y; 
         m_zDim = z; 
         m_histWidth = bx; 
         m_histHeight = by; 
         m_oDim = o; 
         m_hasFV = true; 
         m_featureVector.resize(m_xDim * m_yDim * m_zDim * m_histWidth * m_histHeight);
      }
      int DimensionCount() { return static_cast<int>(m_featureVector.size()); }
      double GetDimensionElement (int dim) { return FVLinearGet(dim); }

      double FVGet(int x, int y, int z, int xb, int yb);
      void FVSet(int x, int y, int z, int xb, int yb, double value);

   protected:
      double m_x, m_y, m_z;
      double m_scale;
      double m_imgScale;
      double m_orientation1, m_orientation2;
      bool m_hasFV;
      std::vector<double> m_featureVector;
      int m_xDim, m_yDim, m_zDim, m_histWidth, m_histHeight, m_oDim;
   };
}

#endif
