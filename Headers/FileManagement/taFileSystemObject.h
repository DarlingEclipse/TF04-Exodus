#ifndef TAFILESYSTEMOBJECT_H
#define TAFILESYSTEMOBJECT_H

#include <QString>
#include <QCheckBox>
#include <QTextStream>
#include <QDebug>

class exWindowBase;
class zlManager;
class exDebugger;
class FileData;

class taFilesystemObject{
public:
    QString m_relativePath;
    QString m_name;
    taFilesystemObject* m_parent;
};

enum {taFolderInvalid = -1, taFolderUncompressed, taFolderCompressed};
class taFolder : public taFilesystemObject{
public:
    std::vector<taFilesystemObject> m_children;
    int m_folderType = taFolderInvalid;
    bool m_modded; //0 for unmodded
    QCheckBox* m_checkOption = nullptr;
};

enum{taFileInvalid = -1, taFileNormal, taFileReplaced, taFileExcluded};
class taFile : public taFilesystemObject{
  public:
    int fileStatus = taFileInvalid;
    QString inputPath;
    QString outputPath;
    exWindowBase *m_UI;
    zlManager *m_zlManager;
    exDebugger *m_Debug;
    FileData *fileData;
    QString fileName;
    QString fileExtension;
    int duplicateFileCount = 0; //defaults to 0, incremented by 1 for each file with a matching name
    bool binary; //0 for text file, 1 for binary

    taFile();

    virtual const QStringList validOutputs(){
        qDebug() << Q_FUNC_INFO << "The chosen file does not have a valid output list.";
        return QStringList("");
    };

    virtual const QString fileCategory(){
        qDebug() << Q_FUNC_INFO << "The chosen file does not have a valid category;";
        return "";
    };

    virtual const QString fullFileName(){
        if(duplicateFileCount > 0){
            return fileName + "." + fileExtension + "(" + QString::number(duplicateFileCount) + ")";
        } else {
            return fileName + "." + fileExtension;
        }
    }

    virtual void save(QString toType);
    virtual void save(QString toType, QTextStream &stream);
    virtual void load(QString fromType);
    virtual void acceptVisitor(zlManager& visitor);
    virtual void updateCenter();
};

#endif // TAFILESYSTEMOBJECT_H
