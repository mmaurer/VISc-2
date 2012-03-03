#ifndef CSCREENSHOT_H
#define CSCREENSHOT_H

#include <QDialog>
#include <QPushButton>
#include <QLabel>
#include <QSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QDialogButtonBox>

#if defined(WIN32) && defined(_DEBUG)
#define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
#define new DEBUG_NEW
#endif

class CScreenShot : public QDialog
{
    Q_OBJECT

public:
    CScreenShot(QSize size);
    virtual ~CScreenShot() { }

    int getWidth() const {return m_width; }
    int getHeight() const { return m_height; }
    QString getFileName() const { return m_qsFileName; }

private slots:
    void accepted();
    void clicked();
    void rejected();

private:
    int m_width, m_height;
    QString m_qsFileName;

    QSharedPointer<QLabel> m_qlWidthLabel, m_qlHeightLabel;
    QSharedPointer<QSpinBox> m_qsbWidthSpin, m_qsbHeightSpin;

    QSharedPointer<QLabel> imageFormatlabel;
    QSharedPointer<QComboBox> m_qcbImageFormatCombo;

    QSharedPointer<QLabel> m_qlFileNameLabel;
    QSharedPointer<QLineEdit> m_qleFileNameEdit;
    QSharedPointer<QPushButton> m_qpbFileNameButton;

    QSharedPointer<QDialogButtonBox> m_qdbbButtons;
};

#endif
