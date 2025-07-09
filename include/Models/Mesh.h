#ifndef MESH_H
#define MESH_H

#include <QQuaternion>
#include "Utility/BinChanger.h"


class VBIN;
class Mesh;

class Triangle{
  public:
    QVector3D vertex1;
    QVector3D vertex2;
    QVector3D vertex3;
};

class TriangleStrip{
public:
    std::vector<int> stripIndecies;
};

class NormalArray{
public:
  int arrayID;
  long arrayLength;
  QString meshName;
  FileData *fileData;
  std::vector<QVector3D> positionList;
};

class ColorArray{
public:
  int arrayID;
  long arrayLength;
  QString meshName;
  FileData *fileData;
  std::vector<QColor> positionList;
};

class TextureCoords{
public:
  int arrayID;
  long arrayLength;
  QString meshName;
  FileData *fileData;
  std::vector<QVector2D> positionList;
};

class LODInfo{
public:
    long fileLocation;
    int levels;
    std::vector<std::vector<int>> targetIndecies;
    std::vector<float> levelDistance;
    FileData *fileData;

    void populateLevels();
    void clear();
};

class IndexArray{
public:
    int arrayID;
    int arrayLength;
    long fileLocation;
    int triangleCount;
    std::vector<TriangleStrip> triangleStrips;
    std::vector<int> indexList;
    FileData *fileData;

    void populateTriangleStrips();
};

class MeshFaceSet{
public:
    int version;
    int indexCount; //pretty sure this is index count or triangle strip count
    int primitiveType;
    IndexArray indexArray;
};

class PositionArray{
public:
  int arrayID;
  int vertexCount;
  long fileLocation;
  QString meshName;
  std::vector<QVector3D> positionList;
  FileData *fileData;

  //void Transform();
  void clear();
  const void operator=(PositionArray input);
};

class VertexSet{
public:
    void createArrays();

    int vertexCount;
    PositionArray positionArray;
    NormalArray normalArray;
    ColorArray colorArray;
    TextureCoords textureCoords;
    FileData *fileData;

    void getArrayLocations();
};

class Material{
public:
    int sectionLength;
    int version;
    int nameLength;
    QString name;
    QVector4D diffuse;
    QVector4D specular;
    QVector4D ambient;
    float specularPower;

    void debug();
};

class RenderStateGroup{
public:
    int version;
    int pixelBlend;
    int textureBlend;
    int textureClamp;
    int textureFlip;
    int textureFilter;
    int cullMode;
};

class SurfaceProperties{
public:
    QString textureName;
    QString texture2Name;
    int version;
    Material material;
    RenderStateGroup renderStateGroup1;
    RenderStateGroup renderStateGroup2;
};

class Element{
public:
    MeshFaceSet meshFaceSet;
    SurfaceProperties surfaceProperties;
    int version;
    int attributes;
};

class ElementArray{
public:
    std::vector<Element> elementArray;
    LODInfo lodInfo;
};

class BoundingVolume{
public:
    SectionHeader headerData;
    int version;
    bool hasVolume;
    int type;
    QVector3D center;
    float radius;
    FileData *fileData;

    void populateData();
    const void operator=(BoundingVolume input);
};

class Modifications{
public:
    int modByte;
    QVector3D offset;
    QQuaternion rotation;
    float scale;

    void clear();
};

class FileSection{
public:
    VBIN *file;
    FileData *fileData;
    FileSection *parent;
    SectionHeader headerData;
    long sectionEnd;
    BoundingVolume boundVol;

    Modifications mods;

    QStringList sectionTypes;
    std::vector<FileSection*> sectionList;
    std::vector<Mesh*> meshList;

    void getSceneNodeTree(long searchStart, long searchEnd, int depth);
    void readNode();
    void sendKeyframe(QVector3D keyOffset, QString channelName);
    void sendKeyframe(QQuaternion keyRotation, QString channelName);
    void readModifications();
    //void getModifications();
    //void modifyPosArrays(std::vector<Modifications> addedMods);
    void printInfo(int depth); // for debugging
    const void operator=(FileSection input);
    void writeSectionListSTL(QTextStream &file);
    void writeSectionListSTL(QString path);
    bool meshListContains(QString checkName);
    void modify(std::vector<Modifications> addedMods);
    // void modifyPosArrays(); //this might go somewhere else, just
    // commenting out for now

    void searchListsWriteDAE(QTextStream &fileOut, void (Mesh::*)(QTextStream&));
    void writeNodes(QTextStream &fileOut);
    virtual void writeNodesDAE(QTextStream &fileOut);

    //template<typename WriteType>
    //void searchListsWriteDAE(QString path, WriteType *write);

    void clear();
};

class Mesh : public FileSection{
public:
    BoundingVolume boundVol;
    VertexSet vertexSet;
    ElementArray elementArray;
    int elementCount;
    int elementOffset;

    void clear();
    void applyKeyframe(QVector3D keyOffset);
    void applyKeyframe(QQuaternion keyRotation);
    int readMesh();
    //void modifyPosArrays(Modifications mods);
    void getModifications();
    void writeDataSTL(QTextStream &fileOut);
    std::vector<int> getChosenElements();
    void modify(std::vector<Modifications> addedMods);
    void writeNodesDAE(QTextStream &fileOut);
    void writeDataDAE(QTextStream &fileOut);
    void writeEffectsDAE(QTextStream &fileOut);
    void writeMaterialsDAE(QTextStream &fileOut);
    void writeImagesDAE(QTextStream &fileOut);

    //template<typename Item>
    //void searchSomethingListDAE(QTextStream &fileOut, Item write);

    const void operator=(Mesh input);
};



#endif // MESH_H
