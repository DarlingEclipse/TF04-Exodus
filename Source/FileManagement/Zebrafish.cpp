#include <QFileDialog>
#include <QInputDialog>
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
    qDebug() << Q_FUNC_INFO << "Initializing Zebrafish file manager.";
    m_UI = passUI;
    m_Debug = &exDebugger::GetInstance();
    m_Settings = passSettings;

    /*Initialize the ISO Builder. Ideally, this will be its own class and not contained within Zebrafish*/
    m_IsoBuilder = new IsoBuilder(m_UI, this);
    m_ModHandler = new ModHandler(m_UI, this);

    AddMenuItems();

    m_fileBrowser = new QListWidget;
    m_UI->SetLeftWindow(m_fileBrowser);
    m_fileBrowser->setGeometry(QRect(QPoint(0,0), QSize(m_UI->m_hSize*0.15,m_UI->m_vSize)));
    QAbstractItemView::connect(m_fileBrowser, &QListWidget::itemSelectionChanged, m_UI, [this]{MatchFile(m_fileBrowser->currentItem()->text())->updateCenter();});

}

void zlManager::AddMenuItems(){
    QMenu *menuVBIN = m_UI->AddMenu("Model");
    QAction *actionLoadVBIN = m_UI->AddAction(menuVBIN, "Load VBIN");
    QAction *actionLoadMeshVBIN = m_UI->AddAction(menuVBIN, "Load Mesh VBIN");
    QAction *actionSaveModel = m_UI->AddAction(menuVBIN, "Export Model");
    QAction *actionBulkLoadModel = m_UI->AddAction(menuVBIN, "Bulk Load Model");
    QAction *actionBulkSaveModel = m_UI->AddAction(menuVBIN, "Bulk Export Model");
    QAbstractButton::connect(actionSaveModel, &QAction::triggered, m_UI, [this] {SaveFile("Model");});
    QAbstractButton::connect(actionLoadMeshVBIN, &QAction::triggered, m_UI, [this] {OpenFile("MESH.VBIN");});
    QAbstractButton::connect(actionLoadVBIN, &QAction::triggered, m_UI, [this] {OpenFile("VBIN");});
    QAbstractButton::connect(actionBulkLoadModel, &QAction::triggered, m_UI, [this] {BulkOpen("VBIN");});
    QAbstractButton::connect(actionBulkSaveModel, &QAction::triggered, m_UI, [this] {BulkSave("Model");});


    QMenu *menuITF = m_UI->AddMenu("Texture");
    QAction *actionLoadITF = m_UI->AddAction(menuITF, "Load ITF");
    QAction *actionLoadQImage = m_UI->AddAction(menuITF, "Import Image");
    QAction *actionSaveITF = m_UI->AddAction(menuITF, "Export Texture");
    QAction *actionBulkLoadITF = m_UI->AddAction(menuITF, "Bulk Load Texture");
    QAction *actionBulkSaveITF = m_UI->AddAction(menuITF, "Bulk Export Texture");
    QAbstractButton::connect(actionLoadITF, &QAction::triggered, m_UI, [this] {OpenFile("ITF");});
    QAbstractButton::connect(actionSaveITF, &QAction::triggered, m_UI, [this] {SaveFile("Texture");});
    QAbstractButton::connect(actionLoadQImage, &QAction::triggered, m_UI, [this] {OpenFile("Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")");});
    QAbstractButton::connect(actionBulkLoadITF, &QAction::triggered, m_UI, [this] {BulkOpen("ITF");});
    QAbstractButton::connect(actionBulkSaveITF, &QAction::triggered, m_UI, [this] {BulkSave("Texture");});


    QMenu *menuSFX = m_UI->AddMenu("Sound");
    QAction *actionLoadVAC = m_UI->AddAction(menuSFX, "Load VAC");
    QAction *actionSaveVAC = m_UI->AddAction(menuSFX, "Export VAC");
    QAction *actionBulkVAC = m_UI->AddAction(menuSFX, "Bulk Export Sound");


    QMenu *menuDatabase = m_UI->AddMenu("Database");
    QAction *actionCreateDefinition = m_UI->AddAction(menuDatabase, "Create Definition");
    QAction *actionCreateDatabase = m_UI->AddAction(menuDatabase, "Create Database");
    QAction *actionLoadTMD = m_UI->AddAction(menuDatabase, "Load TMD");
    QAction *actionLoadTDB = m_UI->AddAction(menuDatabase, "Load TDB");
    QAction *actionLoadBMD = m_UI->AddAction(menuDatabase, "Load BMD");
    QAction *actionLoadBDB = m_UI->AddAction(menuDatabase, "Load BDB");
    QAction *actionSaveDatabase = m_UI->AddAction(menuDatabase, "Export Database");
    QAction *actionBulkDatabase = m_UI->AddAction(menuDatabase, "Bulk Export Database");
    QAbstractButton::connect(actionSaveDatabase, &QAction::triggered, m_UI, [this] {SaveFile("Database");});
    QAbstractButton::connect(actionBulkDatabase, &QAction::triggered, m_UI, [this] {BulkSave("Database");});
    QAbstractButton::connect(actionLoadTMD, &QAction::triggered, m_UI, [this] {OpenFile("TMD");});
    QAbstractButton::connect(actionLoadBMD, &QAction::triggered, m_UI, [this] {OpenFile("BMD");});
    QAbstractButton::connect(actionLoadTDB, &QAction::triggered, m_UI, [this] {OpenFile("TDB");});
    QAbstractButton::connect(actionLoadBDB, &QAction::triggered, m_UI, [this] {OpenFile("BDB");});
    QAbstractButton::connect(actionCreateDefinition, &QAction::triggered, m_UI, [this] {CreateDefinitionFile();});
    QAbstractButton::connect(actionCreateDatabase, &QAction::triggered, m_UI, [this] {CreateDatabaseFile();});


    QMenu *menuClear = m_UI->AddMenu("Files");
    QAction *actionClearFiles = m_UI->AddAction(menuClear, "Clear Loaded Files");
    QAbstractButton::connect(actionClearFiles, &QAction::triggered, m_UI, [this] {m_UI->ClearWindow(); ClearFiles();});


    QMenu *menuBuild = m_UI->AddMenu("Build");
    QAction *actionUnpackISO = m_UI->AddAction(menuBuild, "Unpack ISO");
    QAction *actionUnzipZips = m_UI->AddAction(menuBuild, "Unpack special ZIPs");
    QAction *actionModInterface = m_UI->AddAction(menuBuild, "Mods and Replacements");
    QAction *actionZipBuildISO = m_UI->AddAction(menuBuild, "Zip and Build ISO");
    QAction *actionBuildISO = m_UI->AddAction(menuBuild, "Build ISO");
    QAction *actionPackRandom = m_UI->AddAction(menuBuild, "Build Randomizer ISO");


    QAbstractButton::connect(actionUnpackISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->unpackISO();});
    QAbstractButton::connect(actionUnzipZips, &QAction::triggered, m_UI, [this] {m_IsoBuilder->unzipSpecial();});
    QAbstractButton::connect(actionModInterface, &QAction::triggered, m_UI, [this] {OpenModHandler();});
    QAbstractButton::connect(actionZipBuildISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->rezipTFA_sevenZip(false);});
    QAbstractButton::connect(actionBuildISO, &QAction::triggered, m_UI, [this] {m_IsoBuilder->repackISO(false);});
    QAbstractButton::connect(actionPackRandom, &QAction::triggered, m_UI, [this] {m_IsoBuilder->packModded("Rebuild");});

}

