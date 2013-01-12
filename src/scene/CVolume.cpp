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

#include "CVolume.h"
#include "CVolumeProperties.h"

#include <QSharedPointer>

CVolume::CVolume() :
width(0),
height(0),
depth(0),
maxvalue(0),
c(1),
timeStamp(QDateTime::currentDateTime()),
enabled(true),
name(QObject::tr("Blank"))
{ 

}

// Copy-constructor
CVolume::CVolume (const CVolume &volume)
{
   width    = volume.width;
   height   = volume.height;
   depth    = volume.depth;
   c        = volume.c;
   maxvalue = volume.maxvalue;

   data.reset(new char[volume.totalVoxels()]);
   memcpy(data.data(), volume.data.data(), volume.totalVoxels());

   position.setX(volume.getPosition().getX());
   position.setY(volume.getPosition().getY());
   position.setZ(volume.getPosition().getZ());
   timeStamp = QDateTime::currentDateTime();
   enabled  = true;
   this->name = volume.name + QObject::tr(" Copy");
   this->aspectRatio[VISc::dX] = volume.getAspectRatio(VISc::dX);
   this->aspectRatio[VISc::dY] = volume.getAspectRatio(VISc::dY);
   this->aspectRatio[VISc::dZ] = volume.getAspectRatio(VISc::dZ);
   this->color = volume.getColor();
}

CVolume::CVolume (QString filename)
{
   width = height = depth = 0;
   c = 1;
   maxvalue = 0;

   read(filename);
   timeStamp = QDateTime::currentDateTime();
   enabled = true;
   name = filename;
   aspectRatio[VISc::dX] = aspectRatio[VISc::dY] = aspectRatio[VISc::dZ] = 1.0;
   color = qRgba(255, 255, 255, 255);
}

CVolume::CVolume (int w, int h, int d, int c)
{
   width    = w;
   height   = h;
   depth    = d;
   this->c  = c;
   maxvalue = 255;
   data.reset( new char[totalVoxels() * c] );

   for (int i = 0; i < totalVoxels() * c; i++)
      data[i] = 0;

   timeStamp = QDateTime::currentDateTime();
   enabled  = true;
   name = QObject::tr("Blank");
   aspectRatio[VISc::dX] = aspectRatio[VISc::dY] = aspectRatio[VISc::dZ] = 1.0;
   color = qRgba(255, 255, 255, 255);
}

CVolume::~CVolume()
{
   clear();
}

void CVolume::clear()
{
   width = height = depth = maxvalue = 0;
   freeData();
}

void CVolume::freeData()
{
   if (!data.isNull())
      data.reset( NULL );
}

bool CVolume::read(QString filename)
{
   clear();

   QFile file(filename);

   if (!file.open(QIODevice::ReadOnly))
      return false;

   char type;
   bool read = true;
   int state = 0;
   QStack<int> number;

   while (read)
   {
      file.getChar(&type);

      switch (state)
      {
      case 0: 		
         if (type != 'P')
            return false;
         state = 1;
         break;
      case 1:
         if (type != '7')
            return false;
         state = 2;
         break;
      case 2: if (ISDIGIT(type))
              {
                 file.ungetChar(type);
                 state = 3;
              } break;
      case 3: // x
         if (ISDIGIT(type))
            number.push(type - 48);
         else
         {
            width = stackToNumber(number);
            state = 4;
         } break;
      case 4: if (ISDIGIT(type))
              {
                 file.ungetChar(type);
                 state = 5;
              } break;
      case 5: // y
         if (ISDIGIT(type))
            number.push(type - 48);
         else
         {
            height = stackToNumber(number);
            state = 6;
         } break;
      case 6: if (ISDIGIT(type))
              {
                 file.ungetChar(type);
                 state = 7;
              } break;
      case 7: // z
         if (ISDIGIT(type))
            number.push(type - 48);
         else
         {
            depth = stackToNumber(number);
            state = 8;
         } break;
      case 8: if (ISDIGIT(type))
              {
                 file.ungetChar(type);
                 state = 9;
              } break;
      case 9: // depth
         if (ISDIGIT(type))
            number.push(type - 48);
         else
         {
            maxvalue = stackToNumber(number);
            state = 10;
         } break;
      case 10: if (type != ' ' && type != '\n' && type != 0x10)
               {
                  file.ungetChar(type);
                  read = false;
               } break;
      default: // Spaces and new lines
         break;
      }
   }

   // Read the rest of the volume
   data.reset( new char[totalVoxels()] );

   file.read(data.data(), totalVoxels());
   file.close();

   return true;
}

