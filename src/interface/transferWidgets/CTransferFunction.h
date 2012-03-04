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