void zlManager::OpenModHandler(){
    m_UI->ClearWindow();
    m_ModHandler->updateCenter();
}


template <typename theFile>
void zlManager::LoadFile(theFile fileToOpen, QString givenPath){
    m_UI->ClearWindow();
    std::shared_ptr<taFile> checkFile;
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    m_fileData.input = true;
    fileToOpen->fileData = &m_fileData;
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
        m_fileMode = fileToOpen->fileExtension;
        fileToOpen->inputPath = fileIn;
        m_fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        fileToOpen->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
        fileToOpen->fileExtension = fileInfo.fileName().right(fileInfo.fileName().length() - (fileInfo.fileName().indexOf(".")+1)).toUpper();
        checkFile = MatchFile(fileToOpen->fullFileName());
        while(checkFile != nullptr){
            fileToOpen->duplicateFileCount = checkFile->duplicateFileCount + 1;
            checkFile = MatchFile(fileToOpen->fullFileName());
        }

        fileToOpen->load(fileToOpen->fileExtension);
        m_loadedFiles.push_back(fileToOpen);
        m_loadedFileNames.push_back(fileToOpen->fullFileName().toUpper());
        m_fileBrowser->addItem(fileToOpen->fullFileName());
        qDebug() << Q_FUNC_INFO << "number of items in file browser" << m_fileBrowser->count();
        if(!m_bulkLoading){
            /*This sets the most recent file as the selected file and updates the center view. When loading multiple files, we don't need to do this until the last file.*/
            m_fileBrowser->setCurrentRow(m_fileBrowser->count()-1);
        }

    }
}

