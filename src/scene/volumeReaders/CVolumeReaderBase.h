#ifndef CVOLUMEREADERBASE_H
#define CVOLUMEREADERBASE_H

class CVolumeReaderBase
{
public:
    CVolumeReaderBase(CVolume_ &volume) :
        m_volume(volume)
    {
    }

    int load(QString fileName) = 0;
    int save(QString fileName) = 0;
    QString getLastError(int errno) = 0;

private:
    CVolumeReaderBase() { }
    CVolumeReaderBase(const CVolumeReaderBase &) { }
    CVolumeReaderBase &operator=(const CVolumeReaderBase &volume) { }

    CVolume_ &m_volume;
};

#endif