int CVolume::stackToNumber(QStack<int> &stack)
{
   int dec=1;
   int tmp = 0;

   while (!stack.isEmpty())
   {
      tmp += stack.pop() * dec;
      dec *= 10;
   }

   return tmp;
}

int CVolume::getVoxel(int position)
{
   if ((position >= totalVoxels()) || (position < 0))
      return 0;
   else
      return (unsigned char) this->data[position];
}

int CVolume::getVoxel(int x, int y, int z)
{
   if (x < 0 || x >= this->width)
      return 0;
   if (y < 0 || y >= this->height)
      return 0;
   if (z < 0 || z >= this->depth)
      return 0;
   int pos = (x) + (width*y) + (width*height*z);
   //char a = data[pos];
   //int b = unsigned char(data[pos]);
   return (unsigned char) data[pos];
}

double CVolume::getVoxel(CVector position)
{
   double xd, yd, zd;
   double xc, yc, zc;
   double xf, yf, zf;

   xd = position.getX() - floor( position.getX() );
   yd = position.getY() - floor( position.getY() );
   zd = position.getZ() - floor( position.getZ() );

   xf = floor( position.getX() );
   yf = floor( position.getY() );
   zf = floor( position.getZ() );

   xc = ceil( position.getX() );
   yc = ceil( position.getY() );
   zc = ceil( position.getZ() );

   double i1 = getVoxel(xf, yf, zf) * (1 - zd) + getVoxel(xf, yf, zc) * zd;
   double i2 = getVoxel(xf, yc, zf) * (1 - zd) + getVoxel(xf, yc, zc) * zd;
   double j1 = getVoxel(xc, yf, zf) * (1 - zd) + getVoxel(xc, yf, zc) * zd;
   double j2 = getVoxel(xc, yc, zf) * (1 - zd) + getVoxel(xc, yc, zc) * zd;

   double w1 = i1 * (1 - yd) + i2 * yd;
   double w2 = j1 * (1 - yd) + j2 * yd;

   return floor( (w1 * (1 - xd) + w2 * xd) + 0.5 );

}

void CVolume::setVoxel(int x, int y, int z, int value)
{
   if (x < 0 || x >= this->width)
      return;
   if (y < 0 || y >= this->height)
      return;
   if (z < 0 || z >= this->depth)
      return;
   int pos = ((x) + (width*y) + (width*height*z));
   data[pos] = value;
}

void CVolume::setVoxel(int position, int value)
{
   if ((position >= totalVoxels()) || (position < 0))
      return;

   data[position] = value;
}

void CVolume::setVoxel(int x, int y, int z, int value, int component)
{
   if (x < 0 || x >= this->width)
      return;
   if (y < 0 || y >= this->height)
      return;
   if (z < 0 || z >= this->depth)
      return;
   int pos = ((x) + (width*y) + (width*height*z)) * 4;
   pos += component;
   data[pos] = value;
}


CVolume &CVolume::operator= (CVolume &volume)
{
   if (this != &volume)
   {
      this->width = volume.getWidth();
      this->height = volume.getHeight();
      this->depth = volume.getDepth();
      this->maxvalue = volume.getMaxValue();
      this->name = volume.getName() + " Copy";

      data.reset( new char[volume.totalVoxels()] );
      memcpy(this->data.data(), volume.data.data(), volume.totalVoxels());
   }
   return *this;
}