template <typename theFile>
void zlManager::LoadBulkFile(theFile fileToOpen){
    m_UI->ClearWindow();
    std::shared_ptr<taFile> checkFile;
    //qDebug() << Q_FUNC_INFO << "running this function";
    m_fileData.input = true;
    fileToOpen->fileData = &m_fileData;
    fileToOpen->m_UI = m_UI;
    fileToOpen->m_zlManager = this;
    m_fileMode = fileToOpen->fileExtension;


    m_fileData.readFile(fileToOpen->inputPath);

    QFile inputFile(fileToOpen->inputPath);
    inputFile.open(QIODevice::ReadOnly);
    QFileInfo fileInfo(inputFile);
    fileToOpen->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
    checkFile = MatchFile(fileToOpen->fullFileName());
    while(checkFile != nullptr){
        fileToOpen->duplicateFileCount = checkFile->duplicateFileCount + 1;
        checkFile = MatchFile(fileToOpen->fullFileName());
    }

    fileToOpen->load(fileToOpen->fileExtension);
    m_loadedFiles.push_back(fileToOpen);
    m_loadedFileNames.push_back(fileToOpen->fullFileName().toUpper());
    m_fileBrowser->addItem(fileToOpen->fullFileName());
    //qDebug() << Q_FUNC_INFO << "number of items in file browser" << fileBrowser->count();

}

void zlManager::BulkOpen(QString fileType){
    QString filePath = QFileDialog::getExistingDirectory(m_UI, m_UI->tr(QString("Select " + fileType + " folder.").toStdString().c_str()), QDir::currentPath() + "/" + fileType + "/");
    QDir directory(filePath);
    for(const QFileInfo &checkFile : directory.entryInfoList(QDir::Files)){
        if(checkFile.suffix().toUpper() == fileType){
            if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE"){
                std::shared_ptr<VBIN> vbinFile(new VBIN);
                vbinFile->fileExtension = fileType;
                vbinFile->inputPath = checkFile.filePath();
                LoadBulkFile(vbinFile);
            } else if (fileType == "MESH.VBIN"){
                std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
                levelFile->fileExtension = fileType;
                levelFile->inputPath = checkFile.filePath();
                LoadBulkFile(levelFile);
            } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
                std::shared_ptr<ITF> itfFile(new ITF);
                itfFile->fileExtension = fileType;
                itfFile->inputPath = checkFile.filePath();
                LoadBulkFile(itfFile);
            } else if (fileType == "BMD" or fileType == "TMD"){
                std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
                definitionFile->fileExtension = fileType;
                definitionFile->inputPath = checkFile.filePath();
                LoadBulkFile(definitionFile);
            } else if (fileType == "BDB" or fileType == "TDB"){
                std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
                databaseFile->fileExtension = fileType;
                databaseFile->inputPath = checkFile.filePath();
                LoadBulkFile(databaseFile);
            } else if (fileType == "VAC"){
                std::shared_ptr<VACFile> vacFile(new VACFile);
                vacFile->fileExtension = fileType;
                vacFile->inputPath = checkFile.filePath();
                LoadBulkFile(vacFile);
            } else {
                qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
                return;
            }
        }
    }
    m_fileBrowser->setCurrentRow(m_fileBrowser->count()-1);
}

