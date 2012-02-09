#include "CXMLKeypointReader.h"

CXMLKeypointReader::CXMLKeypointReader(void)
{
}

CXMLKeypointReader::~CXMLKeypointReader(void)
{
}
void CXMLKeypointReader::parseKeypoint()
{
	CKeypoint kp;
	xml.readNext();

	if (xml.name() != "X") // Start X
		exit(0);
	kp.setX(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Y
	if (xml.name() != "Y")
		exit(0);
	kp.setY(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Z
	if (xml.name() != "Z")
		exit(0);
	kp.setZ(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Scale
	if (xml.name() != "Scale")
		exit(0);
	kp.setScale(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Orientation
	if (xml.name() != "Orientation1")
		exit(0);
	kp.setOrientation1(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Orientation 2
	if (xml.name() != "Orientation2")
		exit(0);
	kp.setOrientation2(xml.readElementText().toDouble());
	xml.readNext();

	xml.readNext(); // Start Dim
	if (xml.name() != "Dim")
		exit(0);
	int dimension = xml.readElementText().toInt();
	xml.readNext(); // End Dim

	// ****************************************************************************************************************************
	xml.readNext();
	
	if (xml.name() != "Descriptor")
		exit(0);
	readBlank();
	
	for (int i = 0; i < dimension; i++)
	{
		xml.readNext(); // Start int
		
		if (xml.name() != "int")
			exit(0);
		
		xml.readElementText();
		xml.readNext(); // End int
	}

	xml.readNext(); // End Descriptor
	readBlank();
	// ****************************************************************************************************************************

	kps.push_back(kp);
}
void CXMLKeypointReader::parseKeypoints()
{
	xml.readNext();
	while (xml.name() != "Arr")
	{
		// Start element
		if (xml.name() != "KeypointN")
			exit(0);
		readBlank();

		parseKeypoint();

		// End element
		xml.readNext();
		if (xml.name() != "KeypointN")
			exit(0);

		xml.readNext();
		readBlank();
	}
}

void CXMLKeypointReader::loadKeypoints(QString fileName)
{
	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly))
		return;

	xml.setDevice(&file);

	while (!xml.atEnd())
	{
		xml.readNext();

		QXmlStreamReader::TokenType tt = xml.tokenType();
		switch (tt)
		{
			case QXmlStreamReader::StartElement: 
				if (xml.name() == "XDim")
					xml.readElementText();
				else if (xml.name() == "YDim")
					xml.readElementText();
				else if (xml.name() == "ZDim")
					xml.readElementText();
				else if (xml.name() == "ImageFile")
					imageName = xml.readElementText();
				else if (xml.name() == "Arr")
				{
					readBlank();
					parseKeypoints();
				}
				break;
		}

	}
}