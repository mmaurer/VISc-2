#ifndef CSCREENSHOT_H
#define CSCREENSHOT_H

#include <QDialog>
#include <QtGui>

class QLabel;
class QSpinBox;
class QComboBox;
class QLineEdit;
class QDialogButtonBox;

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CScreenShot : public QDialog
{
	Q_OBJECT

	public:
		CScreenShot(QSize);
		~CScreenShot(void);

		int getWidth() {return this->width; };
		int getHeight() { return this->height; };
		QString getFileName() { return this->fileName; };

	private slots:
		void accepted();
		void clicked();
		void rejected();

	private:
		int width, height;
		QString fileName;

		QLabel *widthLabel, *heightLabel;
		QSpinBox *widthSpin, *heightSpin;

		QLabel *imageFormatlabel;
		QComboBox *imageFormatCombo;

		QLabel *fileNameLabel;
		QLineEdit *fileNameEdit;
		QPushButton *fileNameButton;

		QDialogButtonBox *buttons;
};

#endif
