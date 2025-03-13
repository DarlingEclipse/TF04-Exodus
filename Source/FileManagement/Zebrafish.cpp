#include <QFileDialog>
#include "Headers/FileManagement/Zebrafish.h"
#include "Headers/ISOManager/IsoBuilder.h"
#include "Headers/ISOManager/ModHandler.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/UI/exSettings.h"

#include "Headers/Models/vbin.h"
#include "Headers/Models/LevelGeo.h"
#include "Headers/Textures/itf.h"
#include "Headers/Audio/ToneLibraries.h"
#include "Headers/Databases/Database.h"

zlManager::zlManager(exWindowBase *passUI, exSettings *passSettings)
{
    m_UI = passUI;
    m_Debug = &exDebugger::GetInstance();
    m_Settings = passSettings;

    /*Initialize the ISO Builder. Ideally, this will be its own class and not contained within Zebrafish*/
    m_IsoBuilder = new IsoBuilder(this);
    m_ModHandler = new ModHandler(m_UI, this);

    QMenu *menuBuild = m_UI->AddMenu("Build");
    QAction *actionUnpackISO = m_UI->AddAction(menuBuild, "Unpack ISO");
    QAction *actionUnzipZips = m_UI->AddAction(menuBuild, "Unpack special ZIPs");
    QAction *actionModInterface = m_UI->AddAction(menuBuild, "Mods and Replacements");
    QAction *actionZipBuildISO = m_UI->AddAction(menuBuild, "Zip and Build ISO");
    QAction *actionBuildISO = m_UI->AddAction(menuBuild, "Build ISO");
    QAction *actionPackRandom = m_UI->AddAction(menuBuild, "Build Randomizer ISO");


    QAbstractButton::connect(actionUnpackISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->unpackISO();});
    QAbstractButton::connect(actionUnzipZips, &QAction::triggered, m_UI, [this] {m_IsoBuilder->unzipSpecial();});
    QAbstractButton::connect(actionModInterface, &QAction::triggered, m_UI, [this] {openModHandler();});
    QAbstractButton::connect(actionZipBuildISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->rezipTFA_sevenZip(false);});
    QAbstractButton::connect(actionBuildISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->repackISO(false);});
    QAbstractButton::connect(actionPackRandom, &QAction::triggered, m_UI, [this] {m_IsoBuilder->packModded("Rebuild");});


    fileBrowser = new QListWidget;
    m_UI->SetLeftWindow(fileBrowser);
    fileBrowser->setGeometry(QRect(QPoint(0,0), QSize(m_UI->m_hSize*0.15,m_UI->m_vSize)));
    QAbstractItemView::connect(fileBrowser, &QListWidget::itemSelectionChanged, m_UI, []{qDebug() << "Selection changed";});

    QMenu *menuClear = m_UI->AddMenu("File");
    //With the plan to change to a full file browser of the game directory, this shouldn't be needed later.
    QAction *actionClearFiles = m_UI->AddAction(menuClear, "Clear Loaded Files");

    QAbstractButton::connect(actionClearFiles, &QAction::triggered, m_UI, [this]{m_UI->ClearWindow();});

}

void zlManager::openModHandler(){
    m_UI->ClearWindow();
    m_ModHandler->updateCenter();
}


