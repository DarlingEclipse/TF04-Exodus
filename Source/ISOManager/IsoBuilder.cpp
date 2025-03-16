#include <QCoreApplication>
#include <QFileDialog>

#include "Headers/ISOManager/IsoBuilder.h"
#include "Headers/FileManagement/Zebrafish.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/UI/exSettings.h"

//https://doc.qt.io/qt-6/qprocess.html

IsoBuilder::IsoBuilder(zlManager *fileManager){
    m_zlManager = fileManager;
}

void IsoBuilder::setCopyPath(QString folderName){
    QDir gameParent(m_zlManager->m_Settings->GetValue("Game extract path"));
    gameParent.cdUp();
    m_zlManager->m_copyOutputPath = gameParent.absolutePath() + "/" + folderName;
    //get containing directory
    //create Randomizer folder in that directory
    qDebug() << Q_FUNC_INFO << "path being checked:" << m_zlManager->m_copyOutputPath << "based on " << gameParent.absolutePath();
    QDir checkDir(m_zlManager->m_copyOutputPath);
    if(!checkDir.exists()){
        checkDir.mkpath(".");
    }
}

int IsoBuilder::unpackISO(){
    //calls TF04 ISO Manager to unpack and attempt to unzip TFA etc
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString isoManagerPath = QCoreApplication::applicationDirPath() + "/TF04_ISO_Manager.exe";
    qDebug() << Q_FUNC_INFO << isoManagerPath;

    QStringList args;
    args.append("Unpack");
    QString isoPath = QFileDialog::getOpenFileName(m_UI, m_UI->tr("Choose game ISO"), QDir::currentPath(), m_UI->tr("*.iso"));
    if(isoPath == ""){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return 1;
    }
    m_Debug->Log("Unpacking ISO from: " + isoPath);
    args.append(isoPath);

    QProcess *isoManager = new QProcess();
    QObject::connect(isoManager, &QProcess::readyReadStandardOutput, [this,isoManager]() {
        //below only works if python output is flushed.
        QString debugOutput = isoManager->readAllStandardOutput();
        QDir dirCorrection("");
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
        case 1:
            //ISOManager was unable to unzip the archives. Call 7zip unzipper
            m_Debug->Log("ISO Manager was not able to extract ZIP files. Calling 7zip.");
            unzipSpecial();
            break;
        case 2:
            //ISOManager has provided the clean extract directory
            debugOutput = debugOutput.right(debugOutput.length() - 5).trimmed();
            dirCorrection.setPath(debugOutput);
            //qDebug() << Q_FUNC_INFO << "current path" << debugOutput << "Corrected game path" << dirCorrection.absolutePath();
            m_zlManager->m_Settings->SetValue("Game extract path", dirCorrection.absolutePath());
            m_zlManager->m_Settings->SetValue("Modded game path", dirCorrection.absolutePath());
            break;
        default:
            //either nothing to worry about or something we haven't handled yet. output it.
            qDebug() << "Unhandled output code:" << debugOutput;
            break;
        }
    });
    isoManager->start(isoManagerPath, args);
    m_Debug->Log("Calling ISO Manager.");

    return 0;
}

void IsoBuilder::handleOutputCode(QString output){
    /*This was originally called from each of the readyReadStandardOutput connections, but it kept getting empty strings*/
    /*using this, we can check for specified codes or keywords at the start of the python debug output
    for now, I think these outputs will be simply "Code ____: human definition"
    then QT can read the first 4 for Code, skip the space, and qstring::number() the next four to react accordingly*/

}

