#include "Headers/Main/mainwindow.h"
#include "Headers/Databases/DistanceCalculator.h"

TFFile::TFFile(){
    parent = nullptr;
    duplicateFileCount = 0;
    fileName = "";
    fileExtension = "";
    this->fileData = nullptr;
}

void TFFile::acceptVisitor(ProgWindow& visitor){
    visitor.visit(*this);
}

uint32_t FileData::readSpecial(int length, long location){
    uint32_t readValue = BinChanger::reverse_input(BinChanger::reverse_input(dataBytes.mid(currentPosition + location, length).toHex(),2), 1).toUInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

long FileData::readLong(int length, long location){
    long readValue = BinChanger::reverse_input(dataBytes.mid(currentPosition + location, length).toHex(),2).toLong(nullptr, 16);
    currentPosition += length;
    return readValue;
}

int FileData::readInt(int length, long location){
    QString readBin = BinChanger::reverse_input(BinChanger::hex_to_bin(dataBytes.mid(currentPosition + location, length)), 8);
    int twoscompread = BinChanger::twosCompConv(readBin, 8);
    //qDebug() << Q_FUNC_INFO << "hex read:" << dataBytes.mid(currentPosition + location, length).toHex() << "read as" << readBin << "then" << twoscompread;
    currentPosition += length;
    return twoscompread;
}

int FileData::readUInt(int length, long location){
    int readValue = BinChanger::reverse_input(dataBytes.mid(currentPosition + location, length).toHex(),2).toUInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

bool FileData::readBool(int length, long location){
    bool readValue = dataBytes.mid(currentPosition + location, length).toHex().toInt(nullptr, 16);
    currentPosition += length;
    return readValue;
}

float FileData::readFloat(int length, long location){
    //qDebug() << Q_FUNC_INFO << "full read" << dataBytes;
    //qDebug() << Q_FUNC_INFO << "current position" << currentPosition << "hex read:" << dataBytes.mid(currentPosition + location, length);
    float readValue = BinChanger::hex_to_float(BinChanger::reverse_input(dataBytes.mid(currentPosition + location, length).toHex(), 2));
    if(readValue < 0.00001 and readValue > -0.00001){ //also turbohack
        readValue = 0;
    }
    currentPosition += length;
    return readValue;
}

float FileData::readMiniFloat(int length, long location){
    QString readBin = BinChanger::reverse_input(BinChanger::hex_to_bin(dataBytes.mid(currentPosition + location, length)), 8);
    int twoscompread = BinChanger::twosCompConv(readBin, 8);
    float reduceInt = float(twoscompread) / 30000;
    //qDebug() << Q_FUNC_INFO << "int read:" << float(twoscompread) << "then becomes" << reduceInt;
    currentPosition += length;
    return reduceInt;
}

QVector3D FileData::read3DVector(){
    float x_value = readFloat();
    float y_value = readFloat();
    float z_value = readFloat();
    QVector3D vector = QVector3D(x_value, y_value, z_value);

    return vector;
}

QVector3D FileData::readMini3DVector(){
    float x_value = readMiniFloat();
    float y_value = readMiniFloat();
    float z_value = readMiniFloat();
    QVector3D vector = QVector3D(x_value, y_value, z_value);

    return vector;
}

QVector4D FileData::read4DVector(){
    float x_value = readFloat();
    float y_value = readFloat();
    float z_value = readFloat();
    float w_value = readFloat();
    QVector4D vector = QVector4D(x_value, y_value, z_value, w_value);

    return vector;
}

QColor FileData::readColor(bool isFloat){
    QColor color;
    if(isFloat){
        float r_value = readFloat();
        float g_value = readFloat();
        float b_value = readFloat();
        float a_value = readFloat();
        color = QColor(r_value, g_value, b_value, a_value);
    } else {
        int r_value = readInt();
        int g_value = readInt();
        int b_value = readInt();
        int a_value = readInt();
        color = QColor(r_value, g_value, b_value, a_value);
    }

    return color;
}

QQuaternion FileData::readQuaternion(){
    float x_value = readFloat();
    float y_value = readFloat();
    float z_value = readFloat();
    float m_value = readFloat();
    QQuaternion rotation = QQuaternion(m_value, x_value, y_value, z_value);

    return rotation;
}

QQuaternion FileData::readMiniQuaternion(){
    float x_value = readMiniFloat();
    float y_value = readMiniFloat();
    float z_value = readMiniFloat();
    float m_value = readMiniFloat();
    QQuaternion rotation = QQuaternion(m_value, x_value, y_value, z_value);

    return rotation;
}

//void FileData::vector3DValue(QVector3D* value, long location){
//    if(input){
//        float x_value = readFloat();
//        float y_value = readFloat();
//        float z_value = readFloat();
//        *value = QVector3D(x_value, y_value, z_value);
//    } else {
//        parent->messageError("This is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
//    }
//}

//void FileData::vector4DValue(QVector4D* value, long location){
//    if(input){
//        float x_value = readFloat();
//        float y_value = readFloat();
//        float z_value = readFloat();
//        float w_value = readFloat();
//        *value = QVector4D(x_value, y_value, z_value, w_value);
//    } else {
//        parent->messageError("This is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
//    }
//}

//void FileData::quaternionValue(QQuaternion* value, long location){
//    if(input){
//        float x_value = readFloat();
//        float y_value = readFloat();
//        float z_value = readFloat();
//        float m_value = readFloat();
//        *value = QQuaternion(m_value, x_value, y_value, z_value);
//    } else {
//        parent->messageError("This is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
//    }
//}

//void FileData::miniQuaternionValue(QQuaternion* value, long location){
//    if(input){
//        float x_value = readMiniFloat();
//        float y_value = readMiniFloat();
//        float z_value = readMiniFloat();
//        float m_value = readMiniFloat();
//        *value = QQuaternion(m_value, x_value, y_value, z_value);
//    } else {
//        parent->messageError("This is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
//    }
//}

QByteArray FileData::readHex(int length, long location){
    QByteArray readValue = dataBytes.mid(currentPosition + location, length);
    currentPosition += length;
    return readValue;
}

void FileData::hexValue(QString* value, int length, long location){
    if(input){
        *value = QString(dataBytes.mid(currentPosition + location, length));
        currentPosition += length;
    } else {
        parent->messageError("Writing hex strings is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
    }
}

void FileData::hexValue(QByteArray* value, int length, long location){
    if(input){
        *value = dataBytes.mid(currentPosition + location, length);
        currentPosition += length;
    } else {
        parent->messageError("Writing hex byte arrays is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
    }
}

QByteArray FileData::mid(long location, int length){
    QByteArray readValue = dataBytes.mid(location, length);
    return readValue;
}

void FileData::readFile(QString filePath){
    dataBytes.clear();

    QFile inputFile(filePath);
    QFileInfo fileInfo(inputFile.fileName());
    inputFile.open(QIODevice::ReadOnly);
    dataBytes = inputFile.readAll();
    qDebug() << Q_FUNC_INFO << "file data read length:" << dataBytes.size();
    currentPosition = 0;
}

SectionHeader::SectionHeader(){
    hasName = false;
    name = "";
    type = "";
    sectionLocation = 0;
    sectionLength = 0;
}

void SectionHeader::clear(){
    hasName = false;
    name = "";
    type = "";
    sectionLocation = 0;
    sectionLength = 0;
}

const void SectionHeader::operator=(SectionHeader input){
    hasName = input.hasName;
    name = input.name;
    type = input.type;
    sectionLocation = input.sectionLocation;
    sectionLength = input.sectionLength;
}

QString FileData::textWord(){
    QByteArray expectedWord;
    QString word;
    if(input){
        long wordStart = 0;
        long wordEnd = 0;

        wordStart = currentPosition;
        //qDebug() << Q_FUNC_INFO << "reading text word at" << wordStart;

        QByteArrayMatcher findWordEnd;
        findWordEnd.setPattern(QByteArray::fromHex(QString("20").toUtf8()));
        wordEnd = findWordEnd.indexIn(dataBytes, wordStart);

        if(wordEnd < wordStart){
            parent->log("Word end was found earier than word start. Search started at " + QString::number(wordStart) + " | " + QString(Q_FUNC_INFO));
            wordEnd = wordStart;
        }
        expectedWord = mid(wordStart, wordEnd-wordStart);

        QByteArrayMatcher findLineEnd;
        findLineEnd.setPattern(QByteArray::fromHex(QString("0D0A").toUtf8()));
        if(findLineEnd.indexIn(expectedWord) >= 0){
            //qDebug() << Q_FUNC_INFO << "Attempt to get a word crossed into the next line. That's not cool.";
            return "";
        }

        word = expectedWord;
        currentPosition = wordEnd+1; //+1 to skip the space, otherwise the next word find will fail.
    } else {
        parent->messageError("Writing wordss is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
        return "";
    }
    return word.trimmed();
}

TextProperty FileData::readProperty(){
    TextProperty readProperty;
    QString word;
    long wordStart = 0;
    long wordEnd = 0;

    wordStart = currentPosition;
    qDebug() << Q_FUNC_INFO << "reading text word at" << wordStart;

    QByteArrayMatcher findWordEnd;
    findWordEnd.setPattern(QByteArray::fromHex(QString("3A").toUtf8()));
    wordEnd = findWordEnd.indexIn(dataBytes, wordStart);

    if(wordEnd < wordStart){
        qDebug() << Q_FUNC_INFO << ("Word end was found earier than word start. Search started at " + QString::number(wordStart) + " | " + QString(Q_FUNC_INFO));
        wordEnd = wordStart;
    }
    readProperty.name = mid(wordStart, wordEnd-wordStart);
    currentPosition = wordEnd+2; //+2 to skip the colon and space, otherwise the next word find will fail.
    readProperty.readValue = nextLine();

    return readProperty;
}

QString FileData::nextLine(){
    //qDebug() << Q_FUNC_INFO << "going to next line, starting at" << currentPosition;
    QByteArrayMatcher findEndLine;
    long lineEnd = 0;
    QString lineContents;
    findEndLine.setPattern(QByteArray::fromHex(QString("0D0A").toUtf8()));
    lineEnd = findEndLine.indexIn(dataBytes, currentPosition);
    if(lineEnd < currentPosition){
        lineEnd = dataBytes.size(); //there are no more line breaks, so the end of the line must be the end of the file.
        lineContents = mid(currentPosition, lineEnd-currentPosition);
        currentPosition = lineEnd;
    } else {
        lineContents = mid(currentPosition, lineEnd-currentPosition-1); //-1 for space at the end of the line
        currentPosition = lineEnd+2; //+2 for 0D0A /r/n
    }
    line++;
    return lineContents;
    //qDebug() << Q_FUNC_INFO << "Starting next line at" << currentPosition;
}

/*Skips a line. If set to check for empty, will return true on non-empty line.*/
int FileData::skipLine(bool checkEmpty){
    //qDebug() << Q_FUNC_INFO << "skipping line, starting at" << currentPosition;
    QString lineContents;
    QByteArrayMatcher findEndLine;
    findEndLine.setPattern(QByteArray::fromHex(QString("0D0A").toUtf8()));
    long endLine = findEndLine.indexIn(dataBytes, currentPosition) - currentPosition;
    lineContents = dataBytes.mid(currentPosition, endLine).trimmed();
    //qDebug() << Q_FUNC_INFO << "line contents read as" << lineContents << "with length" << lineContents.length();
    if(checkEmpty){
        if(lineContents.length() != 0){
            //qDebug() << Q_FUNC_INFO << "Line was not empty at" << currentPosition;
            return 1;
        }
    }
    currentPosition += endLine+2;
    line++;
    //qDebug() << Q_FUNC_INFO << "Starting next line at" << currentPosition;
    return 0;
}

void FileData::textSignature(SectionHeader *signature){
    if(input){
        QString nameCheck;
        long signatureStart = 0;
        long signatureEnd = 0;

        //qDebug() << Q_FUNC_INFO << "checking for signature at" << currentPosition;
        signatureStart = currentPosition;

        QByteArrayMatcher findSignatureEnd;
        findSignatureEnd.setPattern(QByteArray::fromHex(QString("20").toUtf8()));
        signatureEnd = findSignatureEnd.indexIn(dataBytes, signatureStart);

        if(signatureEnd < signatureStart){
            qDebug()<< Q_FUNC_INFO << "Signature end was found earier than signature start. Search started at " << signatureStart;
            signatureEnd = signatureStart;
        }
        nameCheck = mid(signatureStart, signatureEnd-signatureStart).trimmed();
        if(nameCheck.left(1) == "~"){
            //qDebug() << Q_FUNC_INFO << "Signature verified as" << nameCheck;
            nameCheck = nameCheck.right(nameCheck.size()-1);
            signature->type = nameCheck;
            currentPosition = signatureEnd;
            signature->hasName = false;
            signature->sectionLocation = line;
        } else {
            //qDebug() << Q_FUNC_INFO << "Signature not verified. Read as:" << nameCheck;
            currentPosition -= nameCheck.length();
            return;
        }
    } else {
        parent->messageError("Writing signatures is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
    }
}

void FileData::signature(SectionHeader *signature){
    if(input){
        long signatureStart = 0;
        long signatureEnd = 0;

        signatureStart = currentPosition+1;

        QByteArrayMatcher findSignatureEnd;
        findSignatureEnd.setPattern(QByteArray::fromHex(QString("00").toUtf8()));
        signatureEnd = findSignatureEnd.indexIn(dataBytes, signatureStart);

        //qDebug() << Q_FUNC_INFO << "signature start:" << signatureStart << "signature end" << signatureEnd << "current position" << currentPosition;

        if(signatureEnd < signatureStart){
            qDebug()<< Q_FUNC_INFO << "Signature end was found earier than signature start. Search started at " << signatureStart;
            signatureEnd = signatureStart;
        }
        signature->type = mid(signatureStart, signatureEnd-signatureStart);

        currentPosition = signatureEnd + 1;
        signature->hasName = readBool();

        if(signature->hasName){
            int sectionNameLength = readInt();
            //signature->name = readHex(sectionNameLength);
            hexValue(&signature->name, sectionNameLength);
        }
        signature->sectionLocation = currentPosition;
        signature->sectionLength = readInt();

        //qDebug() << Q_FUNC_INFO << "position after signature read:" << currentPosition <<". last data read as" << signature->sectionLength << "at" << signature->sectionLocation;
    } else {
        parent->messageError("Writing signatures is not currently supported. It shouldn't even be called. How did you do that? Let Trevor know. " + QString(Q_FUNC_INFO));
    }
}

QString BinChanger::signExtend(QString input, int length){
    bool signBit;
    QString output = input;
    signBit = input.left(1).toInt();
    if(input.length() < length){
        if(signBit){
            while(output.length() < length){
                output = "1" + output;
            }
        } else {
            while(output.length() < length){
                output = "0" + output;
            }
        }
    }
    return output;
}

QVector3D BinChanger::forcedRotate(QMatrix3x3 rotMatrix, QVector3D offset, QVector3D point){
    float rotX = (rotMatrix(0,0)*point.x())+(rotMatrix(0,1)*point.y())+(rotMatrix(0,2)*point.z())+offset.x();
    float rotY = (rotMatrix(1,0)*point.x())+(rotMatrix(1,1)*point.y())+(rotMatrix(1,2)*point.z())+offset.y();
    float rotZ = (rotMatrix(2,0)*point.x())+(rotMatrix(2,1)*point.y())+(rotMatrix(2,2)*point.z())+offset.z();
    QVector3D changedPoint = QVector3D(rotX, rotY, rotZ);


    return changedPoint;
}

QByteArray BinChanger::remakeImm(QString tempRead, long immediate){
    QString replacement;
    QByteArray replacementArray;
    QString paddedImm;
    tempRead = tempRead.mid(0,16); //get the unaltered part of the instruction
    paddedImm = QString::number(immediate, 2); //get a binary string of the immediate
    while (paddedImm.length()<16) {
        paddedImm = "0"+paddedImm; //pad out to 16 bits. done differently for negative immediates. figure out a fix for that.
    }
    tempRead += paddedImm;
    tempRead = reverse_input(tempRead,8); //reverse the whole instruction
    replacement = QString("%1").arg(tempRead.toULong(nullptr,2),8,16,QChar('0')); //convert binary string into hex string
    replacementArray = QByteArray::fromHex(replacement.toUtf8());
    return replacementArray;
}

int BinChanger::twosCompConv(QString input, int length){
    bool isSet;
    bool signBit;
    QString tempValue;
    input = signExtend(input, length);
    signBit = input.left(1).toInt();
    if (signBit){
        for(int i = input.length(); i >= 0 ; i--){
            isSet = input.mid(i, 1).toInt();
            if (isSet){
                for(int j = 0; j < i; j++){
                    if (input.mid(j, 1).toInt() == 1){
                        tempValue += "0";
                    }
                    else {
                        tempValue += "1";
                    }
                }
                for(int j = i; j <= input.length(); j++){
                    tempValue += input.mid(j, 1);
                }
                //qDebug() << Q_FUNC_INFO << "twos comp convert: " << input << " goes to " << tempValue << " " << tempValue.toInt(nullptr, 2);
                return 0 - tempValue.toInt(nullptr, 2);
            }
        }
        //qDebug() << Q_FUNC_INFO << "twos comp convert: no set bits found.";
        return 0 - input.toInt(nullptr, 2);
    }
    return input.toInt(nullptr, 2);
}

QString BinChanger::twosCompConv(int intput, int length){
    bool isSet;
    bool signBit;
    QString tempValue;
    QString input = QStringLiteral("%1").arg(intput, length, 2, QLatin1Char('0'));
    //QString input = QString::number(intput, 2);
    input = signExtend(input, length);
    signBit = input.left(1).toInt();
    if (signBit){
        for(int i = input.length(); i >= 0 ; i--){
            isSet = input.mid(i, 1).toInt();
            if (isSet){
                for(int j = 0; j < i; j++){
                    if (input.mid(j, 1).toInt() == 1){
                        tempValue += "0";
                    }
                    else {
                        tempValue += "1";
                    }
                }
                for(int j = i; j <= input.length(); j++){
                    tempValue += input.mid(j, 1);
                }
                //qDebug() << Q_FUNC_INFO << "twos comp convert: " << input << " goes to " << tempValue << " " << tempValue.toInt(nullptr, 2);
                return "-0x" + QString::number(tempValue.toInt(nullptr, 2), 16);
                //return QString::number(0 - tempValue.toInt(nullptr, 2));
            }
        }
        //qDebug() << Q_FUNC_INFO << "twos comp convert: no set bits found.";
        return "-0x" + QString::number(input.toInt(nullptr, 2), 16);
        //return QString::number(0 - input.toInt(nullptr, 2));
    }
    return "0x" + QString::number(input.toInt(nullptr, 2), 16);
    //return QString::number(input.toInt(nullptr, 2));
}

QString BinChanger::reverse_input(QString input, int unitLength) {
    QString part;
    QString output = "";
    int bytes = int(input.length()/unitLength);
    for (int i = bytes; i >= 0; --i){
        part = input.mid(i*unitLength, unitLength);
        output += part;
    }
    return output;
}

QByteArray BinChanger::reverse_input(QByteArray input, int unitLength) {
    QByteArray part;
    QByteArray output = "";
    int bytes = int(input.length()/unitLength);
    for (int i = bytes; i >= 0; --i){
        part = input.mid(i*unitLength, unitLength);
        output += part;
    }
    return output;
}

QString hex_char_to_bin(QChar c)
{
    switch (c.toUpper().unicode())
    {
    case '0': return "0000";
    case '1': return "0001";
    case '2': return "0010";
    case '3': return "0011";
    case '4': return "0100";
    case '5': return "0101";
    case '6': return "0110";
    case '7': return "0111";
    case '8': return "1000";
    case '9': return "1001";
    case 'A': return "1010";
    case 'B': return "1011";
    case 'C': return "1100";
    case 'D': return "1101";
    case 'E': return "1110";
    case 'F': return "1111";
    default:  return "BR";
    }
}

QString BinChanger::hex_to_bin(QByteArray arrhex)
{
    QString hex = arrhex.toHex();
    unsigned long long hexlen = hex.length();
    QString bin;
    for (unsigned long long i = 0; i < hexlen; ++i) {
        QString binhex = hex_char_to_bin(hex[i]);
        bin += binhex;
    }
    return bin;
}

std::tuple<int8_t,int8_t> BinChanger::byte_to_nib(QByteArray input){
    std::tuple <int8_t, int8_t> nibTup;
    int tempNib = 0;

    tempNib = input.toHex().toInt(nullptr, 16);

    std::get<0>(nibTup) = tempNib & 15;

    tempNib &= 240;
    std::get<1>(nibTup) = tempNib >> 4;
    //qDebug() << Q_FUNC_INFO << "input: " << input.toHex() << " tempNib: " << tempNib << " lower nib: " << std::get<0>(nibTup) << " upper nib: " << std::get<1>(nibTup);

    return nibTup;
}

int BinChanger::nib_to_byte(std::tuple<int8_t, int8_t> input){
    int tempInt = 0;
    tempInt += std::get<0>(input);
    tempInt += std::get<1>(input)<<4;
    //qDebug() << Q_FUNC_INFO << "input lower nib: " << std::get<0>(input) << " upper nib: " << std::get<1>(input) << " tempInt: " << tempInt;

    return tempInt;
}

float BinChanger::hex_to_float(QByteArray array){
    bool ok;
    int sign = 1;
    long long zeroCheck = 0;
    zeroCheck = array.toLongLong(nullptr, 16);
    //qDebug() << Q_FUNC_INFO << "Starting array: " << array;
    //qDebug() << Q_FUNC_INFO << "zerocheck: " << zeroCheck << " for array: " << array;
    if (zeroCheck == 0){
        return 0;
    }
    array = QByteArray::number(array.toLongLong(&ok,16),2); //convert hex to binary
    if(array.length()==32) {
        if(array.at(0)=='1')  sign =-1;                       // if bit 0 is 1 number is negative
        array.remove(0,1);                                     // remove sign bit
    }
    QByteArray fraction =array.right(23);   //get the fractional part
    double mantissa = 0;
    for(int i=0;i<fraction.length();i++)     // iterate through the array to claculate the fraction as a decimal.
        if(fraction.at(i)=='1')     mantissa += 1.0/(pow(2,i+1));
    int exponent = array.left(array.length()-23).toLongLong(&ok,2);
    //qDebug() << Q_FUNC_INFO << "binary read: " << QString(array.left(array.length()-23)) << "exponent: " << exponent;
    exponent -= 127;     //claculate the exponent
    //qDebug() << Q_FUNC_INFO << "number= "<< QString::number( sign*pow(2,exponent)*(mantissa+1.0),'f', 5 );
    return sign*pow(2,exponent)*(mantissa+1.0);
}

QByteArray BinChanger::float_to_hex(float input){
    QByteArray array;
    const unsigned char * pf = reinterpret_cast<const unsigned char*>(&input);

    for (size_t i = 0; i != sizeof(float); ++i)
    {
        array.push_back(pf[i]);

    }
    //qDebug() << Q_FUNC_INFO << "Float value" << input << "converted to" << array;
    return array;
}

qint64 BinChanger::hexWrite(QFile& file, QByteArray var) {
    qint64 written = 0;
    for(int i = 0; i < var.size(); i++){
      written = byteWrite(file, var.at(i));
    }
    //qDebug () << "out: " << written;
    return written;
}

qint64 BinChanger::byteWrite(QFile& file, int8_t var) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug() << "write error";
  }
  //qDebug () << "out: " << written;
  return written;
}

qint64 BinChanger::shortWrite( QFile& file, int16_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64 written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 BinChanger::intWrite( QFile& file, int32_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

qint64 BinChanger::longWrite( QFile& file, int64_t var ) {
  qint64 toWrite = sizeof(decltype (var));
  qint64  written = file.write(reinterpret_cast<const char*>(&var), toWrite);
  if (written != toWrite) {
    qDebug () << "write error";
  }
   //qDebug () << "out: " << written;
  return written;
}

void TFFile::save(QString toType){
    qDebug() << Q_FUNC_INFO << "The class you tried to save doesn't have a valid save function.";
}

void TFFile::save(QString toType, QTextStream &stream){
    qDebug() << Q_FUNC_INFO << "The class you tried to save doesn't have a valid save function (textstream version).";
}

void TFFile::load(QString fromType){
    qDebug() << Q_FUNC_INFO << "The class you tried to load doesn't have a valid load function yet.";
}

void TFFile::updateCenter(){
    qDebug() << Q_FUNC_INFO << "The class you selected doesn't have a valid central layout yet.";
}
