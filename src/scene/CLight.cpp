#include "CLight.h"

#include <QInputDialog>

void CLight::properties()
{
    QString currentName;
    if (!m_qsName.isEmpty())
        currentName = m_qsName;

    bool ok(false);
    QString text = QInputDialog::getText(0, QObject::tr("Light name"), QObject::tr("Lightname:"), QLineEdit::Normal, currentName, &ok);

    if (ok && !text.isEmpty())
        m_qsName = text;
}