QImage CVolume::getTexture(int slice, VISc::EDirection direction, GLubyte *)
{
#define CLAMP(x, min, max) ((x<min) ? min : ((x>max) ? max : x ))

   if (direction == VISc::dX)
   {
      QImage imaget(depth, height, QImage::Format_ARGB32);

      for (int z = 0; z < depth; z++)
         for (int y = 0; y < height; y++)
         {
            int x = slice;
            int zvalue = abs(getVoxel(x,   y,   z+1) - getVoxel(x,   y,   z-1));
            zvalue = CLAMP(zvalue, 0, 255);
            int yvalue = abs(getVoxel(x,   y+1, z)   - getVoxel(x,   y-1, z));
            yvalue = CLAMP(yvalue, 0, 255);
            int xvalue = abs(getVoxel(x+1, y,   z)   - getVoxel(x-1, y,   z));
            xvalue = CLAMP(xvalue, 0, 255);
            //int value = volume.getVoxel(x, y, z);

            unsigned char a = (unsigned char) getVoxel(slice, y, z);
            int t = a;
            //imaget.setPixel(z, y, qRgba(t, t, t, t));
            imaget.setPixel(z, y, qRgba(xvalue, yvalue, zvalue, t));
         }

         return imaget.mirrored(true, false);
   }
   else if (direction == VISc::dY)
   {
      QImage imaget(width, depth, QImage::Format_ARGB32);

      for (int z = 0; z < depth; z++)
         for (int x = 0; x < width; x++)
         {
            int y = slice;
            int zvalue = abs(getVoxel(x,  (height-1)- y,   z+1) - getVoxel(x,   (height-1) - y,   z-1));
            zvalue = CLAMP(zvalue, 0, 255);
            int yvalue = abs(getVoxel(x,   (height-1) - y+1, z)   - getVoxel(x,   (height-1) - y-1, z));
            yvalue = CLAMP(yvalue, 0, 255);
            int xvalue = abs(getVoxel(x+1, (height-1) - y,   z)   - getVoxel(x-1, (height-1) - y,   z));
            xvalue = CLAMP(xvalue, 0, 255);
            //int value = volume.getVoxel(x, y, z);

            unsigned char a = (unsigned char) getVoxel(x, (height-1) - slice, z);
            int t = a;
            //imaget.setPixel(x, z, qRgba(t, t, t, t));
            imaget.setPixel(x, z, qRgba(xvalue, yvalue, zvalue, t));
         }

         return imaget;
   }
   else if (direction == VISc::dZ)
   {
      QImage imaget(width, height, QImage::Format_ARGB32);

      for (int y = 0; y < height; y++)
         for (int x = 0; x < width; x++)
         {
            int z = slice;
            int zvalue = abs(getVoxel(x,   y,   z+1) - getVoxel(x,   y,   z-1));
            zvalue = CLAMP(zvalue, 0, 255);
            int yvalue = abs(getVoxel(x,   y+1, z)   - getVoxel(x,   y-1, z));
            yvalue = CLAMP(yvalue, 0, 255);
            int xvalue = abs(getVoxel(x+1, y,   z)   - getVoxel(x-1, y,   z));
            xvalue = CLAMP(xvalue, 0, 255);
            //int value = volume.getVoxel(x, y, z);

            unsigned char a = (unsigned char) getVoxel(x, y, slice);
            int t = a;
            //imaget.setPixel(x, y, qRgba(t, t, t, t));
            imaget.setPixel(x, y, qRgba(xvalue, yvalue, zvalue, t));
         }

         return imaget;
   }
   QImage foo;
   return foo;
}

void CVolume::properties()
{
   QSharedPointer<CVolumeProperties> properties( new CVolumeProperties(this) );
   properties->exec();
}

void CVolume::render()
{
   glPushMatrix();
   glTranslatef(-getWidth() / 2.0, -getHeight() / 2.0, -getDepth() / 2.0);
   glScalef(aspectRatio[VISc::dX], aspectRatio[VISc::dY],aspectRatio[VISc::dZ]);
   glTranslatef(position.getX(), position.getY(), position.getZ());

   glColor3f(color.redF(), color.greenF(), color.blueF());
   glBegin(GL_LINES);

   // Front icon
   glVertex3f(getWidth() * 0.7, getHeight() * 0.2, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.3, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.6, 0);
   glVertex3f(getWidth() * 0.3, getHeight() * 0.6, 0);

   // Front face
   glVertex3f(0, 0, 0);
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(0, getHeight(), 0);
   glVertex3f(0, getHeight(), 0);
   glVertex3f(0, 0, 0);

   // Left face
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, getDepth());
   glVertex3f(0, 0, getDepth());
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(0, getHeight(), 0);

   // Bottom face
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), 0, getDepth());
   glVertex3f(getWidth(), 0, getDepth());
   glVertex3f(0, 0, getDepth());

   // Right face
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(getWidth(), getHeight(), getDepth());
   glVertex3f(getWidth(), getHeight(), getDepth());
   glVertex3f(getWidth(), 0, getDepth());

   // Hinder face
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(getWidth(), getHeight(), getDepth());
   glEnd();
   glPopMatrix();
}

