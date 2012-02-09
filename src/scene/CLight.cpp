#include "CLight.h"

CLight::CLight(VISc::ELightType lightType) :
m_lightType(lightType),
m_timeStamp( QDateTime::currentDateTime() )
{
}

void CLight::properties()
{
	QString currentName;
	if (!m_name.isEmpty())
		currentName = m_name;

	bool ok;
	QString text;

   text = QInputDialog::getText(0, QObject::tr("Light name"), QObject::tr("Light name:"), QLineEdit::Normal, currentName, &ok); 

	if (ok && !text.isEmpty())
         m_name = text;
};
