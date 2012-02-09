#include "CTransferFunction.h"

CTransferFunction::CTransferFunction(void)
{
	for (int i=0; i < DEPTH; i++)
	{
		histogramb[i] = histogramg[i] = histogramo[i] = histogramr[i] = 0;
		transferb[i] = transferg[i] = transfero[i] = transferr[i] = i;
	}
	selectedHistogram = histogramo;
	selectedTransferFunction = transfero;
	currentComponent = VISc::cOpacity;

	changed = true;
	mouseDown = false;
	lastx = -1;

	// Colors
	gridColor.setRgb(200, 200, 200, 255);

	// Scale of drawing
	scalex = 0.98;
	scaley = 0.98;

	gridx = gridy = 32;
}

CTransferFunction::~CTransferFunction(void)
{

}



// Setter/Getter
void CTransferFunction::setComponent(VISc::EComponent c) 
{ 
	switch (c)
	{
		case VISc::cRed: selectedHistogram = histogramr; selectedTransferFunction = transferr; break;
		case VISc::cGreen: selectedHistogram = histogramg; selectedTransferFunction = transferg; break;
		case VISc::cBlue: selectedHistogram = histogramb; selectedTransferFunction = transferb; break;
		case VISc::cOpacity: selectedHistogram = histogramo; selectedTransferFunction = transfero; break;
	}
	this->currentComponent = c; 
	this->update(); 
}

// end of setter/getter

// Events
void CTransferFunction::paintEvent(QPaintEvent *)
{
	QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
	painter.translate(2, 2);
	painter.setBackground(QBrush(QColor(0, 0, 0, 0)));

	drawGrid(painter);
	drawHistogram(painter);
	drawTransferFunction(painter);

	painter.translate(100, 100);
    painter.end();
}

void CTransferFunction::mousePressEvent ( QMouseEvent * )
{
	mouseDown = true;
}

void CTransferFunction::mouseMoveEvent ( QMouseEvent * event )
{
	if (mouseDown)
	{
		if ((event->x() > 255) || (event->x() < 0) || (event->y() < 0) || (event->y() > 255))
			return;
		selectedTransferFunction[event->x()] = DEPTH - 1 - event->y();
		
		if(lastx >= 0) 
		{
		  int dxAbs = abs(lastx - event->x());
		  if(dxAbs > 1) 
		  {
			 int xincr = dxAbs / (event->x() - lastx);
			 int lasty = selectedTransferFunction[lastx];
			 int ix = lastx;
			 int i=0;
			 while(ix != event->x())
			 {
	  			float u = (float) i / dxAbs;
				selectedTransferFunction[ix] = (int) (u * (DEPTH - 1 - event->y()) + (1 - u) * lasty);
				ix += xincr;
				i += 1;
			 }
		  }
		}
		lastx = event->x();
		changed = true;

		this->update();
	}
}

void CTransferFunction::mouseReleaseEvent ( QMouseEvent * )
{
	mouseDown = false;
	lastx = -1;
	this->update();
}
void CTransferFunction::contextMenuEvent(QContextMenuEvent *event)
{
   if (contextMenu)
	   contextMenu->exec(event->globalPos());
}

// end of event

// Inherited methods
int CTransferFunction::load(QString fileName)
{
	QSharedPointer<QFile> tmpFile(new QFile(fileName));
	tmpFile->open(QIODevice::ReadOnly);

	for (int i=0; i<256; i++)
	{
		int r, g, b, o;
		char data[100];

		tmpFile->readLine(data, 99);
		sscanf_s(data, "%i %i %i %i\n", &r, &g, &b, &o);

		this->transferr[i] = r;
		this->transferg[i] = g;
		this->transferb[i] = b;
		this->transfero[i] = o;
	}

	tmpFile->close();
	this->update();

	return 0;
}

int CTransferFunction::save(QString fileName)
{
	QFile *tmpFile = new QFile(fileName);
	tmpFile->open(QIODevice::WriteOnly);

	for (int i=0; i<256; i++)
	{
		char data[100];
		int size;

		size = sprintf_s(data, "%i\t%i\t%i\t%i\n", transferr[i], transferg[i], transferb[i], transfero[i]);
		tmpFile->write(data, size);
	}
	tmpFile->close();

	return 0;
}

void CTransferFunction::zero()
{
	for (int i=0; i < DEPTH; i++)
		selectedTransferFunction[i] = 0;
	this->update();
}

void CTransferFunction::reset()
{
	for (int i=0; i < DEPTH; i++)
		selectedTransferFunction[i] = i;
	this->update();
}
void CTransferFunction::clear()
{
	for (int i = 0; i < DEPTH; i++)
	{
		histogramb[i] = histogramg[i] = histogramo[i] = histogramr[i] = 0;
		transferb[i]  = transferg[i]  = transfero[i]  = transferr[i]  = i;
	}
}
//end of inherited members

void CTransferFunction::drawGrid(QPainter &painter)
{
	QPen newPen;
	newPen.setWidth(1);
	newPen.setColor(gridColor);

	// Bounding box
	painter.setPen(newPen);
	painter.drawLine(0, 0, 255, 0);
	painter.drawLine(0, 0, 0, 255);
	painter.drawLine(0, 255, 255, 255);
	painter.drawLine(255, 0, 255, 255);

	for (int x = 0; x < DEPTH; x += gridx)
		painter.drawLine(x, 0, x, DEPTH - 1);

	for (int y = 0; y < DEPTH; y += gridy)
		painter.drawLine(0, y, DEPTH - 1, y);
}

