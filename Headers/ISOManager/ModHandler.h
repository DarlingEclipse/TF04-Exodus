#ifndef MODHANDLER_H
#define MODHANDLER_H

#include "Headers/Databases/DataHandler.h"
#include <QCheckBox>

class ProgWindow;


class RandomizerMod : public ExodusOption{
public:
    bool type; //0 for binary, 1 for text
    QString fileName;
};

class FileReplacement : public ExodusOption{
public:
    int rarity;
    QStringList fileNames;
    QStringList fileDestinations;
    QStringList affectedFolders;
};

class FolderOption{
public:
    QString folderName;
    bool moddedSource; //0 for unmodded
    bool zipped; //0 for uncompressed
    QCheckBox *checkOption;
};

class ModHandler{
public:
    ProgWindow* parent;
    std::vector<FileReplacement> replacementList;
    std::vector<RandomizerMod> modList;
    std::vector<FolderOption> folderOptions;

    ModHandler(ProgWindow* passParent);
    ModHandler();

    void updateCenter();
    void setChanges();
    void loadMods();
    void loadFileReplacements();
    void applyModifications();
    void replaceFile(QString fileName, QString destinationPath);
    void replaceFile(FileReplacement fileToReplace);
    void setModdedFolders();
    bool checkNeededFolder(QString folderName);
};

#endif // MODHANDLER_H
