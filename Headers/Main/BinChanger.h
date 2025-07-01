#ifndef BINCHANGER_H
#define BINCHANGER_H

#include <QString>
#include <QByteArray>
#include <QFile>
#include <QDebug>
#include <QMatrix3x3>
#include <QVector3D>
class exDebugger;

typedef signed char exInt8;
typedef unsigned char exUInt8;
typedef signed short exInt16;
typedef unsigned short exUInt16;
typedef signed int exInt32;
typedef unsigned int exUInt32;
typedef signed long exInt64;
typedef unsigned long exUInt64;

class SectionHeader{
public:
    QString type;
    bool hasName;
    QString name;
    int sectionLocation;
    int sectionLength;
    int id;

    SectionHeader();
    const void operator=(SectionHeader input);
    void clear();
};

class TextProperty{
public:
    QString name;
    QString readValue;
};

class BinChanger{
public:
    static QString reverse_input(QString input, int unitLength);
    static QByteArray reverse_input(QByteArray input, int unitLength);
    static QString hex_to_bin(QByteArray arrhex);
    static int twosCompConv(QString input, int length);
    static QString twosCompConv(int intput, int length);
    static QString signExtend(QString input, int length);
    static QByteArray remakeImm(QString tempRead, long immediate);
    static std::tuple<int8_t,int8_t> byte_to_nib(QByteArray input);
    static exUInt8 nib_to_byte(std::tuple<int8_t, int8_t> input);
    static float hex_to_float(QByteArray array);
    static QByteArray float_to_hex(float input);
    static qint64 hexWrite(QFile& file, const QByteArray var);
    static qint64 byteWrite(QFile& file, const int8_t var);
    static qint64 shortWrite(QFile& file, const int16_t var);
    static qint64 intWrite(QFile& file, const int32_t var);
    static qint64 longWrite(QFile& file, const int64_t var);
    static QVector3D forcedRotate(QMatrix3x3 rotMatrix, QVector3D offset, QVector3D point);
};

enum ColorType{
    ColorType_RGBA_Int
    , ColorType_RGB_Int
    , ColorType_RGBA_Char
    , ColorType_RGB_Char
    , ColorType_RGBA_Float
    , ColorType_RGB_Float
    , ColorType_RGBA_Compressed
};

class FileData{
public:
    QByteArray dataBytes;
    exDebugger *m_Debug;
    long currentPosition = 0;
    bool input;
    int line = 0;

    template<typename T>
    QByteArray makeInt(T value);

    void readFile(QString filePath);
    void writeFile(QString filePath);
    void process(char &data);
    void process(exInt8 &data);
    void process(exUInt8 &data);
    void process(std::tuple<exUInt8, exUInt8> &data);
    void process(exInt16 &data);
    void process(exUInt16 &data);
    void process(exInt32 &data);
    void process(exUInt32 &data);
    void process(exInt64 &data);
    void process(exUInt64 &data);
    void process(bool &data);
    void process(float &data, bool isMini = false);
    void process(QVector3D &data, bool isMini = false);
    void process(QVector4D &data);
    void process(QQuaternion &data, bool isMini = false);
    void process(QByteArray &data, int length);
    void process(QString &data, int length);

    void process(QColor &data, int colorType);


    long readLong(int length = 4, long location = 0);
    int readInt(int length = 4, long location = 0);
    int readUInt(int length = 4, long location = 0);
    bool readBool(int length = 1, long location = 0);
    float readFloat(int length = 4, long location = 0);
    float readMiniFloat(int length = 2, long location = 0);
    uint32_t readSpecial(int length = 4, long location = 0);
    QVector3D read3DVector();
    QVector3D readMini3DVector();
    QVector4D read4DVector();
    QColor readColor(bool isFloat);
    QQuaternion readQuaternion();
    QQuaternion readMiniQuaternion();
    QByteArray readHex(int length, long location = 0);
    //    void vector3DValue(QVector3D* value, long location = 0);
    //    void vector4DValue(QVector4D* value, long location = 0);
    //    void quaternionValue(QQuaternion* value, long location = 0);
    //    void miniQuaternionValue(QQuaternion* value, long location = 0); //referred to as "packed quaternion" in game ELF
    void hexValue(QByteArray* value, int length, long location = 0);
    void hexValue(QString* value, int length, long location = 0);
    QByteArray mid(long location, int length);
    void signature(SectionHeader *signature);
    void textSignature(SectionHeader *signature);
    QString textWord();
    QString nextLine(); //skips to next line and returns all text between cursor and next line
    TextProperty readProperty();
    int skipLine(bool checkEmpty = false);
};

#endif // BINCHANGER_H
