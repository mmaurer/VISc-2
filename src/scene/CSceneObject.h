#ifndef CSCENEOBJECT_H
#define CSCENEOBJECT_H

#include <QDateTime>

#if defined(WIN32) && defined(_DEBUG)
   #define DEBUG_NEW new( _NORMAL_BLOCK, __FILE__, __LINE__ )
   #define new DEBUG_NEW
#endif

class CSceneObject
{
public:
   CSceneObject() :
   m_bEnabled(false),
   m_qsName("Unnamed"),
   m_qdtTimeStamp(QDateTime::currentDateTime())
   { }
   virtual ~CSceneObject() { }
   
   virtual void render() { }
   virtual void properties() { }
   virtual void getBoundingBox(float &mw, float &pw, float &mh, float &ph, float &md, float &pd) { mw = pw = mh = ph = md = pd = 0.0f; }

   bool getIsEnabled() const { return m_bEnabled; }
   void setIsEnabled(bool value) { m_bEnabled = value; }
   QString getName() const { return m_qsName; }
   void setName(QString name) { m_qsName = name; }
   QDateTime getTimeStamp() const { return m_qdtTimeStamp; }

protected:
   bool m_bEnabled;
   QString m_qsName;
   QDateTime m_qdtTimeStamp;
};

#endif