int IsoBuilder::unzipSpecial(){
    //calls 7zip to unzip TFA if the ISO Manager was unable to
    m_zlManager->m_Settings->LoadSettings();
    QString sevenZipPath = m_zlManager->m_Settings->GetValue("7Zip EXE path");
    if(sevenZipPath == ""){
        //prompt user for 7zip location
        m_Debug->Log("7Zip executable path is not set. Please locate 7z.exe.");
        sevenZipPath = QFileDialog::getOpenFileName(m_UI, m_UI->tr("Locate 7Zip EXE"), QDir::currentPath(), m_UI->tr("7z.exe"));
        QDir dirCorrection(sevenZipPath);
        m_zlManager->m_Settings->SetValue("7Zip EXE path", dirCorrection.absolutePath());
    }

    QString outputPath = m_zlManager->m_Settings->GetValue("Game extract path");

    qDebug() << Q_FUNC_INFO << "Loading 7zip from:" << sevenZipPath;
    qDebug() << Q_FUNC_INFO <<"Targetting directory:" << outputPath;

    QStringList args;
    args.append("x");
    args.append(outputPath + "\\*.zip");
    args.append("-o" + outputPath + "\\*");
    args.append("-tzip");

    qDebug() << Q_FUNC_INFO << "args:" << args;

    QProcess *sevenZip = new QProcess();
    m_UI->UpdateLoadingBar(0,4);
    QObject::connect(sevenZip, &QProcess::readyReadStandardOutput, [this, sevenZip]() {
        QString debugOutput = sevenZip->readAllStandardOutput();
        qDebug() << debugOutput;
        int outputCode = 0;
        if(debugOutput.contains("Archives with Errors: 4")){
            outputCode = 1005;
        } else if (debugOutput.contains("Would you like to replace the existing file")){
            outputCode = 1007;
        } else if (debugOutput.contains("Unexpected end of archive")){
            outputCode = 1006;
        } else {
            outputCode = 0;
        }
        switch(outputCode){
        case 0:
            //nothing to worry about
            break;
        case 1005:
            //All extraction has completed
            qDebug() << Q_FUNC_INFO << "Extraction completed";
            break;
        case 1006:
            //One extraction has been completed
            qDebug() << Q_FUNC_INFO << "Extraction for file has completed";
            m_UI->UpdateLoadingBar();
            break;
        case 1007:
            //Files are already extracted
            qDebug() << Q_FUNC_INFO << "Extraction already completed.";
            sevenZip->close();
            m_UI->UpdateLoadingBar(4);
            break;
        default:
            //either nothing to worry about or something we haven't handled yet. output it.
            qDebug() << "Unhandled output code:" << debugOutput;
            break;
        }
    });
    sevenZip->start(sevenZipPath, args);
    return 0;
}
int IsoBuilder::rezipTFA_isoManager(bool removeFiles){
    //calls TF04 ISO Manager to repack TFA etc
    //removeFiles currently does nothing in this function - only in the ISO rebuild
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString isoManagerPath = QCoreApplication::applicationDirPath() + "/TF04_ISO_Manager.exe";
    qDebug() << Q_FUNC_INFO << isoManagerPath;

    QStringList args;
    args.append("Build");
    QString extractPath = m_zlManager->m_Settings->GetValue("Modded game path");
    if(extractPath == ""){
        m_Debug->Log("Process cancelled. Extract path is not set.");
        return 1;
    }
    args.append(extractPath);

    QProcess *isoManager = new QProcess();
    QObject::connect(isoManager, &QProcess::readyReadStandardOutput, [this,isoManager, &removeFiles]() {
        //below only works if python output is flushed.
        QString debugOutput = isoManager->readAllStandardOutput();
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
        case 3:
            //ISOManager is unable to build a bootable ISO. Call ImgBurn
            m_Debug->Log("ISOManager is unable to make a bootable ISO. Calling ImgBurn.");
            repackISO(removeFiles);
        default:
            //either nothing to worry about or something we haven't handled yet. output it.
            qDebug() << "Unhandled output code:" << debugOutput;
            break;
        }
    });
    m_Debug->Log("Starting ISOManager to rezip archives.");
    isoManager->start(isoManagerPath, args);

    return 0;
}