void CVolume::getBoundingBox(float *mw, float *pw, float *mh, float *ph, float *md, float *pd)
{
   *mw = position.getX() - this->getWidth() / 2.0;
   *pw = position.getX() + this->getWidth() / 2.0;
   *mh = position.getY() - this->getHeight() / 2.0;
   *ph = position.getY() + this->getHeight() / 2.0;
   *md = position.getZ() - this->getDepth() / 2.0;;
   *pd = position.getZ() + this->getDepth() / 2.0;;
}

CVolume CVolume::resize(double sx, double sy, double sz, VISc::EInterpolation im)
{
   assert(sx);
   assert(sy);
   assert(sz);

#define ROUND(a) int(a + 0.5)

   int nw = ROUND(sx * this->width);
   int nh = ROUND(sy * this->height);
   int nd = ROUND(sz * this->depth);

   double nw2 = nw / 2.0;
   double nh2 = nh / 2.0;
   double nd2 = nd / 2.0;
   double w2 = this->width / 2.0;
   double h2 = this->height / 2.0;
   double d2 = this->depth / 2.0;

   CVolume tmpVolume(nw, nh, nd, this->c);

   for (int x = 0; x < nw; x++)
      for (int y = 0; y < nh; y++)
         for (int z = 0; z < nd; z++)
         {
            double xtmp, ytmp, ztmp;
            xtmp = (x - nw2) / sx + w2;
            ytmp = (y - nh2) / sy + h2;
            ztmp = (z - nd2) / sz + d2;

            int voxel;

            switch (im)
            {
            case VISc::iNearestNeighbor: 
               xtmp = floor(xtmp+0.5);
               ytmp = floor(ytmp+0.5);
               ztmp = floor(ztmp+0.5);
               voxel = this->getVoxel(int(xtmp), int(ytmp), int(ztmp));
               break;
            case VISc::iTrilinear: 
               voxel = int(getVoxelInterpolated(xtmp, ytmp, ztmp));
               break;

            }
            tmpVolume.setVoxel(x, y, z, voxel);
         }

         return tmpVolume;
}

double CVolume::getVoxelInterpolated(double x, double y, double z)
{
   double xd, yd, zd;
   double xc, yc, zc;
   double xf, yf, zf;

   xd = x - floor( x );
   yd = y - floor( y );
   zd = z - floor( z );

   xf = floor( x );
   yf = floor( y );
   zf = floor( z );

   xc = ceil( x );
   yc = ceil( y );
   zc = ceil( z );

   double i1 = getVoxel(int(xf), int(yf), int(zf)) * (1 - zd) + getVoxel(int(xf), int(yf), int(zc)) * zd;
   double i2 = getVoxel(int(xf), int(yc), int(zf)) * (1 - zd) + getVoxel(int(xf), int(yc), int(zc)) * zd;
   double j1 = getVoxel(int(xc), int(yf), int(zf)) * (1 - zd) + getVoxel(int(xc), int(yf), int(zc)) * zd;
   double j2 = getVoxel(int(xc), int(yc), int(zf)) * (1 - zd) + getVoxel(int(xc), int(yc), int(zc)) * zd;

   double w1 = i1 * (1 - yd) + i2 * yd;
   double w2 = j1 * (1 - yd) + j2 * yd;

   return floor( (w1 * (1 - xd) + w2 * xd) + 0.5 );
}




// *************************************************************************************
CVolume_::CVolume_() :
m_width(0),
m_height(0),
m_depth(0),
m_numComponents(1),
m_qcColor(0,0,0),
m_position(0.0, 0.0, 0.0, 0.0)
{
   memset(m_aspectRatio, 0, sizeof(double) * 3);
}

CVolume_::CVolume_(int width, int height, int depth, int numComponents) :
m_width(width),
m_height(height),
m_depth(depth),
m_numComponents(numComponents),
m_qcColor(0,0,0),
m_position(0.0, 0.0, 0.0, 0.0)
{
   memset(m_aspectRatio, 0, sizeof(double) * 3);

   // Initialize the voxels
   m_data.reset( new float[totalVoxels()] );
   memset(m_data.data(), 0, sizeInBytes());
}