void zlManager::OpenFile(QString fileType, QString givenPath){
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE" or fileType == "GRAPH.VBIN"){
        std::shared_ptr<VBIN> vbinFile(new VBIN);
        vbinFile->fileExtension = fileType;
        vbinFile->isSplitFile = false;
        zlManager::LoadFile(vbinFile, givenPath);
    } else if (fileType == "MESH.VBIN"){

        std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
        levelFile->fileExtension = fileType;
        zlManager::LoadFile(levelFile, givenPath);

        std::shared_ptr<VBIN> graphFile(new VBIN);
        graphFile->meshFile = levelFile;
        graphFile->isSplitFile = true;
        graphFile->fileExtension = "GRAPH.VBIN";
        zlManager::LoadFile(graphFile, givenPath);

    } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
        std::shared_ptr<ITF> itfFile(new ITF);
        itfFile->fileExtension = fileType;
        zlManager::LoadFile(itfFile, givenPath);
    } else if (fileType == "BMD" or fileType == "TMD"){
        std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
        definitionFile->fileExtension = fileType;
        zlManager::LoadFile(definitionFile, givenPath);
        definitionFile->acceptVisitor(*this);
    } else if (fileType == "BDB" or fileType == "TDB"){
        std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
        databaseFile->fileExtension = fileType;
        zlManager::LoadFile(databaseFile, givenPath);
        databaseFile->acceptVisitor(*this);
    } else if (fileType == "VAC"){
        std::shared_ptr<VACFile> vacFile(new VACFile);
        vacFile->fileExtension = fileType;
        zlManager::LoadFile(vacFile, givenPath);
    } else {
        qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
        return;
    }

}

void zlManager::SaveFile(QString fromType, QString givenPath){
    QStringList validFiles;
    QString selectedFile;
    QString currentCenterFile;
    int centerFileIndex = 0;
    bool completed;
    qDebug() << Q_FUNC_INFO << "loaded files" << m_loadedFiles.size();
    for(int i = 0; i < m_loadedFiles.size(); i++){
        //qDebug() << Q_FUNC_INFO << "loaded file" << loadedFiles[i]->fileName << "with extension" << loadedFiles[i]->fileExtension << "vs" << fromType;
        if(m_loadedFiles[i]->fileCategory() == fromType){
            validFiles.append(m_loadedFiles[i]->fullFileName());
        }
    }
    if(validFiles.isEmpty()){
        m_Debug->MessageError("No loaded files of the selected type: " + fromType);
        return;
    }

    currentCenterFile = m_fileBrowser->currentItem()->text();
    if(validFiles.contains(currentCenterFile)){
        qDebug() << Q_FUNC_INFO << "File was found in the list.";
        //I would use validFiles.indexOf(currentCenterFile) but that doesn't work
        for(int i = 0; i < validFiles.size(); i++){
            qDebug() << Q_FUNC_INFO << "comparing" << validFiles[i] << "to" << currentCenterFile;
            if(validFiles[i] == currentCenterFile){
                centerFileIndex = i;
            }
        }
    }
    qDebug() << Q_FUNC_INFO << "center file index:" << centerFileIndex;

    selectedFile = QInputDialog::getItem(m_UI, m_UI->tr("Select a file:"), m_UI->tr("File name:"), validFiles, centerFileIndex, false, &completed);
    if(!completed){
        m_Debug->MessageError("File save operation cancelled.");
        return;
    }
    for(int i = 0; i < m_loadedFiles.size(); i++){
        if(m_loadedFiles[i]->fullFileName() != selectedFile){
            continue;
        }
        QString selectedType = QInputDialog::getItem(m_UI, m_UI->tr("Select an output type:"), m_UI->tr("File type:"), m_loadedFiles[i]->validOutputs(), 0, false, &completed);
        qDebug() << Q_FUNC_INFO << "completed value:" << completed;
        if(!completed){
            m_Debug->MessageError("File save operation cancelled.");
            return;
        }

        QString fileOut;
        if(givenPath == ""){
            fileOut = QFileDialog::getSaveFileName(m_UI, m_UI->tr(QString("Select Output "  + selectedType).toStdString().c_str()), QDir::currentPath() + "/" + selectedType + "/"
                                                   , m_UI->tr(QString(m_loadedFiles[i]->fileCategory() + " Files (*." + selectedType + ")").toStdString().c_str()));
        } else {
            fileOut = givenPath;
        }


        if(fileOut.isEmpty()){
            m_Debug->MessageError(selectedType + " export cancelled.");
            return;
        }
        m_loadedFiles[i]->outputPath = fileOut;
        m_loadedFiles[i]->save(selectedType);
    }

}