template <typename theFile>
void zlManager::loadFile(theFile fileToOpen, QString givenPath){
    m_UI->ClearWindow();
    std::shared_ptr<taFile> checkFile;
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    fileData.input = true;
    fileToOpen->fileData = &fileData;
    fileToOpen->m_UI = m_UI;
    fileToOpen->m_zlManager = this;
    QString openSelector;
    openSelector = "Select " + fileToOpen->fileExtension;
    QString openLimiter;
    openLimiter = fileToOpen->fileExtension + " files (*." + fileToOpen->fileExtension + ")";
    QString fileIn;
    if(givenPath == ""){
        fileIn = QFileDialog::getOpenFileName(m_UI, m_UI->tr(openSelector.toStdString().c_str()), QDir::currentPath() + "/" + fileToOpen->fileExtension + "/", m_UI->tr(openLimiter.toStdString().c_str()));
    } else {
        fileIn = givenPath;
    }
    qDebug() << Q_FUNC_INFO << "filein:" << fileIn;
    if(!fileIn.isNull()){
        fileMode = fileToOpen->fileExtension;
        fileToOpen->inputPath = fileIn;
        fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        fileToOpen->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
        fileToOpen->fileExtension = fileInfo.fileName().right(fileInfo.fileName().length() - (fileInfo.fileName().indexOf(".")+1)).toUpper();
        checkFile = matchFile(fileToOpen->fullFileName());
        while(checkFile != nullptr){
            fileToOpen->duplicateFileCount = checkFile->duplicateFileCount + 1;
            checkFile = matchFile(fileToOpen->fullFileName());
        }

        fileToOpen->load(fileToOpen->fileExtension);
        loadedFiles.push_back(fileToOpen);
        loadedFileNames.push_back(fileToOpen->fullFileName().toUpper());
        fileBrowser->addItem(fileToOpen->fullFileName());
        qDebug() << Q_FUNC_INFO << "number of items in file browser" << fileBrowser->count();
        if(!bulkLoading){
            /*This sets the most recent file as the selected file and updates the center view. When loading multiple files, we don't need to do this until the last file.*/
            fileBrowser->setCurrentRow(fileBrowser->count()-1);
        }

    }
}

template <typename theFile>
void zlManager::loadBulkFile(theFile fileToOpen){
    m_UI->ClearWindow();
    std::shared_ptr<taFile> checkFile;
    //qDebug() << Q_FUNC_INFO << "running this function";
    fileData.input = true;
    fileToOpen->fileData = &fileData;
    fileToOpen->m_UI = m_UI;
    fileToOpen->m_zlManager = this;
    fileMode = fileToOpen->fileExtension;


    fileData.readFile(fileToOpen->inputPath);

    QFile inputFile(fileToOpen->inputPath);
    inputFile.open(QIODevice::ReadOnly);
    QFileInfo fileInfo(inputFile);
    fileToOpen->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
    checkFile = matchFile(fileToOpen->fullFileName());
    while(checkFile != nullptr){
        fileToOpen->duplicateFileCount = checkFile->duplicateFileCount + 1;
        checkFile = matchFile(fileToOpen->fullFileName());
    }

    fileToOpen->load(fileToOpen->fileExtension);
    loadedFiles.push_back(fileToOpen);
    loadedFileNames.push_back(fileToOpen->fullFileName().toUpper());
    fileBrowser->addItem(fileToOpen->fullFileName());
    //qDebug() << Q_FUNC_INFO << "number of items in file browser" << fileBrowser->count();

}

void zlManager::bulkOpen(QString fileType){
    QString filePath = QFileDialog::getExistingDirectory(m_UI, m_UI->tr(QString("Select " + fileType + " folder.").toStdString().c_str()), QDir::currentPath() + "/" + fileType + "/");
    QDir directory(filePath);
    for(const QFileInfo &checkFile : directory.entryInfoList(QDir::Files)){
        if(checkFile.suffix().toUpper() == fileType){
            if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE"){
                std::shared_ptr<VBIN> vbinFile(new VBIN);
                vbinFile->fileExtension = fileType;
                vbinFile->inputPath = checkFile.filePath();
                zlManager::loadBulkFile(vbinFile);
            } else if (fileType == "MESH.VBIN"){
                std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
                levelFile->fileExtension = fileType;
                levelFile->inputPath = checkFile.filePath();
                loadBulkFile(levelFile);
            } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
                std::shared_ptr<ITF> itfFile(new ITF);
                itfFile->fileExtension = fileType;
                itfFile->inputPath = checkFile.filePath();
                loadBulkFile(itfFile);
            } else if (fileType == "BMD" or fileType == "TMD"){
                std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
                definitionFile->fileExtension = fileType;
                definitionFile->inputPath = checkFile.filePath();
                loadBulkFile(definitionFile);
            } else if (fileType == "BDB" or fileType == "TDB"){
                std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
                databaseFile->fileExtension = fileType;
                databaseFile->inputPath = checkFile.filePath();
                loadBulkFile(databaseFile);
            } else if (fileType == "VAC"){
                std::shared_ptr<VACFile> vacFile(new VACFile);
                vacFile->fileExtension = fileType;
                vacFile->inputPath = checkFile.filePath();
                loadBulkFile(vacFile);
            } else {
                qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
                return;
            }
        }
    }
    fileBrowser->setCurrentRow(fileBrowser->count()-1);
}