int IsoBuilder::rezipTFA_sevenZip(bool removeFiles){
    //calls 7zip to repack TFA etc
    //removeFiles currently does nothing in this function - only in the ISO rebuild
    m_zlManager->m_Settings->LoadSettings();
    QString sevenZipPath = m_zlManager->m_Settings->GetValue("7Zip EXE path");
    if(sevenZipPath == ""){
        //prompt user for 7zip location
        m_Debug->Log("7Zip executable path is not set. Please locate 7z.exe.");
        sevenZipPath = QFileDialog::getOpenFileName(m_UI, m_UI->tr("Locate 7Zip EXE"), QDir::currentPath(), m_UI->tr("7z.exe"));
        if(sevenZipPath == ""){
            m_Debug->MessageError("7zip was not located. Build process cancelled.");
            return 0;
        }
        QDir dirCorrection(sevenZipPath);
        m_zlManager->m_Settings->SetValue("7Zip EXE path", dirCorrection.absolutePath());
    }

    QString inputPath = m_zlManager->m_Settings->GetValue("Game extract path");
    QString outputPath;
    if(moddedOutput == ""){
        outputPath = m_zlManager->m_Settings->GetValue("Modded game path");
    } else {
        outputPath = moddedOutput;
    }

    qDebug() << Q_FUNC_INFO << "Loading 7zip from:" << sevenZipPath;
    qDebug() << Q_FUNC_INFO <<"Clean directory:" << inputPath;
    qDebug() << Q_FUNC_INFO <<"Targetting directory:" << outputPath;

    const static QStringList buildArchivesDeflate3 = {"TFA2", "TA_XTRAS"};
    const static QStringList buildArchivesDeflate1 = {"TFA"};
    const static QStringList buildArchivesStore = {"SOUNDE"};
    QStringList archiveList = {};

    archiveList = m_zlManager->ZippedFolders(true);

    QProcess *sevenZip = new QProcess();
    QObject::connect(sevenZip, &QProcess::readyReadStandardOutput, [this,sevenZip, &removeFiles]() {
        //below only works if python output is flushed.
        QString debugOutput = sevenZip->readAllStandardOutput();
        QString errorOutput = sevenZip->readAllStandardError();
        qDebug() << "normal" << debugOutput;
        qDebug() << "error" << errorOutput;
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
        case 3:
            //ISOManager is unable to build a bootable ISO. Call ImgBurn
            m_Debug->Log("ISOManager is unable to make a bootable ISO. Calling ImgBurn.");
            repackISO(removeFiles);
        default:
            //either nothing to worry about or something we haven't handled yet. output it.
            qDebug() << "Unhandled output code:" << debugOutput;
            break;
        }
    });
    m_Debug->Log("Starting 7Zip to rezip archives.");

    for(int i = 0; i < archiveList.size(); i++){
        /*The zips resulting from this are still way smaller than the original - this is probably contributing to the long load times. See if anything can be done about that.
        https://7-zip.opensource.jp/chm/cmdline/switches/method.htm*/
        /*Check output path for files, add those files to both the output list and the existing files list*/
        QDir outputDirectory(outputPath + "\\" + archiveList[i]);
        QDirIterator outputIterator(outputPath + "\\" + archiveList[i], QDirIterator::Subdirectories);
        QStringList existingFiles;
        QString zipFileOut = outputPath + "\\tempzip.txt";
        QString zipFileExclude = QCoreApplication::applicationDirPath() + "/excludezip.txt";
        QFileInfo excludeFile(zipFileExclude);
        if(!excludeFile.isFile()){
            m_Debug->MessageError("excludezip.txt was not found. The rebuild will have the BDB files. If this is a Randomizer build, that will break the randomization.");
        }
        QFile zipTextFile(zipFileOut);
        zipTextFile.open(QIODevice::WriteOnly);
        while(outputIterator.hasNext()){
            QFileInfo current(outputIterator.next());
            QString checkFile = outputDirectory.relativeFilePath(current.absoluteFilePath());
            if(existingFiles.contains(checkFile) || !current.isFile()){
                continue;
            }
            if(checkFile != ".." && checkFile != "." && checkFile != "../"){
                existingFiles.push_back(checkFile);
                zipTextFile.write(checkFile.toUtf8());
                zipTextFile.write("\n");
            }
        }
        existingFiles.removeDuplicates();
        qDebug() << Q_FUNC_INFO << "existing files:" << existingFiles;
        zipTextFile.close();

        sevenZip->setWorkingDirectory(outputPath + "/" + archiveList[i]);

        QStringList args;
        args.append("a");
        args.append("..\\" + archiveList[i] + ".zip");
        //add all files to a file list text file https://7zip.bugaco.com/7zip/MANUAL/cmdline/syntax.htm
        args.append("@..\\tempzip.txt");
        if(buildArchivesStore.contains(archiveList[i])){
            qDebug() << Q_FUNC_INFO << "creating zip for" << archiveList[i] << "with compression level 0";
            args.append("-mx0");
        } else if (buildArchivesDeflate1.contains(archiveList[i])){
            qDebug() << Q_FUNC_INFO << "creating zip for" << archiveList[i] << "with compression level 1";
            args.append("-mx1");
        } else {
            qDebug() << Q_FUNC_INFO << "creating zip for" << archiveList[i] << "with compression level 3";
            args.append("-mx3");
        }
        /*the exclude shouldn't be necessary here, only on the other one.*/
        //args.append("x");
        //args.append("@" + zipFileExclude);
        args.append("-sdel");
        qDebug() << Q_FUNC_INFO << "Here we would start 7zip with the commands:" << args;
        sevenZip->start(sevenZipPath, args);
        sevenZip->waitForFinished();
        zipTextFile.remove();


        QDir inputDirectory(inputPath + "\\" + archiveList[i]);
        QDirIterator inputIterator(inputPath + "\\" + archiveList[i], QDirIterator::Subdirectories);
        zipFileOut = inputPath + "\\tempzip.txt";
        zipTextFile.setFileName(zipFileOut);
        zipTextFile.open(QIODevice::WriteOnly);
        while(inputIterator.hasNext()){
            QFileInfo current(inputIterator.next());
            QString checkFile = inputDirectory.relativeFilePath(current.absoluteFilePath());
            if(existingFiles.contains(checkFile) || !current.isFile()){
                continue;
            }
            if(checkFile != ".." && checkFile != "." && checkFile != "../"){
                existingFiles.push_back(checkFile);
                zipTextFile.write(checkFile.toUtf8());
                zipTextFile.write("\n");
            }
        }
        zipTextFile.close();

        sevenZip->setWorkingDirectory(inputPath + "/" + archiveList[i]);

        args.clear();
        args.append("a");
        args.append(outputPath + "/" + archiveList[i] + ".zip");
        args.append("@..\\tempzip.txt");
        if(buildArchivesStore.contains(archiveList[i])){
            args.append("-mx0");
        } else if (buildArchivesDeflate1.contains(archiveList[i])){
            args.append("-mx1");
        } else {
            args.append("-mx3");
        }
        if(excludeFile.isFile()){
            args.append("-x@" + zipFileExclude);
        }
        qDebug() << Q_FUNC_INFO << "Here we would start 7zip with the commands:" << args;
        sevenZip->start(sevenZipPath, args);
        sevenZip->waitForFinished();
        zipTextFile.remove();
    }

    /*for(int i = 0; i < buildArchivesStore.size(); i++){
        QStringList args;
        args.append("a");
        //name and path of destination - [archive].zip
        //folder of source = folder/*
        args.append(outputPath + "\\" + buildArchivesStore[i] + ".zip");
        args.append(inputPath + "\\" + buildArchivesStore[i] + "\\*");
        args.append("-m0=Copy");
        //args.append("-sdel");
        qDebug() << Q_FUNC_INFO << "Here we would start 7zip with the commands:" << args;
        //sevenZip->start(sevenZipPath, args);
        sevenZip->waitForFinished();
    }*/
    m_Debug->Log("All archives zipped.");
    repackISO(removeFiles);

    return 0;
}

