#include "Headers/Main/mainwindow.h"
#include "ui_mainwindow.h"

template <typename theFile>
void ProgWindow::loadFile(theFile fileToOpen, QString givenPath){
    clearWindow();
    std::shared_ptr<TFFile> checkFile;
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    fileData.input = true;
    fileToOpen->fileData = &fileData;
    fileToOpen->parent = this;
    QString openSelector;
    openSelector = "Select " + fileToOpen->fileExtension;
    QString openLimiter;
    openLimiter = fileToOpen->fileExtension + " files (*." + fileToOpen->fileExtension + ")";
    QString fileIn;
    if(givenPath == ""){
        fileIn = QFileDialog::getOpenFileName(this, tr(openSelector.toStdString().c_str()), QDir::currentPath() + "/" + fileToOpen->fileExtension + "/", tr(openLimiter.toStdString().c_str()));
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
void ProgWindow::loadBulkFile(theFile fileToOpen){
    clearWindow();
    std::shared_ptr<TFFile> checkFile;
    //qDebug() << Q_FUNC_INFO << "running this function";
    fileData.input = true;
    fileToOpen->fileData = &fileData;
    fileToOpen->parent = this;
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

void ProgWindow::bulkOpen(QString fileType){
    QString filePath = QFileDialog::getExistingDirectory(this, tr(QString("Select " + fileType + " folder.").toStdString().c_str()), QDir::currentPath() + "/" + fileType + "/");
    QDir directory(filePath);
    for(const QFileInfo &checkFile : directory.entryInfoList(QDir::Files)){
        if(checkFile.suffix().toUpper() == fileType){
            if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE"){
                std::shared_ptr<VBIN> vbinFile(new VBIN);
                vbinFile->fileExtension = fileType;
                vbinFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(vbinFile);
            } else if (fileType == "MESH.VBIN"){
                std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
                levelFile->fileExtension = fileType;
                levelFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(levelFile);
            } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
                std::shared_ptr<ITF> itfFile(new ITF);
                itfFile->fileExtension = fileType;
                itfFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(itfFile);
            } else if (fileType == "BMD" or fileType == "TMD"){
                std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
                definitionFile->fileExtension = fileType;
                definitionFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(definitionFile);
            } else if (fileType == "BDB" or fileType == "TDB"){
                std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
                databaseFile->fileExtension = fileType;
                databaseFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(databaseFile);
            } else if (fileType == "VAC"){
                std::shared_ptr<VACFile> vacFile(new VACFile);
                vacFile->fileExtension = fileType;
                vacFile->inputPath = checkFile.filePath();
                ProgWindow::loadBulkFile(vacFile);
            } else {
                qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
                return;
            }
        }
    }
    fileBrowser->setCurrentRow(fileBrowser->count()-1);
}

void ProgWindow::openFile(QString fileType, QString givenPath){
    qDebug() << Q_FUNC_INFO << "running this function for file" << givenPath;
    if(fileType == "VBIN" or fileType == "STL" or fileType == "DAE" or fileType == "GRAPH.VBIN"){
        std::shared_ptr<VBIN> vbinFile(new VBIN);
        vbinFile->fileExtension = fileType;
        vbinFile->isSplitFile = false;
        ProgWindow::loadFile(vbinFile, givenPath);
    } else if (fileType == "MESH.VBIN"){

        std::shared_ptr<MeshVBIN> levelFile(new MeshVBIN);
        levelFile->fileExtension = fileType;
        ProgWindow::loadFile(levelFile, givenPath);

        std::shared_ptr<VBIN> graphFile(new VBIN);
        graphFile->meshFile = levelFile;
        graphFile->isSplitFile = true;
        graphFile->fileExtension = "GRAPH.VBIN";
        ProgWindow::loadFile(graphFile, givenPath);

    } else if (fileType == "ITF" or fileType == "Image Files (*." + QImageWriter::supportedImageFormats().join(";*.")+")"){
        std::shared_ptr<ITF> itfFile(new ITF);
        itfFile->fileExtension = fileType;
        ProgWindow::loadFile(itfFile, givenPath);
    } else if (fileType == "BMD" or fileType == "TMD"){
        std::shared_ptr<DefinitionFile> definitionFile(new DefinitionFile);
        definitionFile->fileExtension = fileType;
        ProgWindow::loadFile(definitionFile, givenPath);
        definitionFile->acceptVisitor(*this);
    } else if (fileType == "BDB" or fileType == "TDB"){
        std::shared_ptr<DatabaseFile> databaseFile(new DatabaseFile);
        databaseFile->fileExtension = fileType;
        ProgWindow::loadFile(databaseFile, givenPath);
        databaseFile->acceptVisitor(*this);
    } else if (fileType == "VAC"){
        std::shared_ptr<VACFile> vacFile(new VACFile);
        vacFile->fileExtension = fileType;
        ProgWindow::loadFile(vacFile, givenPath);
    } else {
        qDebug() << Q_FUNC_INFO << "File type" << fileType << "hasn't been implemented yet.";
        return;
    }

}

//commenting out old functions, some parts of them may still need to be brought to the new system

/*void ProgWindow::openVBIN(){
    fileMode = "VBIN";
    std::shared_ptr<VBIN> vbinFile(new VBIN);
    //VBIN vbinFile;
    clearWindow();
    fileData.input = true;
    QString fileIn = QFileDialog::getOpenFileName(this, tr("Select VBIN"), QDir::currentPath() + "/VBIN/", tr("Model Files (*.vbin)"));
    if (!fileIn.isNull()){
        changeMode(2);
        vbinFile->filePath = fileIn;
        vbinFile->highestLOD = 0;
        vbinFile->parent = this;
        fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        vbinFile->fileName = fileInfo.fileName().left(fileInfo.fileName().indexOf("."));
        vbinFile->fileExtension = fileInfo.fileName().right(fileInfo.fileName().length() - fileInfo.fileName().indexOf(".")-1);
        qDebug() << Q_FUNC_INFO << "file name" << vbinFile->fileName << "extension" << vbinFile->fileExtension;
//        fileData.dataBytes.clear();

//        QFile inputFile(fileIn);
//        inputFile.open(QIODevice::ReadOnly);
//        fileData.dataBytes = inputFile.readAll();

        qDebug() << Q_FUNC_INFO << "File data loaded.";
//        if(vbinFile->readDataVBIN()){
//            messageError("There was an error reading " + vbinFile->fileName);
//            return;
//        }
        qDebug() << Q_FUNC_INFO << "File data read.";
        //vbinFiles.push_back(vbinFile);
        loadedFiles.push_back(vbinFile);
        createLevelList(vbinFile->highestLOD);
        createFileList();
        createMultiRadios();
        //createAnimationList(vbinFile.animationSet);
    } else {
        messageError("There was an error opening the file.");
    }

}

void ProgWindow::openITF(){
    fileMode = "ITF";
    ITF itfFile;
    deleteMultiRadios();
    clearWindow();
    fileData.input = true;
    QString fileIn = QFileDialog::getOpenFileName(this, tr("Select ITF"), QDir::currentPath() + "/ITF/", tr("Texture Files (*.itf)"));
    if (!fileIn.isNull()){
        changeMode(1);
        itfFile.filePath = fileIn;
        itfFile.parent = this;
        fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        itfFile.fileName = fileInfo.fileName();

        qDebug() << Q_FUNC_INFO << "File data loaded.";
        //itfFile.readData();
        itfFiles.push_back(itfFile);
        createLevelList(itfFile.paletteCount);
        createFileList();
        itfFile.populatePalette();
        qDebug() << Q_FUNC_INFO << "File data read.";
    } else {
        messageError("There was an error opening the file.");
    }
}

void ProgWindow::openDefinition(bool binary){
    int passed=0;
    DefinitionFile openedFile;
    QString fileIn;
    clearWindow();
    fileData.input = true;
    if(binary){
        fileIn = QFileDialog::getOpenFileName(this, tr("Select BMD"), QDir::currentPath(), tr("Database Definition Files (*.BMD)"));
    } else {
        fileIn = QFileDialog::getOpenFileName(this, tr("Select TMD"), QDir::currentPath(), tr("Database Definition Files (*.TMD)"));
    }
    if (!fileIn.isNull()){
        openedFile.filePath = fileIn;
        openedFile.parent = this;
        fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        openedFile.fileName = fileInfo.fileName();
        openedFile.binary = binary;
        openedFile.database = false;
        fileData.dataBytes = inputFile.readAll();

        qDebug() << Q_FUNC_INFO << "File data loaded.";
        passed = openedFile.readData();
        qDebug() << Q_FUNC_INFO << "File data read.";
        if (passed != -1){
            createDBButtons(4);
            openedFile.createDBTree();
            definitions.push_back(openedFile);
            changeMode(4);
        } else {
            messageError("There was an error reading the file.");
        }

    } else {
        messageError("There was an error opening the file.");
    }
}

void ProgWindow::openDatabase(bool binary){
    int passed=0;
    DatabaseFile openedFile;
    QString fileIn;
    clearWindow();
    fileData.input = true;
    if(binary){
        fileIn = QFileDialog::getOpenFileName(this, tr("Select BDB"), QDir::currentPath(), tr("Database Definition Files (*.BDB)"));
    } else {
        fileIn = QFileDialog::getOpenFileName(this, tr("Select TDB"), QDir::currentPath(), tr("Database Definition Files (*.TDB)"));
    }
    if (!fileIn.isNull()){
        openedFile.filePath = fileIn;
        openedFile.parent = this;
        fileData.readFile(fileIn);

        QFile inputFile(fileIn);
        inputFile.open(QIODevice::ReadOnly);
        QFileInfo fileInfo(inputFile);
        openedFile.fileName = fileInfo.fileName();
        openedFile.binary = binary;
        openedFile.database = true;
        fileData.dataBytes = inputFile.readAll();

        qDebug() << Q_FUNC_INFO << "File data loaded.";
        passed = openedFile.readData();
        qDebug() << Q_FUNC_INFO << "File data read.";
        if (passed != -1){
            createDBButtons(4);
            openedFile.createDBTree();
            databases.push_back(openedFile);
            changeMode(4);
        } else {
            messageError("There was an error reading the file.");
        }

    } else {
        messageError("There was an error opening the file.");
    }
}

void ProgWindow::openVAC(){
    fileMode = "Tone";
    clearWindow();
    fileData.input = true;
    QString fileIn = QFileDialog::getOpenFileName(this, tr("Select VAC"), QDir::currentPath() + "/VAC/", tr("Audio Files (*.vac)"));
    if (!fileIn.isNull()){
        vacFile->filePath = fileIn;
        vacFile->parent = this;
        fileData.readFile(fileIn);

        qDebug() << Q_FUNC_INFO << "File data loaded.";
        vacFile->tempRead();
        qDebug() << Q_FUNC_INFO << "File data read.";
    } else {
        messageError("There was an error opening the file.");
    }

}*/