void zlManager::openFile(QString fileType, QString givenPath){
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE" or fileType == "GRAPH.VBIN"){
        std::shared_ptr<VBIN> vbinFile(new VBIN);
        vbinFile->fileExtension = fileType;
        vbinFile->isSplitFile = false;
        zlManager::loadFile(vbinFile, givenPath);
    } else if (fileType == "MESH.VBIN"){

        std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
        levelFile->fileExtension = fileType;
        zlManager::loadFile(levelFile, givenPath);

        std::shared_ptr<VBIN> graphFile(new VBIN);
        graphFile->meshFile = levelFile;
        graphFile->isSplitFile = true;
        graphFile->fileExtension = "GRAPH.VBIN";
        zlManager::loadFile(graphFile, givenPath);

    } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
        std::shared_ptr<ITF> itfFile(new ITF);
        itfFile->fileExtension = fileType;
        zlManager::loadFile(itfFile, givenPath);
    } else if (fileType == "BMD" or fileType == "TMD"){
        std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
        definitionFile->fileExtension = fileType;
        zlManager::loadFile(definitionFile, givenPath);
        //definitionFile->acceptVisitor(*this);
    } else if (fileType == "BDB" or fileType == "TDB"){
        std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
        databaseFile->fileExtension = fileType;
        zlManager::loadFile(databaseFile, givenPath);
        //databaseFile->acceptVisitor(*this);
    } else if (fileType == "VAC"){
        std::shared_ptr<VACFile> vacFile(new VACFile);
        vacFile->fileExtension = fileType;
        zlManager::loadFile(vacFile, givenPath);
    } else {
        qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
        return;
    }

}

/*Might be worth it to make this one function and pass in the enum for the folder type*/
QStringList zlManager::ZippedFolders(bool modded){
    std::vector<taFolder> folderList = m_ModHandler->folderOptions;
    QStringList zippedList;
    for(int i = 0; i < folderList.size(); i++){
        qDebug() << Q_FUNC_INFO << "folder" << folderList[i].m_name << "folder type?" << folderList[i].m_folderType << "is modded?" << folderList[i].m_modded;
        if(folderList[i].m_folderType == taFolderCompressed && folderList[i].m_modded == modded){
            zippedList.push_back(folderList[i].m_name);
        }
    }

    return zippedList;
}

QStringList zlManager::UnzippedFolders(bool modded){
    std::vector<taFolder> folderList = m_ModHandler->folderOptions;
    QStringList unzippedList;
    for(int i = 0; i < folderList.size(); i++){
        qDebug() << Q_FUNC_INFO << "folder" << folderList[i].m_name << "folder type?" << folderList[i].m_folderType << "is modded?" << folderList[i].m_modded;
        if(folderList[i].m_folderType == taFolderUncompressed && folderList[i].m_modded == modded){
            unzippedList.push_back(folderList[i].m_name);
        }
    }

    return unzippedList;
}

