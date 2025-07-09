#ifndef TEXTDATABASE_H
#define TEXTDATABASE_H

#include <QString>

/*TFA2\CREATURE.TMD for testing
file is in sections and subsections, each contained in brackets and preceded by a ~[NAME]
test file has main sections IncludedFiles and Dictionary

*/

class IncludedFiles{
public:
    QStringList *paths;
};

class DictItem{
public:
    QStringList *properties;
};

class Dictionary{
public:
    std::vector<DictItem> dictItems;
};

class DBFile{
public:
    QString path;
    IncludedFiles dbFiles;
    Dictionary dbDictionary;
};

#endif // TEXTDATABASE_H