int IsoBuilder::repackISO(bool removeFiles){
    //calls imgburn to rebuild a new ISO
    static QStringList elfVersions = {"SLUS_206.68","SLES_523.88","SLES_533.09","SLKA_251.75"};
    int currentELF = 0;
    m_zlManager->m_Settings->LoadSettings();
    QString imgBurnPath = m_zlManager->m_Settings->GetValue("ImgBurn EXE path");
    if(imgBurnPath == ""){
        //prompt user for imgBurn location
        m_Debug->Log("ImgBurn executable path is not set. Please locate ImgBurn.exe.");
        imgBurnPath = QFileDialog::getOpenFileName(m_UI, m_UI->tr("Locate ImgBurn EXE"), QDir::currentPath(), m_UI->tr("*.exe"));
        if(imgBurnPath == ""){
            m_Debug->MessageError("ImgBurn was not located. Build process cancelled.");
            return 0;
        }
        QDir dirCorrection(imgBurnPath);
        m_zlManager->m_Settings->SetValue("ImgBurn EXE path", dirCorrection.absolutePath());
    }

    QString outputPath;
    if(moddedOutput == ""){
        outputPath = m_zlManager->m_Settings->GetValue("Modded game path");
    } else {
        outputPath = moddedOutput;
    }
    QString inputPath = m_zlManager->m_Settings->GetValue("Game extract path");
    QDir outputDir(outputPath);
    qDebug() << Q_FUNC_INFO << "testing path:" << outputDir.absolutePath() << "vs" << outputPath;
    outputPath = outputDir.absolutePath();
    outputPath.replace("/", "\\");
    QString elfPath;
    for(int i = 0; i < elfVersions.size(); i++){
        elfPath = outputPath + "\\" + elfVersions[i];
        QFileInfo elfFile(elfPath);
        qDebug() << Q_FUNC_INFO << "checking if ELF file" << elfPath << "exists:" << elfFile.isFile();
        if(elfFile.isFile()){
            currentELF = i;
        }
    }
    outputDir.cdUp();
    QString targetELF = elfVersions[currentELF];
    QString buildName;
    if(buildingRandomizer){
        buildName = " Randomizer";
    } else {
        buildName = " Rebuild";
    }
    QString outputISO = outputDir.absolutePath() + "/Transformers " + targetELF.remove(".") + buildName + ".iso";

    //there's probably a better way to do this but I don't feel like researching at the moment
    QString slashType;
    if(outputPath.contains("/")){
        slashType = "/";
    } else {
        slashType = "\\";
    }

    qDebug() << Q_FUNC_INFO << "Loading imgBurn from:" << imgBurnPath;
    qDebug() << Q_FUNC_INFO <<"Targetting directory:" << outputPath;
    qDebug() << Q_FUNC_INFO << "creating output" << outputISO;

    QString fileDeleteLog = "";

    QStringList args;
    QString fileListArg;
    fileListArg = outputPath + slashType + "SYSTEM.CNF";
    //m_Debug->Log("adding file or path to ISO build list: " + inputPath + slashType + "SYSTEM.CNF");
    QString verification = "|" + outputPath + slashType + elfVersions[currentELF];
    qDebug() << Q_FUNC_INFO << "verifying ELF:" << verification;
    fileListArg += "|" + outputPath + slashType + elfVersions[currentELF];
    fileListArg += "|" + outputPath + slashType + "TA_XTRAS.ZIP";
    fileListArg += "|" + outputPath + slashType + "TFA.ZIP";
    fileListArg += "|" + outputPath + slashType + "TFA2.ZIP";
    fileListArg += "|" + outputPath + slashType + "SOUNDE.ZIP";
    fileListArg += "|" + outputPath + slashType + "IOP" + slashType;
    fileListArg += "|" + outputPath + slashType + "FMV" + slashType;
    fileListArg += "|" + outputPath + slashType + "SOUND" + slashType;

    /*This can be written to a text file just like with 7zip (see readme file, SRCLIST).*/
    //QStringList unpackedfolders = {"FMV", "IOP", "SOUND"};
    QStringList uneditedFolders = m_zlManager->UnzippedFolders(false);
    QStringList editedFolders = m_zlManager->UnzippedFolders(true);
    QStringList uneditedZips = m_zlManager->ZippedFolders(false);
    QStringList editedZips = m_zlManager->ZippedFolders(true);

    QString isoTextPath = outputPath + "\\tempiso.txt";
    QFile isoTextFile(isoTextPath);
    isoTextFile.open(QIODevice::WriteOnly);
    for(int i = 0; i < uneditedFolders.size(); i++){
        isoTextFile.write(inputPath.toUtf8() + slashType.toUtf8() + uneditedFolders[i].toUtf8());
        isoTextFile.write("\n");
    }
    for(int i = 0; i < editedFolders.size(); i++){
        isoTextFile.write(outputPath.toUtf8() + slashType.toUtf8() + editedFolders[i].toUtf8());
        isoTextFile.write("\n");
    }
    isoTextFile.write(inputPath.toUtf8() + slashType.toUtf8() + "SYSTEM.CNF");
    isoTextFile.write("\n");
    /*checking to see if we made a modified ELF file*/
    QFileInfo checkELF(outputPath + slashType + elfVersions[currentELF]);
    if(checkELF.exists() && checkELF.isFile()){
        isoTextFile.write(outputPath.toUtf8() + slashType.toUtf8() + elfVersions[currentELF].toUtf8());
    } else {
        isoTextFile.write(inputPath.toUtf8() + slashType.toUtf8() + elfVersions[currentELF].toUtf8());
    }
    isoTextFile.write("\n");
    for(int i = 0; i < uneditedZips.size(); i++){
        isoTextFile.write(inputPath.toUtf8() + slashType.toUtf8() + uneditedZips[i].toUtf8() + ".zip");
        isoTextFile.write("\n");
    }
    for(int i = 0; i < editedZips.size(); i++){
        isoTextFile.write(outputPath.toUtf8() + slashType.toUtf8() + editedZips[i].toUtf8() + ".zip");
        isoTextFile.write("\n");
    }
    isoTextFile.close();


    args.append("/MODE");
    args.append("BUILD");
    args.append("/OUTPUTMODE");
    args.append("IMAGEFILE");
    args.append("/SRCLIST");
    args.append(isoTextPath);
    args.append("/DEST");
    args.append(outputISO);
    args.append("/FILESYSTEM");
    args.append("ISO9660 + UDF");
    args.append("/UDFREVISION");
    args.append("\"1.02\"");
    args.append("/VOLUMELABEL");
    args.append(elfVersions[currentELF].remove("."));
    if(removeFiles){
        //fileDeleteLog = " Deleting loose files once complete.";
        //args.append("/DELETESOURCE");
    }
    args.append("/START");


    /*ImgBurn args below were taken from my template used for manual rebuilds. These seem to make broken ISOs when done through here. TODO: isolate the specific problem args*/
    /*args.append("/MODE");
    args.append("BUILD");
    args.append("/INPUTMODE");
    args.append("2");
    args.append("/OUTPUTMODE");
    args.append("2");
    args.append("/SRC");
    args.append(fileListArg);
    args.append("/DEST");
    args.append(outputPath);
    args.append("/FILESYSTEM");
    args.append("3");
    args.append("/UDFREVISION");
    args.append("0");
    args.append("/VOLUMELABEL_ISO9660");
    args.append(elfVersions[currentELF].remove("."));
    args.append("/VOLUMELABEL_UDF");
    args.append(elfVersions[currentELF].remove("."));
    if(removeFiles){
        fileDeleteLog = " Deleting loose files once complete.";
        args.append("/DELETESOURCE");
    }
    args.append("/START");*/

    qDebug() << Q_FUNC_INFO << "args:" << args;

    QProcess *imgBurn = new QProcess();
    QObject::connect(imgBurn, &QProcess::readyReadStandardOutput, [imgBurn]() {
        QString debugOutput = imgBurn->readAllStandardOutput();
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

    QObject::connect(imgBurn, &QProcess::finished, [outputPath](){
        qDebug() << Q_FUNC_INFO << "Cleaning up files.";
        QDir outputDirectory(outputPath);
        QDirIterator outputIteratorFiles(outputPath, QDirIterator::Subdirectories);
        while(outputIteratorFiles.hasNext()){
            QFileInfo current(outputIteratorFiles.next());
            QString checkFile = outputDirectory.relativeFilePath(current.absoluteFilePath());
            if(checkFile == ".." && checkFile == "." && checkFile == "../"){
                continue;
            }
            if(checkFile == "Spoilers.txt"){
                continue;
            }
            if(!current.isDir()){
                QFile fileToRemove(current.absoluteFilePath());
                fileToRemove.moveToTrash();
            }
        }
        QDirIterator outputIteratorPaths(outputPath, QDirIterator::Subdirectories);
        while(outputIteratorPaths.hasNext()){
            QFileInfo current(outputIteratorPaths.next());
            QString checkFolder = outputDirectory.relativeFilePath(current.absoluteFilePath());
            if(checkFolder == ".." && checkFolder == "." && checkFolder == "../"){
                continue;
            }
            if(current.isDir()){
                QDir dirToRemove(current.absolutePath());
                qDebug() << Q_FUNC_INFO << "Removing directory at:" << current.absolutePath();
                dirToRemove.rmdir(current.absolutePath());
            }
        }

    });
    m_Debug->Log("Building ISO." + fileDeleteLog);
    /*The above needs to be tested again to make sure the correct files get deleted please & thank you*/
    imgBurn->start(imgBurnPath, args);
    return 0;
}

void IsoBuilder::copyFiles(QString folderName){
    QDir inputDir(moddedInput);
    inputDir.setPath(moddedInput + "/" + folderName);
    int totalFileCount = getFileCount(inputDir);
    int fileCount = 0;

    qDebug() << Q_FUNC_INFO << "Total files:" << totalFileCount;

    QDirIterator currentFile(inputDir, QDirIterator::Subdirectories);
    int sourcePathLength = inputDir.absoluteFilePath(moddedInput).length();

    while(currentFile.hasNext()){
        currentFile.next();
        QFileInfo currentFileInfo = currentFile.fileInfo();
        if(!currentFileInfo.isHidden()){
            QString subPathStructure = currentFileInfo.absoluteFilePath().mid(sourcePathLength);
            QString outputPath = moddedOutput + subPathStructure;
            if(currentFileInfo.isDir()){
                qDebug() << Q_FUNC_INFO << "creating directory:" << outputPath << "from" << moddedOutput << "and" << subPathStructure;
                inputDir.mkpath(outputPath);
                fileCount++;
            } else if (currentFileInfo.isFile()){
                qDebug() << Q_FUNC_INFO << "copying file" << fileCount << "/" << totalFileCount << ":" << currentFileInfo.fileName();
                QFile::copy(currentFileInfo.absoluteFilePath(), outputPath);
                fileCount++;
            }
            m_UI->UpdateLoadingBar(fileCount, totalFileCount);
        }
    }
}

int IsoBuilder::packModded(QString outputFolder){
    if(outputFolder == "Randomizer"){
        buildingRandomizer = true;
    }
    moddedInput = m_zlManager->m_Settings->GetValue("Modded game path");
    bool manualDirectory = false;
    if(moddedInput == ""){
        moddedInput = QFileDialog::getExistingDirectory(m_UI, m_UI->tr("Choose un-modded source."), QDir::currentPath());
        manualDirectory = true;
    }
    QDir inputDir(moddedInput);
    if(moddedOutput == ""){
        QDir findModded(moddedInput);
        findModded.cdUp();
        moddedOutput = findModded.absolutePath() + "/" + outputFolder;
    }
    if(moddedInput == moddedOutput){
        //randomizer directory is already the modded game directory - we need the game path instead
        moddedInput = m_zlManager->m_Settings->GetValue("Game extract path");
        inputDir.setPath(moddedInput);
    }
    QMessageBox confirmPath;

    confirmPath.setText("Is the input (non-modified) path correct?");
    confirmPath.setInformativeText("Path: " + moddedInput);
    confirmPath.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

    int confirmation = 0;
    if(!manualDirectory){
        confirmation = confirmPath.exec();
    }
    if(confirmation == QMessageBox::No){
        moddedInput = QFileDialog::getExistingDirectory(m_UI, m_UI->tr("Choose un-modded source."), QDir::currentPath());
    }
    if(moddedInput == ""){
        m_Debug->Log("Process cancelled.");
    } else {
        m_zlManager->m_Settings->SetValue("Game extract path", moddedInput);
    }

    confirmPath.setText("Is the modified path correct?");
    confirmPath.setInformativeText("Path: " + moddedOutput);

    confirmation = confirmPath.exec();
    if(confirmation == QMessageBox::No){
        moddedOutput = QFileDialog::getExistingDirectory(m_UI, m_UI->tr("Choose modded source."), QDir::currentPath());
    }
    if(moddedOutput == ""){
        m_Debug->Log("Process cancelled.");
    }

    qDebug() << Q_FUNC_INFO << "input path read as" << moddedInput;
    qDebug() << Q_FUNC_INFO << "Path confirmed?" << confirmation;

    m_Debug->Log("Copying files from " + moddedInput + " to " + moddedOutput);
    m_Debug->Log("Copying game files for modified build. (This can take a couple minutes)");
    //QStringList foldersToCopy = {"FMV", "IOP", "SOUND"};
    QStringList foldersToCopy = m_zlManager->UnzippedFolders(true);

    for(int i = 0; i < foldersToCopy.size(); i++){
        copyFiles(foldersToCopy[i]);
    }

    //parent->updateLoadingBar(totalFileCount, totalFileCount);

    QDir dirCorrection(moddedOutput);
    m_zlManager->m_Settings->SetValue("Modded game path", dirCorrection.absolutePath());
    rezipTFA_sevenZip(true);

    moddedInput = "";
    moddedOutput = "";
    buildingRandomizer = false;

    return 0;
}

int IsoBuilder::getFileCount(QDir dirToCount){
    int fileCount = 0;
    QDirIterator fileCounter(dirToCount, QDirIterator::Subdirectories);
    while(fileCounter.hasNext()){
        fileCounter.next();
        fileCount++;
    }
    qDebug() << Q_FUNC_INFO << "directory" << dirToCount.absolutePath() << "has" << fileCount << "files";
    return fileCount;
}
