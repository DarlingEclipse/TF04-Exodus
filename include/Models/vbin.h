#ifndef VBIN_H
#define VBIN_H

#include <QVector>
#include <QByteArrayMatcher>
#include <QTransform>
#include <QBuffer>

#include "Antioch2.h"
#include "FileManagement/taFileSystemObject.h"
#include "Models/Mesh.h"

//just change over to this.
//https://doc.qt.io/qt-5/qt3drender-qattribute.html

//https://forum.qt.io/topic/56386/load-3d-model-with-qt3d-qt5-5/11

class ProgWindow;

class MeshVBIN;
class SectionHeader;

//something like this can be used to improve Normal, Color, and Position arrays
template <typename T> class binArray {
public:
    int arrayID;
    long arrayLength;
    Mesh *parentMesh;
    QVector3D valueList[];
};

class Portal{
  public:
    int dataLength; //appears to always be 92
    int possibleVersion; //appears to alway be 0
    std::vector<int> references;
    QVector3D unknownPoint;
    std::vector<QVector3D> pointList;
    float unknownValue;
};

class vlLodSwitcher : public FileSection{

};

class Instance : public FileSection{
  public:
    int unknownValue;
    QString modelReference;
    void writeNodesDAE(QTextStream &fileOut);
};

class Cell {
  public:
    int dataLength;
    int possibleVersion;
    int unknownShort;
    std::vector<int> portals;
    std::vector<int> excludedCells;
    std::vector<QVector3D> axisBounds; //AABB, appears to always be 2 values
    std::vector<QVector3D> orientationBounds; //OBB, appears to always be 5 values
};

class CellManager : public FileSection{
  public:
    int unknownValue1;
    int portalCount; //might be portal count
    std::vector<Portal*> portalList;
    std::vector<Cell*> cellList;

    void readPortals();
    void readCell();
    void outputPortals();
};

class SceneNode : public FileSection{
public:
    void modify(std::vector<Modifications> addedMods);
    void writeDataSTL(QTextStream &fileOut);
    void writeDataDAE(QTextStream &fileOut);
    void writeSceneListDAE(QTextStream &FileOut);
    //void getModifications(); //inherited version should be workable
    //void modifyPosArrays();
    void clear();
};

class AnimationPrototype{
public:
    VBIN *file;
    QString name;
    long sectionLength;
    long sectionEnd;

    int version1;
    int version2;
    int neutralDataCount;

    int version3;

    Modifications mods;

    int detailLevel;
    float visibilityFactorNear;
    float visibilityFactorFar;
    int attributes;

    BoundingVolume boundingVolume1;
    BoundingVolume boundingVolume2;
};

class VBIN : public taFile {
public:
    const QStringList knownSections = {"SceneNode", "Mesh", "anAnimationPrototype", "BoundingVolume", "vlLODSwitcher", "anAnimationSourceSet", "LevelMasks", "Instance","vlCellManager"
                                      "CellManager", "Portals", "vlCell", "Cell"};
    const QStringList validOutputs(){
        return QStringList{"STL", "DAE"};
    };
    virtual const QString fileCategory(){
        return "Model";
    };

    static inline QString version = "1.0";
    AnimationSourceSet animationSet;
    int highestLOD;
    long currentLocation;
    FileSection base;
    int selectedLOD;
    bool singleOutput;
    bool isSplitFile;
    std::shared_ptr<MeshVBIN> meshFile;

    QStringList textureNameList;
    QStringList instanceNameList;
    int meshCount;

    VBIN();
    std::vector<int> getIndexArrays(int posCount, int chosenLOD, int location);
    void makeTriangles(std::vector<int> indArrays, int whichArray, int location, QTextStream *stream);
    void applyKeyframe();
    void outputDataSTL();
    void outputDataDAE();

    void writeEffectListDAE(QTextStream &stream);
    void writeImageListDAE(QTextStream &stream);
    void writeMaterialListDAE(QTextStream &stream);
    void writeGeometryListDAE(QTextStream &stream);
    void writeAnimationListDAE(QTextStream &stream);
    void writeNodeListDAE(QTextStream &stream);

    void save(QString toType);
    void save(QString toType, QTextStream &stream);
    void load(QString fromType);
    void updateCenter();
    void setOutput(bool single);
    void setLevel(int level);
private:
    //void modifyPosArrays();
    int readDataVBIN();
    void outputPositionList(int location);
    void readAnimationPrototype();
    void readBoundingVolume(SectionHeader* signature);
    int getSceneNodeTree();
    void readModifications();
    void analyzeTriangles(std::vector<int> indArrays, int whichArray);
    QVector3D needOffset(int offsetLocation);
    QQuaternion getRotation(int offsetLocation);
    float getScale(int offsetLocation);
};

#endif // VBIN_H