void zlManager::loadRequiredFile(taFile* fromFile, QString needFile, QString fileType){
    qDebug() << Q_FUNC_INFO << "dependant file is:" << needFile << "from starting file" << fromFile->fullFileName();
    static QRegularExpression pathRemover = QRegularExpression("../");
    int directoriesToAscend = needFile.count(pathRemover);
    needFile = needFile.remove(pathRemover);
    if(!needFile.contains("."+fileType) && !needFile.contains("." + fileType.toLower())){
        needFile += "."+fileType;
    }

    long storedPosition = fileData.currentPosition;
    QString storedPath = fromFile->inputPath;

    QString startingPath = fromFile->inputPath;
    startingPath.remove(fromFile->fullFileName());

    std::shared_ptr<taFile> testLoaded = matchFile(needFile);
    if(testLoaded == nullptr){
        QDir dir(startingPath);
        qDebug() << Q_FUNC_INFO << "Dir before ascending:" << dir.absolutePath();
        for(int i = 0; i < directoriesToAscend; i++){
            dir.cdUp();
        }
        qDebug() << Q_FUNC_INFO << "Dir after ascending:" << dir.absolutePath();
            //then check if file exists at that path
            //then also check in TFA2 (or TFA)
        QString inheritPath = dir.absolutePath() + "/" + needFile;
        qDebug() << Q_FUNC_INFO << "Checking path" << inheritPath;
        bool isFileInDirectory = QFileInfo::exists(inheritPath);
        qDebug() << Q_FUNC_INFO << "file directory original is" << inheritPath << "and file exists?" << isFileInDirectory;
        if(!isFileInDirectory){
            qDebug() << Q_FUNC_INFO << "file not at expected directory, checking the other TFA";
            if(inheritPath.contains("TFA2")){
                qDebug() << Q_FUNC_INFO << "originally TFA2, changing to TFA";
                inheritPath.replace("TFA2", "TFA");
            } else {
                qDebug() << Q_FUNC_INFO << "originally TFA, changing to TFA2";
                inheritPath.replace("TFA", "TFA2");
            }
        }
        isFileInDirectory = QFileInfo::exists(inheritPath);
        qDebug() << Q_FUNC_INFO << "file directory after check is" << inheritPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            openFile(fileType, inheritPath);
        }
        testLoaded = matchFile(needFile);
    }
    while(testLoaded == nullptr){
        m_Debug->MessageError("Please load file " + needFile);
        openFile(fileType);
        testLoaded = matchFile(needFile);
    }
    fileData.readFile(storedPath);
    fileData.currentPosition = storedPosition;
}

void zlManager::visit(taFile dataFile){
    qDebug() << Q_FUNC_INFO << "invalid file visited:" << dataFile.fullFileName();
}

void zlManager::visit(DatabaseFile dataFile){
    qDebug() << Q_FUNC_INFO << "Correct data file visited:" << dataFile.fullFileName();
    databaseList.push_back(std::make_shared<DatabaseFile> (dataFile));
}

