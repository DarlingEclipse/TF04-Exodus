#include "Audio/ToneLibraries.h"
#include "Utility/exDebugger.h"
#include "Utility/BinChanger.h"

//http://soundfile.sapp.org/doc/WaveFormat/


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
        m_Debug->MessageError("There was an error reading " + fileName);
        return;
    }
}

int VACFile::tempRead(){
    fileData->readFile("D:\\TF2_RevEngineer\\Model work\\FLAKBURST.VAC");
    std::vector<int> freqSet;
    while (fileData->currentPosition < fileData->dataBytes.size()){
        noteList.push_back(fileData->readInt(2));
        for(int i = 0; i < 14; i++){
            freqSet.push_back(fileData->readInt(1));
        }
        freqList.push_back(freqSet);
        freqSet.clear();
    }
    return 0;
}

void VACFile::tempWrite(){
    if(this->noteList.empty()){
       m_Debug->MessageError("No audio files available to export. Please load an audio file.");
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
            BinChanger::shortWrite(vacOut, noteList[i]);
            for(int j = 0; j < freqList[i].size(); j++){
                BinChanger::byteWrite(vacOut, freqList[i][j]);
            }
        }
    }
}
