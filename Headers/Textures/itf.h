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
#include "Headers/Main/BinChanger.h"

class exImageData{
public:
    std::vector<QColor> m_pixels;
    std::vector<int> m_indexedPixels;

    void SetPixel(int index, QColor pixel);
    void SetPixel(int pixelIndex, int colorIndex);
};

class exPaletteData{
public:
    std::vector<QColor> m_colors;

    void SetColor(int index, QColor color);
};

enum ITFProperties
{
    //Not sure if the binary signifies anything, it doesn't seem to
    ITFProperties_32bpp = 2 //0010
    , ITFProperties_16bpp = 7 //0111
    , ITFProperties_4bpp = 10 //1010
    , ITFProperties_8bpp = 11 //1011
    , ITFProperties_24bpp = 12 //1100
    , ITFProperties_Swizzled = 128
};

enum Alpha{
    Alpha_Gradient
    ,Alpha_Opaque
    ,Alpha_Punchthrough
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


    exUInt32 fileLength;
    exUInt8 versionNum;
    exUInt32 headerLength;
    exUInt8 m_format;
    exUInt32 alphaType; //related to alpha transparency - possibly determines punch-through bit vs actual alpha? http://sv-journal.org/2014-1/06/en/index.php?lang=en
    //type 0: MOTION_BLADE. Normal blended alpha
    //type 1: most textures, but also SENTRYDRONE2. Opaque. Not sure why these textures still have an alpha value, worth looking into.
    //type 2: MAGA_WHEEL, DECEPTICON_DROPSHIP, UNICRON_SPINNERS. Punchthrough alpha.

    exUInt32 m_width;
    exUInt32 m_height;

    //https://docs.unity3d.com/Manual/texture-mipmaps-introduction.html
    exUInt32 mipmapCount; //texture LODs https://discord.com/channels/393973249685323777/769368848779706410/1129052999390593115

    exUInt32 paletteCount;
    exUInt32 unknown4Byte3;
    exUInt32 unknown4Byte4;
    exUInt32 m_dataLength;
    bool swizzled;
    bool hasPalette;
    bool hasMipmaps;
    int currentPalette;
    int currentMipMaps;
    QComboBox *comboPalettes;
    QTableWidget *paletteTable;
    QLabel *labelImageDisplay;
    //QImage imageData; //replaced by mipmaps[0]
    std::vector<exImageData> m_mipMaps;
    std::vector<exPaletteData> m_palettes;
    //std::vector<QImage> mipMaps;
    std::vector<QLabel*> labelMipMaps;


    QImage CreateImage(int mipMapLevel = 0);
    void AdaptImage(QImage input);
    exImageData ScaleBase(int mipMapLevel);
    void writeITF();
    void readPalette();
    void readIndexedData();
    void readImageData();
    void writeIndexedData(QFile& fileOut, exImageData *writeData);
    void writeImageData(QFile& fileOut, exImageData *writeData);
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
    void createMipMaps(int mipmapLevels);
    int importPalette();
    void promptColorToIndex();
    void promptIndexToColor();
    void convertColorToIndex(bool cancelled);
    void convertIndexToColor(bool cancelled);
    void convertAlpha(int index);
    void changeSwizzleType(int index);
    void changeColorTable(bool input);
    int MapWidth(int level);
    int MapHeight(int level);

    private:
    void saveITFPalette();
    int readDataITF();
    void swizzle(bool unswizzle = false);
};

#endif // ITF_H