int zlManager::loadDatabases(){
    qDebug() << Q_FUNC_INFO << "Attempting to load all level database files";
    std::shared_ptr<taFile> testLoaded;
    bulkLoading = true;
    //need to prompt the user for the game directory, then use that
    if(m_Settings->GetValue("Game extract path") == ""){
        //can re-empty gamePath if there's an error reading the files
        gamePath = QFileDialog::getExistingDirectory(m_UI, m_UI->tr(QString("Select TF2004 game folder.").toStdString().c_str()), QDir::currentPath());
        m_Settings->SetValue("Game extract path", gamePath);
    } else {
        gamePath = m_Settings->GetValue("Game extract path");
    }
    //then load TMD from TFA2, then load each file from TFA.
    testLoaded = matchFile("CREATURE.TMD");
    if(testLoaded == nullptr){
        QString definitionPath = gamePath + "/TFA2/CREATURE.TMD";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            openFile("TMD", definitionPath);
        }
        testLoaded = matchFile("CREATURE.TMD");

        if(testLoaded == nullptr){
            m_Debug->MessageError("CREATURE.TMD was not found. Database files were not loaded.");
            gamePath = "";
            return 1;
        }
    }

    //Load the METAGAME files for minicon randomization.
    testLoaded = matchFile("METAGAME.TMD");
    if(testLoaded == nullptr){
        QString definitionPath = gamePath + "/TFA/METAGAME.TMD";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            openFile("TMD", definitionPath);
        }
        testLoaded = matchFile("METAGAME.TMD");

        if(testLoaded == nullptr){
            m_Debug->MessageError("METAGAME.TMD was not found. Database files were not loaded.");
            gamePath = "";
            return 1;
        }
    }

    testLoaded = matchFile("METAGAME.TDB");
    if(testLoaded == nullptr){
        QString definitionPath = gamePath + "/TFA/METAGAME.TDB";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            openFile("TDB", definitionPath);
        }
        testLoaded = matchFile("TFA-METAGAME.TDB");

        if(testLoaded == nullptr){
            m_Debug->MessageError("METAGAME.TDB was not found. Database files were not loaded.");
            gamePath = "";
            return 1;
        }
        //adds the database to the database list
        //testLoaded->acceptVisitor(*this);
    }

    QString levelPath = gamePath + "/TFA/LEVELS/EPISODES";
    QStringList levelList = QDir(levelPath).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    int levelCount = levelList.count();
    for(int level = 0; level < levelCount; level++){
        testLoaded = matchFile(levelList[level] + "-CREATURE.BDB");
        if(testLoaded == nullptr){
            QString creaturePath = levelPath + "/" + levelList[level] + "/CREATURE.BDB";
            bool isFileInDirectory = QFileInfo::exists(creaturePath);
            qDebug() << Q_FUNC_INFO << "file directory is" << creaturePath << "and file exists?" << isFileInDirectory;
            if(isFileInDirectory){
                openFile("BDB", creaturePath);
            }
            testLoaded = matchFile(levelList[level] + "-CREATURE.BDB");
        }
        if(testLoaded == nullptr){
            m_Debug->MessageError("CREATURE.BDB was not found. Database files were not loaded.");
            gamePath = "";
            return 1;
        }
        //testLoaded->acceptVisitor(*this);

    }
    bulkLoading = false;
    return 0;
}

int zlManager::createDefinitionFile(){
    //make popup window with all options
    /*for a definition file, need:
        Name - can get this from save dialog
        text or binary
        inherited files - this is sometimes used. Excluding for now
    */

    bool isBinary = false;
    bool isDialogOpen = true;

    //popup

    QDialog *dialogDefinition = m_UI->MakeYesNoPopup(isDialogOpen);
    QCheckBox *comboBinOrText = new QCheckBox("Check box if making a binary file.", dialogDefinition);
    dialogDefinition->setWindowTitle("Create Database Definition");
    comboBinOrText->setGeometry(QRect(QPoint(20,20), QSize(200,30)));

    dialogDefinition->open();

    int resultDialog = 0;
    QAbstractButton::connect(dialogDefinition, &QDialog::finished, m_UI, [&isDialogOpen]() {isDialogOpen = false;});

    while(isDialogOpen){
        m_UI->ForceProcessEvents();
    }
    resultDialog = dialogDefinition->result();
    isBinary = comboBinOrText->isChecked();

    qDebug() << Q_FUNC_INFO << "escaping while loop. isBinary value:" << isBinary << "result value:" << resultDialog;

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process was cancelled.";
        return 1;
    }

    QString selectedType = "TMD";

    if(isBinary){
        selectedType = "BMD";
    }

    QString fileOut = QFileDialog::getSaveFileName(m_UI, m_UI->tr(QString("Select Output "  + selectedType).toStdString().c_str()), QDir::currentPath() + "/" + selectedType + "/"
                                                   , m_UI->tr(QString("Definition File (*." + selectedType + ")").toStdString().c_str()));

    QFile outputFile(fileOut);
    QFileInfo fileInfo(outputFile);
    std::shared_ptr<DefinitionFile> customFile(new DefinitionFile);

    customFile->inputPath = fileOut;
    customFile->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
    customFile->fileExtension = selectedType;
    customFile->m_UI = m_UI;

    loadedFiles.push_back(customFile);
    loadedFileNames.push_back(customFile->fullFileName().toUpper());
    fileBrowser->addItem(customFile->fullFileName());
    qDebug() << Q_FUNC_INFO << "number of items in file browser" << fileBrowser->count();
    fileBrowser->setCurrentRow(fileBrowser->count()-1);
    customFile->updateCenter();

    return 0;

}