void CTransferFunction::drawHistogram(QPainter &painter)
{
	QPen newPen;
	newPen.setWidth(1);
	newPen.setColor(QColor(80, 80, 80));
	painter.setPen(newPen);

	int previousx = FIRST_VALUE_TO_PLOT; 
	int previousy = DEPTH - 1;
	for (int i = FIRST_VALUE_TO_PLOT + 1; i <= DEPTH; i++)
	{
		float ratio = selectedHistogram[i] / float(maxHist);
		painter.drawLine(previousx, previousy, i, DEPTH - 1 - (ratio * DEPTH));
		previousx = i;
		previousy = DEPTH - 1 - (ratio * DEPTH);
	}
}

void CTransferFunction::drawTransferFunction(QPainter &painter)
{
	QPen newPen, newPenr, newPeng, newPenb;
	newPen.setWidth(2);
	newPenr.setWidth(2);
	newPeng.setWidth(2);
	newPenb.setWidth(2);

	int opacityBack = 20;
	int opacityFront = 255;
	switch (currentComponent)
	{
		case VISc::cRed: 
						newPen.setColor(QColor(80, 80, 80, opacityBack));
						newPenr.setColor(QColor(255, 0, 0, opacityFront));
						newPeng.setColor(QColor(0, 255, 0, opacityBack));
						newPenb.setColor(QColor(0, 0, 255, opacityBack));
						break;
		case VISc::cGreen: 
						newPen.setColor(QColor(80, 80, 80, opacityBack));
						newPenr.setColor(QColor(255, 0, 0, opacityBack));
						newPeng.setColor(QColor(0, 255, 0, opacityFront));
						newPenb.setColor(QColor(0, 0, 255, opacityBack));
						break;
		case VISc::cBlue: 
						newPen.setColor(QColor(80, 80, 80, opacityBack));
						newPenr.setColor(QColor(255, 0, 0, opacityBack));
						newPeng.setColor(QColor(0, 255, 0, opacityBack));
						newPenb.setColor(QColor(0, 0, 255, opacityFront));
						break;
		case VISc::cOpacity: 
						newPen.setColor(QColor(80, 80, 80, opacityFront));
						newPenr.setColor(QColor(255, 0, 0, opacityBack));
						newPeng.setColor(QColor(0, 255, 0, opacityBack));
						newPenb.setColor(QColor(0, 0, 255, opacityBack));
						break;
		default: 
						newPen.setColor(QColor(80, 80, 80, opacityBack));
						newPenr.setColor(QColor(255, 0, 0, opacityBack));
						newPeng.setColor(QColor(0, 255, 0, opacityBack));
						newPenb.setColor(QColor(0, 0, 255, opacityBack));
						break;
	}
	painter.setPen(newPen);

	int previousx = 0, previousy = DEPTH - 1, previousr = DEPTH - 1, previousg = DEPTH - 1, previousb = DEPTH - 1;
	for (int i = 1; i <= DEPTH; i++)
	{
		painter.setPen(newPenr);
		painter.drawLine(previousx, previousr, i, DEPTH - 1 - transferr[i]);
		painter.setPen(newPeng);
		painter.drawLine(previousx, previousg, i, DEPTH - 1 - transferg[i]);
		painter.setPen(newPenb);
		painter.drawLine(previousx, previousb, i, DEPTH - 1 - transferb[i]);
		painter.setPen(newPen);
		painter.drawLine(previousx, previousy, i, DEPTH - 1 - transfero[i]);
		previousx = i;
		
		previousy = DEPTH - 1 - transfero[i];
		previousr = DEPTH - 1 - transferr[i];
		previousg = DEPTH - 1 - transferg[i];
		previousb = DEPTH - 1 - transferb[i];
	}
}
QImage CTransferFunction::getTransferFunction()
{
	QImage newTransferFunction(DEPTH, 1, QImage::Format_ARGB32);

	for (int i=0; i < DEPTH; i++)
		newTransferFunction.setPixel(i, 0, qRgba(transferr[i], transferg[i], transferb[i], transfero[i]));

	return newTransferFunction;
}
void CTransferFunction::computeHistogram(CVolume &volume, bool resetTransferFunction)
{
	// Initialize canvas
	for (int i = 0; i < DEPTH; i++)
	{
		if (resetTransferFunction)
			transfero[i] = transferr[i] = transferg[i] = transferb[i] = i;
		histogramo[i] = histogramr[i] = histogramg[i] = histogramb[i] = 0;
	}

	maxHist = -1;
	int index1=0;
	for (int w = 0; w < volume.totalVoxels(); w++)
	{			
		unsigned char voxel = volume.getVoxel(w);
		int index = (int) voxel;

		histogramo[index]++;
		histogramr[index]++;
		histogramg[index]++;
		histogramb[index]++;

		if (index >= FIRST_VALUE_TO_PLOT)
			if (histogramo[index] > maxHist)
			{
				maxHist = histogramo[index];
				index1 = index;
			}
	}
}