CVolume_::CVolume_ (const CVolume_ &volume)
{
   *this = volume;
}

CVolume_ & CVolume_::operator= (const CVolume_ &volume)
{
   if (&volume != this)
   {
      m_width = volume.m_width;
      m_height = volume.m_height;
      m_depth = volume.m_height;
      m_numComponents = volume.m_numComponents;
      m_qcColor = volume.m_qcColor;
      m_position = volume.m_position;
      m_bEnabled = volume.m_bEnabled;
      m_qsName = volume.m_qsName + " Copy";
      m_qdtTimeStamp = QDateTime::currentDateTime();

      // Copy the aspect ratio array
      memcpy(m_aspectRatio, volume.m_aspectRatio, sizeof(double) * 3);

      // Copy the voxels
      m_data.reset( new float[totalVoxels()] );
      memcpy(m_data.data(), volume.m_data.data(), sizeInBytes());
   }

   return *this;
}

CVolume_::~CVolume_()
{
   m_data.reset( NULL );
}

void CVolume_::render()
{
   glPushMatrix();
   glTranslatef(-m_width / 2.0, -m_height / 2.0, -m_depth / 2.0);
   glScalef(m_aspectRatio[VISc::dX], m_aspectRatio[VISc::dY],m_aspectRatio[VISc::dZ]);
   glTranslatef(m_position.x(), m_position.y(), m_position.z());

   glColor3f(m_qcColor.redF(), m_qcColor.greenF(), m_qcColor.blueF());
   glBegin(GL_LINES);

   // Front icon
   glVertex3f(getWidth() * 0.7, getHeight() * 0.2, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.3, getHeight() * 0.8, 0);
   glVertex3f(getWidth() * 0.7, getHeight() * 0.6, 0);
   glVertex3f(getWidth() * 0.3, getHeight() * 0.6, 0);

   // Front face
   glVertex3f(0, 0, 0);
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(0, getHeight(), 0);
   glVertex3f(0, getHeight(), 0);
   glVertex3f(0, 0, 0);

   // Left face
   glVertex3f(0, 0, 0);
   glVertex3f(0, 0, getDepth());
   glVertex3f(0, 0, getDepth());
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(0, getHeight(), 0);

   // Bottom face
   glVertex3f(getWidth(), 0, 0);
   glVertex3f(getWidth(), 0, getDepth());
   glVertex3f(getWidth(), 0, getDepth());
   glVertex3f(0, 0, getDepth());

   // Right face
   glVertex3f(getWidth(), getHeight(), 0);
   glVertex3f(getWidth(), getHeight(), getDepth());
   glVertex3f(getWidth(), getHeight(), getDepth());
   glVertex3f(getWidth(), 0, getDepth());

   // Hinder face
   glVertex3f(0, getHeight(), getDepth());
   glVertex3f(getWidth(), getHeight(), getDepth());
   glEnd();
   glPopMatrix();
}

void CVolume_::properties()
{
   // CVolumeProperties vproperties(this);
   // vproperties.show();
}

void CVolume_::getBoundingBox(float &mw, float &pw, float &mh, float &ph, float &md, float &pd)
{
   mw = m_position.x() - (m_width / 2.0);
   pw = m_position.x() + (m_width / 2.0);
   mh = m_position.y() - (m_height / 2.0);
   ph = m_position.y() - (m_height / 2.0);
   md = m_position.z() - (m_depth / 2.0);
   pd = m_position.z() - (m_depth / 2.0);
}

float CVolume_::getVoxel(int pos) const
{
   if ((pos < 0) || (pos >= size()))
      return 0.0f;

   return m_data.data()[pos];
}

float CVolume_::getVoxel(int x, int y, int z) const
{
   if ((x < 0) || (x >= m_width))
      return 0.0f;
   if ((y < 0) || (y >= m_height))
      return 0.0f;
   if ((z < 0) || (z >= m_depth))
      return 0.0f;

   return getVoxel(coordinatesToPosition(x, y, z));
}

