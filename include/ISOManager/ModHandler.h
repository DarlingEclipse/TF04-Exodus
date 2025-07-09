#ifndef MODHANDLER_H
#define MODHANDLER_H

/*The only thing we need from datahandler is ExodusOption, this could be rearranged a bit better*/
#include "Databases/DataHandler.h"
#include <QCheckBox>

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

class ModHandler{
public:
    exWindowBase* m_UI;
    zlManager* m_zlManager;
    exDebugger* m_Debug;
    std::vector<FileReplacement> replacementList;
    std::vector<RandomizerMod> modList;
    std::vector<taFolder> folderOptions;

    ModHandler(exWindowBase *passUI, zlManager *fileManager);

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