void zlManager::BulkSave(QString category){
    QStringList validFiles;
    QString selectedType = "";
    QString currentCenterFile;
    int centerFileIndex = 0;
    bool completed;
    qDebug() << Q_FUNC_INFO << "loaded files" << m_loadedFiles.size();
    for(int i = 0; i < m_loadedFiles.size(); i++){
        //qDebug() << Q_FUNC_INFO << "loaded file" << loadedFiles[i]->fileName << "with extension" << loadedFiles[i]->fileExtension << "vs" << fromType;
        if(m_loadedFiles[i]->fileCategory() != category){
            continue;
        }
        if(selectedType == ""){
            selectedType = QInputDialog::getItem(m_UI, m_UI->tr("Select an output type:"), m_UI->tr("File type:"), m_loadedFiles[i]->validOutputs(), 0, false, &completed);
            if(!completed){
                m_Debug->MessageError("File save operation cancelled.");
                return;
            }
        }
        validFiles.append(m_loadedFiles[i]->fullFileName());
    }
    if(validFiles.isEmpty()){
        m_Debug->MessageError("No loaded files of the selected type: " + category);
        return;
    }

    QString filePath = QFileDialog::getExistingDirectory(m_UI, m_UI->tr("Select output folder"), QDir::currentPath());

    if(filePath.isEmpty()){
        m_Debug->MessageError(category + " export cancelled.");
        return;
    }
    for(int i = 0; i < m_loadedFiles.size(); i++){
        if(m_loadedFiles[i]->fileCategory() != category){
            continue;
        }

        m_loadedFiles[i]->outputPath = filePath + "\\" + m_loadedFiles[i]->fileName + "." + selectedType;
        m_loadedFiles[i]->save(selectedType);
        m_Debug->MessageSuccess(selectedType + " file saved.");
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

void zlManager::LoadRequiredFile(taFile* fromFile, QString needFile, QString fileType){
    qDebug() << Q_FUNC_INFO << "dependant file is:" << needFile << "from starting file" << fromFile->fullFileName();
    static QRegularExpression pathRemover = QRegularExpression("../");
    int directoriesToAscend = needFile.count(pathRemover);
    needFile = needFile.remove(pathRemover);
    if(!needFile.contains("."+fileType) && !needFile.contains("." + fileType.toLower())){
        needFile += "."+fileType;
    }

    long storedPosition = m_fileData.currentPosition;
    QString storedPath = fromFile->inputPath;

    QString startingPath = fromFile->inputPath;
    startingPath.remove(fromFile->fullFileName());

    std::shared_ptr<taFile> testLoaded = MatchFile(needFile);
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
            OpenFile(fileType, inheritPath);
        }
        testLoaded = MatchFile(needFile);
    }
    while(testLoaded == nullptr){
        m_Debug->MessageError("Please load file " + needFile);
        OpenFile(fileType);
        testLoaded = MatchFile(needFile);
    }
    m_fileData.readFile(storedPath);
    m_fileData.currentPosition = storedPosition;
}

void zlManager::Visit(taFile dataFile){
    qDebug() << Q_FUNC_INFO << "invalid file visited:" << dataFile.fullFileName();
}

void zlManager::Visit(DatabaseFile dataFile){
    qDebug() << Q_FUNC_INFO << "Correct data file visited:" << dataFile.fullFileName();
    m_databaseList.push_back(std::make_shared<DatabaseFile> (dataFile));
}

