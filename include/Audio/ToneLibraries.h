#ifndef TONELIBRARIES_H
#define TONELIBRARIES_H

//https://sites.google.com/site/musicgapi/technical-documents/wav-file-format

#include <QString>
#include "FileManagement/taFileSystemObject.h"

class exWindowBase;

class VACFile : public taFile {
  public:
//    const QStringList validOutputs(){
//        return QStringList{"TMD", "BMD"};
//    }; //no valid outputs yet
    virtual const QString fileCategory(){
        return "Sound";
    };
    long fileSize;
    int compressionCode;
    int channels;
    int bitsPerSample;
    QString name;
    long offset;
    int rootNote;
    bool loop;

    std::vector<int> noteList;
    std::vector<std::vector<int>> freqList;

    int tempRead();
    void tempWrite();

    void load(QString fromType);
    void save(QString toType);
};

class ToneLibrary{
  public:
    int version;
    int tones;
    QString dataFile;
    std::vector<VACFile> toneList;


};

#endif // TONELIBRARIES_H
