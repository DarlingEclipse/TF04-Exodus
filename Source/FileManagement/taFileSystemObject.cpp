
#include "FileManagement/taFileSystemObject.h"
#include "Utility/exDebugger.h"

/*only including this while the visitor system exists*/
#include "FileManagement/Zebrafish.h"

void taFile::save(QString toType){
    qDebug() << Q_FUNC_INFO << "The class you tried to save doesn't have a valid save function.";
}

void taFile::save(QString toType, QTextStream &stream){
    qDebug() << Q_FUNC_INFO << "The class you tried to save doesn't have a valid save function (textstream version).";
}

void taFile::load(QString fromType){
    qDebug() << Q_FUNC_INFO << "The class you tried to load doesn't have a valid load function yet.";
}

void taFile::updateCenter(){
    qDebug() << Q_FUNC_INFO << "The class you selected doesn't have a valid central layout yet.";
}

taFile::taFile(){
    m_UI = nullptr;
    duplicateFileCount = 0;
    fileName = "";
    fileExtension = "";
    this->fileData = nullptr;
    m_Debug = &exDebugger::GetInstance();
}

void taFile::acceptVisitor(zlManager& visitor){
    visitor.Visit(*this);
}