int zlManager::createDatabaseFile(){
    //make popup window with all options
    /*for a database file, need:
        Name - can get this from save dialog
        text or binary
        inherited file(s)
    */

    bool isBinary = false;
    bool isDialogOpen = true;

    CustomPopup *dialogDatabase = m_UI->MakeSpecificPopup(isDialogOpen, {"checkbox", "combobox"}, {"Choose inherited file:"});
    dialogDatabase->setWindowTitle("Create Database Definition");
    dialogDatabase->checkOption->setText("Check box if making a binary file.");

    for(int i = 0; i < loadedFiles.size(); i++){
        dialogDatabase->comboOption->addItem(loadedFiles[i]->fullFileName());
    }

    dialogDatabase->open();

    int resultDialog = 0;
    CustomPopup::connect(dialogDatabase, &QDialog::finished, m_UI, [&isDialogOpen]() {isDialogOpen = false;});

    while(isDialogOpen){
        m_UI->ForceProcessEvents();
    }
    resultDialog = dialogDatabase->result();

    qDebug() << Q_FUNC_INFO << "escaping while loop. isBinary value:" << isBinary << "result value:" << resultDialog;

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process was cancelled.";
        return 1;
    }
    isBinary = dialogDatabase->checkOption->isChecked();

    QString selectedType = "TDB";

    if(isBinary){
        selectedType = "BDB";
    }

    QString fileOut = QFileDialog::getSaveFileName(m_UI, m_UI->tr(QString("Select Output "  + selectedType).toStdString().c_str()), QDir::currentPath() + "/" + selectedType + "/"
                                                   , m_UI->tr(QString("Definition File (*." + selectedType + ")").toStdString().c_str()));

    QFile outputFile(fileOut);
    QFileInfo fileInfo(outputFile);
    std::shared_ptr<DatabaseFile> customFile(new DatabaseFile);

    QString fileSelection = dialogDatabase->comboOption->currentText();
    qDebug() << Q_FUNC_INFO << "Selected file to inherit:" << fileSelection;

    customFile->inputPath = fileOut;
    customFile->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
    customFile->fileExtension = selectedType;
    customFile->inheritedFileName = fileSelection;
    customFile->inheritedFile = std::static_pointer_cast<DefinitionFile>(matchFile(fileSelection));
    customFile->maxInstances = 0;
    customFile->m_UI = m_UI;

    loadedFiles.push_back(customFile);
    loadedFileNames.push_back(customFile->fullFileName().toUpper());
    fileBrowser->addItem(customFile->fullFileName());
    qDebug() << Q_FUNC_INFO << "number of items in file browser" << fileBrowser->count();
    fileBrowser->setCurrentRow(fileBrowser->count()-1);
    customFile->updateCenter();

    return 0;

}

std::shared_ptr<taFile> zlManager::matchFile(QString fileNameFull){
    if(loadedFileNames.contains(fileNameFull.toUpper())){
        return loadedFiles[loadedFileNames.indexOf(fileNameFull.toUpper())];
    } else {
        qDebug() << Q_FUNC_INFO << "loaded file names:" << loadedFileNames;
        //log("File " + fileNameFull + " was not found.");
    }
    /*for(int i = 0; i < loadedFiles.size(); i++){
        qDebug() << Q_FUNC_INFO << "loaded file" << loadedFiles[i]->fullFileName().toUpper() << "vs" << fileNameFull.toUpper();
        if(loadedFiles[i]->fullFileName().toUpper() == fileNameFull.toUpper()){
            return loadedFiles[i];
        }
    }*/
    return nullptr;
}
