#include "CVolume.h"

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
CVolume::CVolume (CVolume &volume)
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
