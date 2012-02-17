#ifndef CTRANSFERFUNCTION_H
#define CTRANSFERFUNCTION_H

#include "../../visc.h"
#include "../../scene/CVolume.h"

#include <QWidget>
#include <QPainter>
#include <QtGui>

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CTransferFunction : public QWidget
{
	Q_OBJECT

	public:
		static const int DEPTH = 256;
		static const int FIRST_VALUE_TO_PLOT = 2;

		CTransferFunction(void);
		~CTransferFunction(void);

		void setComponent(VISc::EComponent c);
		VISc::EComponent getComponent() { return this->currentComponent; }
		void setContextMenu(QMenu *menu) { this->contextMenu = menu; }
		int getWidth() { return DEPTH + 4; }
		int getHeight() { return DEPTH + 4; } // It's odd that the body of this method is just like the body of getWidth
		void setGridX(int value) { this->gridx = value; this->update(); }
		int getGridX() { return this->gridx; }
		void setGridY(int value) { this->gridy = value; this->update(); }
		int getGridY() { return this->gridy; }
		QImage getTransferFunction();
		void computeHistogram(CVolume &volume, bool resetTransferFunction);
		bool isChanged() { return this->changed; }
		void setChanged() { this->changed = true; }
		void setUnchanged() { this->changed = false; } 

		// Inherited methods
		int load(QString fileName);
		int save(QString fileName);
		void zero();
		void reset();
		void clear();

	private:
		int histogramr[DEPTH], histogramg[DEPTH], histogramb[DEPTH], histogramo[DEPTH];
		int transferr[DEPTH], transferg[DEPTH], transferb[DEPTH], transfero[DEPTH];
		int *selectedHistogram, *selectedTransferFunction;
		bool mouseDown;
		VISc::EComponent currentComponent;
		qreal scalex, scaley;
		int gridx, gridy;
		int maxHist;
		bool changed;

		// Colors
		QColor gridColor;

		// Keep TF from drawing 'holes'
		int lastx;

		void drawGrid(QPainter &painter);
		void drawHistogram(QPainter &painter);
		void drawTransferFunction(QPainter &painter);


	protected:
		void paintEvent ( QPaintEvent * event );
		void mousePressEvent ( QMouseEvent * event );
		void mouseMoveEvent ( QMouseEvent * event );
		void mouseReleaseEvent ( QMouseEvent * event );
		void contextMenuEvent(QContextMenuEvent *event);

		QMenu *contextMenu;
};
#endif // CTRANSFERFUNCTION_H
