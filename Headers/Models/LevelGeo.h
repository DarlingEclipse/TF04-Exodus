#ifndef LEVELGEO_H
#define LEVELGEO_H

#include <QVector3D>
#include <QColor>
#include "Headers/Main/BinChanger.h"
#include "Headers/FileManagement/taFileSystemObject.h"

class ProgWindow;
class MeshVBIN;
class VBIN;
class FileData;
class TriangleStrip;

class GeometrySet{
public:
    SectionHeader headerData;
    std::vector<QVector3D> geoSetVerticies;
    std::vector<TriangleStrip> indexArray;
    std::vector<QColor> geoSetColors;
    std::vector<QVector2D> geoSetTexCoords;
    std::vector<QVector3D> geoSetNormals;
    int geoSetID;
    int version;
    int unknownValue1;
    int vertexAttributeGroup;
    int vertexFormat;
    int unknownValue2;
    int unknownValue3;
    int unknownValue4;
    int unknownValue5;
    int unknownValue6;
    int unknownValue7;
    int unknownValue8;
    int unknownValue9;
    int unknownValue10;
    int unknownValue11;
    int unknownValue12;

    uint32_t indexList;
    QVector3D position1;
    QVector3D position2;
    MeshVBIN *file;
    FileData *fileData;
    QString textureName;

    void getVerticies();
    void writeDataSTL(QTextStream &fileOut);
    void writeDataDAE(QTextStream &fileOut);
    void writeNodesDAE(QTextStream &fileOut);
    void writeEffectsDAE(QTextStream &fileOut);
    void writeImagesDAE(QTextStream &fileOut);
    void writeMaterialsDAE(QTextStream &fileOut);
};

class MeshVBIN : public taFile {
  public:
    const QStringList validOutputs(){
        return QStringList{"STL", "DAE"};
    };
    virtual const QString fileCategory(){
        return "Model";
    };
    std::vector<GeometrySet> geoSets;

    int readData();
    int outputDataSTL();
    int outputDataDAE();

    void load(QString fromType);
    void save(QString toType);
};

#endif // LEVELGEO_H
