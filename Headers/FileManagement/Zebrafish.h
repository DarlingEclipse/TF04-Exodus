#ifndef ZEBRAFISH_H
#define ZEBRAFISH_H

#include <QListWidget>

#include "Headers/Main/BinChanger.h"

class DatabaseFile;
class exWindowBase;
class IsoBuilder;
class ModHandler;
class exSettings;
class taFile;

/*2/28/2025 - isolated from mainwindow.h as part of The Great Refactoring (Part 1)
 * This is the main file manager - named after what is believied to be Melbourne House's file manager, Fish.
 * This will be the main interface between the various file classes and systems and the OS file system
 * Still needs some work, but this is a start*/

class zlManager
{
public:
    zlManager(exWindowBase *passUI, exSettings *passSettings);

    exWindowBase* m_UI;
    exSettings* m_Settings;
    exDebugger* m_Debug;

    IsoBuilder* m_IsoBuilder;
    ModHandler* m_ModHandler;

    QString copyOutputPath;
    std::vector<std::shared_ptr<taFile>> loadedFiles;
    std::vector<std::shared_ptr<DatabaseFile>> databaseList;
    QStringList loadedFileNames;
    QListWidget* fileBrowser;
    QString gamePath = "";
    bool bulkLoading = false;

    QString fileMode;

    FileData fileData;

    void clearFiles();

    std::shared_ptr<taFile> matchFile(QString fileNameFull);

    QStringList ZippedFolders(bool modded = false);
    QStringList UnzippedFolders(bool modded = false);

    void openWarpgateCalculator();
    void openModHandler();
    void openRandomizer();
    void saveFile(QString fromType, QString givenPath = "");
    void bulkSave(QString category);

    void bulkOpen(QString fileType);
    void loadRequiredFile(taFile* fromFile, QString needFile, QString fileType);
    template <typename theFile>
    void loadBulkFile(theFile fileToOpen);

    int loadDatabases();
    int createDefinitionFile();
    int createDatabaseFile();

    template <typename theFile>
    void loadFile(theFile fileToOpen, QString givenPath = "");
    void openFile(QString fileType, QString givenPath = "");

    /*Visitor functions for adding files of a specific type to their respective lists.
     *For now, that's only database files. I would like to find another way if one exists.
     *Something about this setup doesn't feel correct.*/
    void visit(taFile dataFile);
    void visit(DatabaseFile dataFile);
};

#endif // ZEBRAFISH_H
