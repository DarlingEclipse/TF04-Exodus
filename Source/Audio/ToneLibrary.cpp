#include "Headers/Main/mainwindow.h"

void VACFile::save(QString toType){
    if(toType == "VAC"){
        tempWrite();
    } /*else if (toType == "DAE"){
        outputDataDAE();
    }*/
}

void VACFile::load(QString fromType){
    int failedRead = 0;
    if(fromType == "VAC"){
        failedRead = tempRead();
    } else {
        failedRead = 1;
    }
    if(failedRead){
        parent->messageError("There was an error reading " + fileName);
        return;
    }
}

int VACFile::tempRead(){
    parent->fileData.readFile("D:\\TF2_RevEngineer\\Model work\\FLAKBURST.VAC");
    std::vector<int> freqSet;
    while (parent->fileData.currentPosition < parent->fileData.dataBytes.size()){
        noteList.push_back(parent->fileData.readInt(2));
        for(int i = 0; i < 14; i++){
            freqSet.push_back(parent->fileData.readInt(1));
        }
        freqList.push_back(freqSet);
        freqSet.clear();
    }
    return 0;
}

void VACFile::tempWrite(){
    if(this->noteList.empty()){
       parent->messageError("No audio files available to export. Please load an audio file.");
       return;
    }
    //QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output VAC"), QDir::currentPath() + "/VAC/", parent->tr("Tone Files (*.vac)"));
    QFile vacOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if (vacOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&vacOut);

        for(int i = 0; i < noteList.size(); i++){
            parent->binChanger.shortWrite(vacOut, noteList[i]);
            for(int j = 0; j < freqList[i].size(); j++){
                parent->binChanger.byteWrite(vacOut, freqList[i][j]);
            }
        }
    }
}
