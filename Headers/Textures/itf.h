#ifndef ITF_H
#define ITF_H

#include <QMatrix4x4>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QPainter>
#include <QImageReader>
#include <QImageWriter>
#include <QFile>

#include <vector>

#include "Headers/FileManagement/taFileSystemObject.h"

class Color{
public:
    uint32_t redInt;
    uint32_t greenInt;
    uint32_t blueInt;
    uint32_t alphaInt;
    uint32_t paletteIndex;

    const void operator=(Color input);
};

class ITF : public taFile {
public:
    const QStringList validOutputs(){
        QStringList outputs = QStringList{"ITF"};
        foreach(const QByteArray &item, QImageWriter::supportedImageFormats()){
            outputs.append(QString::fromLocal8Bit(item));
        }
        return outputs;
    };
    virtual const QString fileCategory(){
        return "Texture";
    };

    const QStringList alphaTypes = QStringList{"Gradient Alpha","Opaque/No Alpha","Punch-Through Alpha"};

    /*Problems still to be solved:
    0. Make sure the different alpha types write the correct values to the file (ex. opaque should write 0x80)
    1. Rewrite so main image data is mipMaps[0], for consistency - easier on qtcreator
    2. Saving alternate palettes to QImage formats - actually, this is probably already handled by selectPalette
    4. Clean up function and member variable names - easier on qtcreator
    */


    int fileLength;
    int versionNum;
    int headerLength;
    int propertyByte;
    int alphaType; //related to alpha transparency - possibly determines punch-through bit vs actual alpha? http://sv-journal.org/2014-1/06/en/index.php?lang=en
    //type 0: MOTION_BLADE. Normal blended alpha
    //type 1: most textures, but also SENTRYDRONE2. Opaque. Not sure why these textures still have an alpha value, worth looking into.
    //type 2: MAGA_WHEEL, DECEPTICON_DROPSHIP, UNICRON_SPINNERS. Punchthrough alpha.

    //also known as mipmaps, for those who know what they're talking about (not us, apparently) https://docs.unity3d.com/Manual/texture-mipmaps-introduction.html
    int mipmapCount; //texture LODs https://discord.com/channels/393973249685323777/769368848779706410/1129052999390593115

    int paletteCount;
    int unknown4Byte3;
    int unknown4Byte4;
    bool swizzled;
    bool hasPalette;
    bool hasMipmaps;
    int currentPalette;
    int currentMipMaps;
    QComboBox *comboPalettes;
    QTableWidget *paletteTable;
    QLabel *labelImageDisplay;
    //QImage imageData; //replaced by mipmaps[0]
    std::vector<QImage> mipMaps;
    std::vector<QLabel*> labelMipMaps;


    void writeITF();
    void readPalette();
    void readIndexedData();
    void readImageData();
    void writeIndexedData(QFile& fileOut, QImage *writeData);
    void writeImageData(QFile& fileOut, QImage *writeData);
    void populatePalette();
    void editPalette(int row, int column);
    void save(QString toType);
    void load(QString fromType);
    void updateCenter();
    void updatePreview();
    void selectPalette(int palette);
    void selectMipMap(int mipmapCount);
    int bytesPerPixel();
    int dataLength();
    void adaptProperties();
    void createMipMaps(int mipmapLevels);
    int importPalette();
    void promptColorToIndex();
    void promptIndexToColor();
    void convertColorToIndex(bool cancelled);
    void convertIndexToColor(bool cancelled);
    void convertAlphaType(int index);
    void convertGradientAlpha(QImage *imageData);
    void convertOpaqueAlpha(QImage *imageData);
    void convertPunchthroughAlpha(QImage *imageData);
    void changeSwizzleType(int index);
    void changeColorTable(bool input);

    private:
    void saveITFPalette();
    int readDataITF();
    void unswizzle();
    void swizzle();
};

#endif // ITF_H