int zlManager::LoadDatabases(){
    qDebug() << Q_FUNC_INFO << "Attempting to load all level database files";
    std::shared_ptr<taFile> testLoaded;
    m_bulkLoading = true;
    //need to prompt the user for the game directory, then use that
    if(m_Settings->GetValue("Game extract path") == ""){
        //can re-empty gamePath if there's an error reading the files
        m_gamePath = QFileDialog::getExistingDirectory(m_UI, m_UI->tr(QString("Select TF2004 game folder.").toStdString().c_str()), QDir::currentPath());
        m_Settings->SetValue("Game extract path", m_gamePath);
    } else {
        m_gamePath = m_Settings->GetValue("Game extract path");
    }
    //then load TMD from TFA2, then load each file from TFA.
    testLoaded = MatchFile("CREATURE.TMD");
    if(testLoaded == nullptr){
        QString definitionPath = m_gamePath + "/TFA2/CREATURE.TMD";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            OpenFile("TMD", definitionPath);
        }
        testLoaded = MatchFile("CREATURE.TMD");

        if(testLoaded == nullptr){
            m_Debug->MessageError("CREATURE.TMD was not found. Database files were not loaded.");
            m_gamePath = "";
            return 1;
        }
    }

    //Load the METAGAME files for minicon randomization.
    testLoaded = MatchFile("METAGAME.TMD");
    if(testLoaded == nullptr){
        QString definitionPath = m_gamePath + "/TFA/METAGAME.TMD";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            OpenFile("TMD", definitionPath);
        }
        testLoaded = MatchFile("METAGAME.TMD");

        if(testLoaded == nullptr){
            m_Debug->MessageError("METAGAME.TMD was not found. Database files were not loaded.");
            m_gamePath = "";
            return 1;
        }
    }

    testLoaded = MatchFile("METAGAME.TDB");
    if(testLoaded == nullptr){
        QString definitionPath = m_gamePath + "/TFA/METAGAME.TDB";
        bool isFileInDirectory = QFileInfo::exists(definitionPath);
        qDebug() << Q_FUNC_INFO << "file directory is" << definitionPath << "and file exists?" << isFileInDirectory;
        if(isFileInDirectory){
            OpenFile("TDB", definitionPath);
        }
        testLoaded = MatchFile("TFA-METAGAME.TDB");

        if(testLoaded == nullptr){
            m_Debug->MessageError("METAGAME.TDB was not found. Database files were not loaded.");
            m_gamePath = "";
            return 1;
        }
        //adds the database to the database list
        //testLoaded->acceptVisitor(*this);
    }

    QString levelPath = m_gamePath + "/TFA/LEVELS/EPISODES";
    QStringList levelList = QDir(levelPath).entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    int levelCount = levelList.count();
    for(int level = 0; level < levelCount; level++){
        testLoaded = MatchFile(levelList[level] + "-CREATURE.BDB");
        if(testLoaded == nullptr){
            QString creaturePath = levelPath + "/" + levelList[level] + "/CREATURE.BDB";
            bool isFileInDirectory = QFileInfo::exists(creaturePath);
            qDebug() << Q_FUNC_INFO << "file directory is" << creaturePath << "and file exists?" << isFileInDirectory;
            if(isFileInDirectory){
                OpenFile("BDB", creaturePath);
            }
            testLoaded = MatchFile(levelList[level] + "-CREATURE.BDB");
        }
        if(testLoaded == nullptr){
            m_Debug->MessageError("CREATURE.BDB was not found. Database files were not loaded.");
            m_gamePath = "";
            return 1;
        }
        //testLoaded->acceptVisitor(*this);

    }
    m_bulkLoading = false;
    return 0;
}

int zlManager::CreateDefinitionFile(){
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

    m_loadedFiles.push_back(customFile);
    m_loadedFileNames.push_back(customFile->fullFileName().toUpper());
    m_fileBrowser->addItem(customFile->fullFileName());
    qDebug() << Q_FUNC_INFO << "number of items in file browser" << m_fileBrowser->count();
    m_fileBrowser->setCurrentRow(m_fileBrowser->count()-1);
    customFile->updateCenter();

    return 0;

}

int zlManager::CreateDatabaseFile(){
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

    for(int i = 0; i < m_loadedFiles.size(); i++){
        dialogDatabase->comboOption->addItem(m_loadedFiles[i]->fullFileName());
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
    customFile->inheritedFile = std::static_pointer_cast<DefinitionFile>(MatchFile(fileSelection));
    customFile->maxInstances = 0;
    customFile->m_UI = m_UI;

    m_loadedFiles.push_back(customFile);
    m_loadedFileNames.push_back(customFile->fullFileName().toUpper());
    m_fileBrowser->addItem(customFile->fullFileName());
    qDebug() << Q_FUNC_INFO << "number of items in file browser" << m_fileBrowser->count();
    m_fileBrowser->setCurrentRow(m_fileBrowser->count()-1);
    customFile->updateCenter();

    return 0;

}

std::shared_ptr<taFile> zlManager::MatchFile(QString fileNameFull){
    if(m_loadedFileNames.contains(fileNameFull.toUpper())){
        return m_loadedFiles[m_loadedFileNames.indexOf(fileNameFull.toUpper())];
    } else {
        qDebug() << Q_FUNC_INFO << "loaded file names:" << m_loadedFileNames;
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

void zlManager::ClearFiles(){
    m_loadedFiles.clear();
}
