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

    QString m_copyOutputPath;
    std::vector<std::shared_ptr<taFile>> m_loadedFiles;
    std::vector<std::shared_ptr<DatabaseFile>> m_databaseList;
    QStringList m_loadedFileNames;
    QListWidget* m_fileBrowser;
    QString m_gamePath = "";
    bool m_bulkLoading = false;

    QString m_fileMode;

    FileData m_fileData;

    void AddMenuItems();
    void ClearFiles();

    std::shared_ptr<taFile> MatchFile(QString fileNameFull);

    QStringList ZippedFolders(bool modded = false);
    QStringList UnzippedFolders(bool modded = false);

    void OpenWarpgateCalculator();
    void OpenModHandler();
    void OpenRandomizer();
    void SaveFile(QString fromType, QString givenPath = "");
    void BulkSave(QString category);

    void BulkOpen(QString fileType);
    void LoadRequiredFile(taFile* fromFile, QString needFile, QString fileType);
    template <typename theFile>
    void LoadBulkFile(theFile fileToOpen);

    int LoadDatabases();
    int CreateDefinitionFile();
    int CreateDatabaseFile();

    template <typename theFile>
    void LoadFile(theFile fileToOpen, QString givenPath = "");
    void OpenFile(QString fileType, QString givenPath = "");

    /*Visitor functions for adding files of a specific type to their respective lists.
     *For now, that's only database files. I would like to find another way if one exists.
     *Something about this setup doesn't feel correct.*/
    void Visit(taFile dataFile);
    void Visit(DatabaseFile dataFile);
};

#endif // ZEBRAFISH_H
