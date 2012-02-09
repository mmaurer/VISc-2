#ifndef CXMLKEYPOINTREADER_H
#define CXMLKEYPOINTREADER_H

#include <vector>

#include <QXmlStreamReader>
#include <QFile>

#include "../../algs/sift3d/CKeypoint.h"
using namespace sift3dlib;

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CXMLKeypointReader
{
	public:
		CXMLKeypointReader(void);
		~CXMLKeypointReader(void);

		QString getImageName() { return this->imageName; };
		std::vector<CKeypoint> getKeypoints() const { return this->kps; }
		void loadKeypoints(QString fileName);
	
	private:
		QXmlStreamReader xml;
		QString imageName;
		std::vector<CKeypoint> kps;

		void readBlank() { xml.readNext(); };
		void parseKeypoints();
		void parseKeypoint();
};

#endif