float CVolume_::getVoxel(int x, int y, int z, int component) const
{
   if ((x < 0) || (x >= m_width))
      return 0.0f;
   if ((y < 0) || (y >= m_height))
      return 0.0f;
   if ((z < 0) || (z >= m_depth))
      return 0.0f;

   return m_data.data()[coordinatesToPosition(x, y, z) + component];
}

float CVolume_::getVoxel(QVector3D &position) const
{
   return getVoxel(position.x(), position.y(), position.z());
}

float CVolume_::getVoxelInterpolated(double x, double y, double z) const
{
   double xd, yd, zd;
   double xc, yc, zc;
   double xf, yf, zf;

   xd = x - floor( x );
   yd = y - floor( y );
   zd = z - floor( z );

   xf = floor( x );
   yf = floor( y );
   zf = floor( z );

   xc = ceil( x );
   yc = ceil( y );
   zc = ceil( z );

   float i1 = getVoxel(int(xf), int(yf), int(zf)) * (1 - zd) + getVoxel(int(xf), int(yf), int(zc)) * zd;
   float i2 = getVoxel(int(xf), int(yc), int(zf)) * (1 - zd) + getVoxel(int(xf), int(yc), int(zc)) * zd;
   float j1 = getVoxel(int(xc), int(yf), int(zf)) * (1 - zd) + getVoxel(int(xc), int(yf), int(zc)) * zd;
   float j2 = getVoxel(int(xc), int(yc), int(zf)) * (1 - zd) + getVoxel(int(xc), int(yc), int(zc)) * zd;

   float w1 = i1 * (1 - yd) + i2 * yd;
   float w2 = j1 * (1 - yd) + j2 * yd;

   return floor( (w1 * (1 - xd) + w2 * xd) + 0.5 );
}

void CVolume_::setVoxel(int pos, float value)
{
   if ((pos < 0) || (pos >= size()))
      return;

   m_data.data()[pos] = value;
}

void CVolume_::setVoxel(int x, int y, int z, float value)
{
   if ((x < 0) || (x >= m_width))
      return;
   if ((y < 0) || (y >= m_height))
      return;
   if ((z < 0) || (z >= m_depth))
      return;

   setVoxel(coordinatesToPosition(x, y, z), value);
}

void CVolume_::setVoxel(int x, int y, int z, int component, float value)
{
   if (x < 0 || x >= m_width)
      return;
   if (y < 0 || y >= m_height)
      return;
   if (z < 0 || z >= m_depth)
      return;
   
   int pos = coordinatesToPosition(x, y, z);
   pos += component;
   m_data.data()[pos] = value;
}

CVolume_ CVolume_::resize(float sx, float sy, float sz, VISc::EInterpolation interpolationMode)
{
   if ((sx <= 0.0) || (sy <= 0.0) || (sz <= 0.0))
      return CVolume_();

   int nw = VISc::MathUtils::Round(sx * m_width);
   int nh = VISc::MathUtils::Round(sy * m_height);
   int nd = VISc::MathUtils::Round(sz * m_depth);

   float nw2 = nw / 2.0f;
   float nh2 = nh / 2.0f;
   float nd2 = nd / 2.0f;
   float w2 = m_width / 2.0f;
   float h2 = m_height / 2.0f;
   float d2 = m_depth / 2.0f;

   CVolume_ tmpVolume(nw, nh, nd, m_numComponents);

   for (int x = 0; x < nw; ++x)
   {
      for (int y = 0; y < nh; ++y)
      {
         for (int z = 0; z < nd; ++z)
         {
            float xtmp = (x - nw2) / sx + w2;
            float ytmp = (y - nh2) / sy + h2;
            float ztmp = (z - nd2) / sz + d2;

            float voxel;
            switch (interpolationMode)
            {
            case VISc::iNearestNeighbor: 
               xtmp = floor(xtmp+0.5);
               ytmp = floor(ytmp+0.5);
               ztmp = floor(ztmp+0.5);
               voxel = getVoxel(static_cast<int>(xtmp), static_cast<int>(ytmp), static_cast<int>(ztmp));
               break;
            case VISc::iTrilinear: 
               voxel = getVoxelInterpolated(xtmp, ytmp, ztmp);
               break;
            }
            tmpVolume.setVoxel(x, y, z, voxel);
         }
      }
   }

   return tmpVolume;
}
