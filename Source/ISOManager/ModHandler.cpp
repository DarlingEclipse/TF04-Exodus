#include <QPushButton>
#include <QCoreApplication>

#include "Headers/FileManagement/Zebrafish.h"
#include "Headers/ISOManager/ModHandler.h"
#include "Headers/ISOManager/IsoBuilder.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/UI/exSettings.h"

ModHandler::ModHandler(exWindowBase *passUI, zlManager *fileManager){

    m_UI = passUI;
    m_zlManager = fileManager;
    m_Debug = &exDebugger::GetInstance();

    loadMods();
    loadFileReplacements();

    QStringList folderList = {"SOUND","FMV","IOP", "SOUNDE", "TFA", "TFA2", "TA_XTRAS"};
    QStringList uncompressed = {"SOUND", "FMV", "IOP"};

    for(int i = 0; i < folderList.size(); i++){


        taFolder nextFolder;
        nextFolder.m_name = folderList[i];
        nextFolder.m_modded = false;
        if(uncompressed.contains(folderList[i])){
            nextFolder.m_folderType = taFolderUncompressed;
        } else {
            nextFolder.m_folderType = taFolderCompressed;
        }
        nextFolder.m_checkOption = nullptr;
        folderOptions.push_back(nextFolder);

    }
}

void ModHandler::updateCenter(){
    m_zlManager->m_IsoBuilder->setCopyPath("ModdedISO");

    m_UI->m_centralContainer->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QLabel{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QToolTip{color: rgb(0,0,0);}");

    QGroupBox *groupModOptions = new QGroupBox("Mod Options", m_UI->m_centralContainer);
    groupModOptions->setGeometry(QRect(QPoint(250,100), QSize(200,300)));
    m_UI->m_currentWidgets.push_back(groupModOptions);

    for(int i = 0; i < modList.size(); i++){
        //this will need to be edited later for when we have more mods than will fit in the box to move to the next column. or scroll?
        QCheckBox *modCheck = new QCheckBox(modList[i].name, groupModOptions);
        modCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        //modCheck->setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);}");
        QAbstractButton::connect(modCheck, &QCheckBox::checkStateChanged, m_UI, [i, modCheck, this] {modList[i].enabled = modCheck->isChecked();});
        modCheck->setToolTip(modList[i].description);
        modCheck->show();
        //parent->currentModeWidgets.push_back(modCheck);
    }

    groupModOptions->show();

    QGroupBox *groupReplacements = new QGroupBox("File Replacements", m_UI->m_centralContainer);
    groupReplacements->setGeometry(QRect(QPoint(600,100), QSize(200,300)));
    /*groupReplacements->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QToolTip{color: rgb(0,0,0);}");*/
    m_UI->m_currentWidgets.push_back(groupReplacements);

    for(int i = 0; i < replacementList.size(); i++){
        //this will need to be edited later for when we have more mods than will fit in the box to move to the next column. or scroll?
        QCheckBox *replacementCheck = new QCheckBox(replacementList[i].name, groupReplacements);
        replacementCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        //replacementCheck->setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);}");
        QAbstractButton::connect(replacementCheck, &QCheckBox::checkStateChanged, m_UI, [i, replacementCheck, this]
            {replacementList[i].enabled = replacementCheck->isChecked();
            setModdedFolders();
        });
        replacementCheck->setToolTip(replacementList[i].description);
        replacementCheck->show();
        //parent->currentModeWidgets.push_back(replacementCheck);
    }

    groupReplacements->show();

    QGroupBox *groupFolders = new QGroupBox("Folder to Use", m_UI->m_centralContainer);
    groupFolders->setGeometry(QRect(QPoint(850,100), QSize(200,300)));
    /*groupReplacements->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QToolTip{color: rgb(0,0,0);}");*/
    m_UI->m_currentWidgets.push_back(groupFolders);
    for(int i = 0; i < folderOptions.size(); i++){
        QCheckBox *folderCheck = new QCheckBox(folderOptions[i].m_name, groupFolders);
        folderCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        folderOptions[i].m_checkOption = folderCheck;
        QAbstractButton::connect(folderCheck, &QCheckBox::checkStateChanged, m_UI, [i, folderCheck, this]
        {   if(checkNeededFolder(folderOptions[i].m_name)){
                folderOptions[i].m_modded = true;
                if(!folderCheck->isChecked()){
                    folderCheck->setChecked(true);
                    m_Debug->Log("Cannot disable folder " + folderOptions[i].m_name + ". An active file replacement needs it.");
                }
            } else {
                folderOptions[i].m_modded = folderCheck->isChecked();
            }
        });
        folderCheck->show();
    }

    groupFolders->show();

    QPushButton* buttonApply = new QPushButton("Apply Changes", m_UI->m_centralContainer);
    buttonApply->setGeometry(QRect(QPoint(50,320), QSize(150,30)));
    QAbstractButton::connect(buttonApply, &QPushButton::released, m_UI, [this] {setChanges();});
    m_UI->m_currentWidgets.push_back(buttonApply);
    buttonApply->show();
}

void ModHandler::setModdedFolders(){

    for(int i = 0; i < folderOptions.size(); i++){
        if(checkNeededFolder(folderOptions[i].m_name)){
            if(folderOptions[i].m_checkOption != nullptr){
                folderOptions[i].m_checkOption->setChecked(true);
            } else {
                folderOptions[i].m_modded = true;
            }
        }
    }
    /*for(int i = 0; i < replacementList.size(); i++){
        if(!replacementList[i].enabled){
            continue;
        }
        for(int j = 0; j < folderOptions.size(); j++){
            if(!replacementList[i].affectedFolders.contains(folderOptions[j].folderName)){
                continue;
            }
            folderOptions[j].checkOption->setChecked(true);
        }
    }*/
}

bool ModHandler::checkNeededFolder(QString folderName){
    for(int i = 0; i < replacementList.size(); i++){
        if(replacementList[i].enabled && replacementList[i].affectedFolders.contains(folderName)){
            return true;
        }
    }
    return false;
}

void ModHandler::setChanges(){
    /*To set up custom rebuilds, we need to do a few things:
    Make a new folder for the modified files
    Copy the replacement files and ELF (like what is done with the Randomzier)
    Save the new path as the "rebuild" path*/

    m_zlManager->m_IsoBuilder->setCopyPath("Modded");
    for(int i = 0; i < replacementList.size(); i++){
        if(replacementList[i].enabled){
            replaceFile(replacementList[i]);
        }
    }
    applyModifications();
    m_zlManager->m_IsoBuilder->packModded("Rebuild");

}

void ModHandler::loadFileReplacements(){
    QString modPath = QCoreApplication::applicationDirPath() + "/Replacements/";
    qDebug() << Q_FUNC_INFO << "Loading file replacements from" << modPath;
    QDir modFolder(modPath);
    QDirIterator modIterator(modFolder.absolutePath());
    qDebug() << Q_FUNC_INFO << "next file info:" << modIterator.nextFileInfo().fileName() << "from path" << modFolder.absolutePath();
    bool headerFinished = false;
    TextProperty modProperty;
    QStringList propertyOptions = {"File Version", "Name", "Description", "Rarity", "File Count", "File Name", "Destination Path"};
    int modVersion = 0;
    int replacementCount = 0;

    while (modIterator.hasNext()){
        QFile currentModFile = modIterator.next();
        qDebug() << Q_FUNC_INFO << "Current file" << currentModFile.fileName();
        if (currentModFile.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "Reading file";
            FileReplacement moddedFiles;
            moddedFiles.enabled = false;
            FileData modBuffer;
            modBuffer.dataBytes = currentModFile.readAll();
            modBuffer.input = true;
            headerFinished = false;
            QString targetLevel;
            QStringList brokenPath;
            while(!headerFinished){
                modProperty = modBuffer.readProperty();
                qDebug() << Q_FUNC_INFO << "test property type:" << modProperty.name << "with value:" << modProperty.readValue;
                switch(propertyOptions.indexOf(modProperty.name)){
                case 0: //File Version
                    modVersion = modProperty.readValue.toInt();
                    break;
                case 1: //Name
                    moddedFiles.name = modProperty.readValue;
                    break;
                case 2: //Description
                    //for human use only, for now. tooltips later.
                    moddedFiles.description = modProperty.readValue;
                    break;
                case 3: //Rarity
                    //Only used for Randomizer
                    //the value x from 1/x that generates the chance of this file swap
                    //ex. a rarity of 4 has a 1/4 or 25% chance of swapping
                    moddedFiles.rarity = modProperty.readValue.toInt();
                    qDebug() << Q_FUNC_INFO << "replacement" << moddedFiles.name << "has rarity" << moddedFiles.rarity;
                    break;
                case 4: //File count
                    replacementCount = modProperty.readValue.toInt();
                    headerFinished = true;
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "Unknown property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                }
            }
            for(int i = 0; i < replacementCount*2; i++){ //verify this line with the SELF version
                modProperty = modBuffer.readProperty();
                switch(propertyOptions.indexOf(modProperty.name)){
                case 5: //File Name
                    moddedFiles.fileNames.push_back(modProperty.readValue);
                    break;
                case 6: //Destination path
                    moddedFiles.fileDestinations.push_back(modProperty.readValue);
                    brokenPath = modProperty.readValue.split("/");
                    if(!moddedFiles.affectedFolders.contains(brokenPath[1])){
                        moddedFiles.affectedFolders.push_back(brokenPath[1]);
                    }
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "Unknown property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                }
            }
            replacementList.push_back(moddedFiles);
        }
    }

    for(int i = 0; i < replacementList.size(); i++){
        qDebug() << Q_FUNC_INFO << "File replacement" << i << "is" << replacementList[i].name;
    }
}

void ModHandler::replaceFile(QString fileName, QString destinationPath){
    QString modFileDirectory = QDir::currentPath();
    bool didItWork = false;
    QString fileInputDirectory = modFileDirectory + "/ASSETS/" + fileName;
    qDebug() << Q_FUNC_INFO << "current path:" << modFileDirectory;
    QFile fileReplacement(fileInputDirectory);
    qDebug() << Q_FUNC_INFO << "checking if replacement image exists:" << fileReplacement.exists() << "at directory:" << fileInputDirectory;

    QString fileOutputDirectory = m_zlManager->copyOutputPath + destinationPath;

    qDebug() << Q_FUNC_INFO << "creating directory" << fileOutputDirectory;
    QDir createFile(fileOutputDirectory);
    if(!createFile.exists()){
        createFile.mkpath(".");
    }
    fileOutputDirectory += "/" + fileName;
    QFile original(fileOutputDirectory);
    qDebug() << Q_FUNC_INFO << "checking if original file exists:" << original.exists() << "path" << fileOutputDirectory;
    if(original.exists()){
        original.remove();
    }
    if(fileReplacement.exists()){
        didItWork = QFile::copy(fileInputDirectory, fileOutputDirectory);
        qDebug() << Q_FUNC_INFO << "did it work?" << didItWork;
    } else {
        m_Debug->Log("File " + fileName + " could not be replaced.");
    }
}

void ModHandler::replaceFile(FileReplacement fileToReplace){
    qDebug() << Q_FUNC_INFO << "file has" << fileToReplace.fileNames.size() << "files";
    for(int i = 0; i < fileToReplace.fileNames.size(); i++){
        replaceFile(fileToReplace.fileNames[i], fileToReplace.fileDestinations[i]);
    }

    for(int i = 0; i < folderOptions.size(); i++){
        if(checkNeededFolder(folderOptions[i].m_name)){
            if(folderOptions[i].m_checkOption != nullptr){
                folderOptions[i].m_checkOption->setChecked(true);
            } else {
                folderOptions[i].m_modded = true;
            }
        }
    }
}

void ModHandler::loadMods(){
    QString modPath = QCoreApplication::applicationDirPath() + "/Mods/";
    qDebug() << Q_FUNC_INFO << "Loading mods from" << modPath;
    QDir modFolder(modPath);
    QDirIterator modIterator(modFolder.absolutePath());
    qDebug() << Q_FUNC_INFO << "next file info:" << modIterator.nextFileInfo().fileName() << "from path" << modFolder.absolutePath();
    bool headerFinished = false;
    TextProperty modProperty;
    QStringList propertyOptions = {"Version", "Mod Name", "Mod Description", "Mod type", "Mod sections", "Starting address", "Section lines"};
    int modVersion = 0;

    while (modIterator.hasNext()){
        QFile currentModFile = modIterator.next();
        RandomizerMod currentModData;
        qDebug() << Q_FUNC_INFO << "Current file" << QFileInfo(currentModFile).fileName();
        currentModData.fileName = QFileInfo(currentModFile).fileName();
        currentModData.enabled = false;
        if(QFileInfo(currentModFile).suffix() == "txt"){
            currentModData.type = 1;
        } else {
            currentModData.type = 0;
            qDebug() << Q_FUNC_INFO << "Skipping binary mod for now - not currently supported by SELF.";
            continue;
        }
        if (currentModFile.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "Reading file";
            FileData modBuffer;
            modBuffer.dataBytes = currentModFile.readAll();
            modBuffer.input = true;
            headerFinished = false;
            while(!headerFinished){
                modProperty = modBuffer.readProperty();
                qDebug() << Q_FUNC_INFO << "test property type:" << modProperty.name << "with value:" << modProperty.readValue;
                switch(propertyOptions.indexOf(modProperty.name)){
                case 0: //Version
                    modVersion = modProperty.readValue.toInt();
                    if(modVersion < 2){
                        headerFinished = true;
                    }
                    break;
                case 1: //Mod Name
                    currentModData.name = modProperty.readValue;
                    break;
                case 2: //Mod Description
                    currentModData.description = modProperty.readValue;
                    headerFinished = true;
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "Unknown property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                }
            }
            qDebug() << Q_FUNC_INFO << "mod name:" << currentModData.name << "mod description:" << currentModData.description;
            modList.push_back(currentModData);
        }
    }
}

void ModHandler::applyModifications(){
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString mipsEditorPath = QCoreApplication::applicationDirPath() + "/SELF.exe";
    qDebug() << Q_FUNC_INFO << mipsEditorPath;

    QStringList args;
    QString gamePath = m_zlManager->m_Settings->GetValue("Game extract path") + "/SLUS_206.68";
    QString modPath = QCoreApplication::applicationDirPath() + "/Mods/";
    if(gamePath == ""){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }
    m_Debug->Log("Modifying ELF from: " + gamePath);
    args.append(gamePath);
    args.append(modPath);

    QString modFiles = "";
    for(int i = 0; i < modList.size(); i++){
        if(modList[i].enabled){
            if(modFiles != ""){
                modFiles += "|";
            }
            modFiles += modPath + modList[i].fileName;
        }
    }
    qDebug() << Q_FUNC_INFO << "mod file list:" << modFiles;
    args.append(modFiles);

    QString randomizerPath = m_zlManager->copyOutputPath + "/SLUS_206.68";

    args.append(randomizerPath);

    if(modFiles != ""){
        m_Debug->Log("Putting modified ELF in " + randomizerPath);
        QProcess *mipsEditor = new QProcess();
        QObject::connect(mipsEditor, &QProcess::readyReadStandardOutput, [mipsEditor]() {
            QString debugOutput = mipsEditor->readAllStandardOutput();
            qDebug() << debugOutput;
            int outputCode = 0;
            if(debugOutput.left(4) == "Code"){
                debugOutput = debugOutput.right(debugOutput.length() - 5);
                qDebug() << "trimmed debug output:" << debugOutput;
                outputCode = debugOutput.left(4).toInt();
            } else {
                outputCode = 0;
            }
            switch(outputCode){
            case 0:
                //nothing to worry about
                break;
            default:
                //either nothing to worry about or something we haven't handled yet. output it.
                qDebug() << "Unhandled output code:" << debugOutput;
                break;
            }
        });
        m_Debug->Log("Modding ELF.");
        mipsEditor->start(mipsEditorPath, args);
    }

    qDebug() << Q_FUNC_INFO << "program arguments:" << args;

}
