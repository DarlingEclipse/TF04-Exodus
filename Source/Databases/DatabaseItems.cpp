#include <QTimer>
#include <QLineEdit>
#include <QPushButton>

#include "Headers/Databases/Database.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/Main/CustomQT.h"
#include "Headers/Main/BinChanger.h"

/*template <typename ValueType>
ValueType dictItem::searchAttributes(QString itemName){
    for(int i = 0; i < attributes.size(); i++){
        if(attributes[i]->name == itemName){
            return attributes[i]->value();
        }
    }
    return;
}*/

std::shared_ptr<taData> dictItem::getAttribute(QString attributeName){
    for(int i = 0; i < attributes.size(); i++){
        if(attributes[i]->name == attributeName){
            return attributes[i];
        }
    }
    return nullptr;
}

int dictItem::setAttribute(QString itemName, QString value){
    //qDebug() << Q_FUNC_INFO << "Setting attribute" << itemName << "to value" << value;
    for(int i = 0; i < attributes.size(); i++){
        if(attributes[i]->name == itemName){
            if((value == attributes[i]->display() || value == "") && attributes[i]->isDefault){
                return 0;
            }
            attributes[i]->setValue(value);
            attributes[i]->isDefault = false;
            return 0;
        }
    }
    qDebug() << Q_FUNC_INFO << "Could not find attribute to set.";
    return 1; //value not found
}

int dictItem::setAttributeDefault(QString itemName){
    for(int i = 0; i < attributes.size(); i++){
        if(attributes[i]->name == itemName){
            attributes[i]->isDefault = true;
            return 0;
        }
    }
    return 1; //value not found
}

std::variant<QString, QVector3D, QQuaternion, int, float, bool> taData::value(){
    if(type == "String"){
        return stringValue();
    } else if (type == "Point"){
        return vectorValue();
    } else if (type == "Quaternion"){
        return quatValue();
    } else if (type == "Integer" or type == "Link" or type == "Enum"){
        return intValue();
    } else if (type == "Float"){
        return floatValue();
    } else if (type == "Bool"){
        return boolValue();
    }
    return -1;
}



std::shared_ptr<taData> DefinitionFile::createItem(QString itemType){
    if(itemType == "Bool"){
        std::shared_ptr<taDataBool<bool>> boolItem(new taDataBool<bool>);
        boolItem->file = this;
        return boolItem;
    } else if(itemType == "String"){
        std::shared_ptr<taDataString<QString>> stringItem(new taDataString<QString>);
        stringItem->file = this;
        return stringItem;
    } else if(itemType == "Float"){
        std::shared_ptr<taDataFloat<float>> floatItem(new taDataFloat<float>);
        floatItem->file = this;
        return floatItem;
    } else if(itemType == "Quaternion"){
        std::shared_ptr<taDataQuaternion<QQuaternion>> quatItem(new taDataQuaternion<QQuaternion>);
        quatItem->file = this;
        return quatItem;
    } else if(itemType == "Integer"){
        std::shared_ptr<taDataInteger<int>> intItem(new taDataInteger<int>);
        intItem->file = this;
        return intItem;
    } else if(itemType == "Link"){
        std::shared_ptr<taDataLink<uint16_t>> linkItem(new taDataLink<uint16_t>);
        linkItem->file = this;
        return linkItem;
    } else if(itemType == "Flag"){
        std::shared_ptr<taDataFlag<int>> flagItem(new taDataFlag<int>);
        flagItem->file = this;
        return flagItem;
    } else if(itemType == "Point"){
        std::shared_ptr<taDataPoint<QVector3D>> pointItem(new taDataPoint<QVector3D>);
        pointItem->file = this;
        return pointItem;
    } else if(itemType == "Enum"){
        std::shared_ptr<taDataEnum> enumItem(new taDataEnum);
        enumItem->file = this;
        return enumItem;
    } else if(itemType == "StringArray"){
        std::shared_ptr<taDataStringArray<QString>> stringArrayItem(new taDataStringArray<QString>);
        stringArrayItem->file = this;
        return stringArrayItem;
    } else if(itemType == "IntegerArray"){
        std::shared_ptr<taDataIntArray<int>> intArrayItem(new taDataIntArray<int>);
        intArrayItem->file = this;
        return intArrayItem;
    } else if(itemType == "FloatArray"){
        std::shared_ptr<taDataFloatArray<float>> floatArrayItem(new taDataFloatArray<float>);
        floatArrayItem->file = this;
        return floatArrayItem;
    } else if(itemType == "VectorArray"){
        std::shared_ptr<taDataVectorArray<QVector3D>> vectorArrayItem(new taDataVectorArray<QVector3D>);
        vectorArrayItem->file = this;
        return vectorArrayItem;
    } else if(itemType == "LinkArray"){
        std::shared_ptr<taDataLinkArray<uint16_t>> linkArrayItem(new taDataLinkArray<uint16_t>);
        linkArrayItem->file = this;
        return linkArrayItem;
    } else if(itemType == "Color"){
        std::shared_ptr<taDataColor<QColor>> colorItem(new taDataColor<QColor>);
        colorItem->file = this;
        return colorItem;
    } else {
        m_Debug->Log("Data type " + itemType + " hasn't been implemented yet. | " + QString(Q_FUNC_INFO));
        qDebug() << Q_FUNC_INFO << "Data type" << itemType << "hasn't been implemented yet.";
        return nullptr;
    }
}

/*---------- taData ----------*/

void taData::read(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid read function";
}

void taData::write(QFile& file){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid write function";
}

QString taData::display(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid display value";
    return "";
}

QString taData::definitionOutput(){
    //qDebug() << Q_FUNC_INFO << "Outputting default definition display value";
    return display() + " \"" + comment + "\" ";
}

QString taData::databaseOutput(){
    //qDebug() << Q_FUNC_INFO << "Outputting default database display value";
    return display() + " ";
}

QString taData::backupDisplay(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type really does not have a valid display value";
    return "";
}

QStringList taData::options(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return {""};
}

void taData::addOption(QString optionToAdd){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return;
}

QString taData::stringValue(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return "";
}

QVector3D taData::vectorValue(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return QVector3D(0,0,0);
}

QQuaternion taData::quatValue(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return QQuaternion(0,0,0,0);
}

int taData::intValue(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return 0;
}

float taData::floatValue(){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid multi-option value";
    return 0;
}

bool taData::boolValue(){
    return 0;
}

void taData::setValue(QString changedValue){
    //qDebug() << Q_FUNC_INFO << "Item of this data type does not have a valid value-setting function.";
}

std::shared_ptr<taData> taData::clone(){
    file->m_Debug->Log("Data type " + type + " doesn't have a clone operation yet. | " + QString(Q_FUNC_INFO));
    return nullptr;
}

taDataEnum* taData::cloneEnum(){
    return nullptr;
}

int taData::binarySize(){
    int sectionLength = 0;
    sectionLength += 4; //length of type string
    sectionLength += type.size();
    sectionLength += 4; //length of active string
    if(active){
        sectionLength += 4;
    } else {
        sectionLength += 5;
    }
    sectionLength += 4; //length of name string
    sectionLength += name.size();
    sectionLength += size();

    return sectionLength;
}

/*---------- taDataSingle ----------*/

template <class valueType>
valueType taDataSingle<valueType>::giveValue(){
    return value;
}

template <class valueType>
QString taDataSingle<valueType>::display(){
    QString displayValue = QVariant(value).value<QString>();
    //qDebug() << Q_FUNC_INFO << "generating display value for item" << name << "with value" << value << ":" << QVariant(value).value<QString>() << ".";
    if(this->type == "String"){
        displayValue = "\"" + displayValue + "\"";
    }
    if(displayValue == "" or displayValue.left(1) == ","){
        displayValue = backupDisplay();
    }
    return displayValue;
}

/*---------- Bool ----------*/

template <>
std::shared_ptr<taData> taDataBool<bool>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataBool<bool>> stringItem(new taDataBool<bool>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataBool<bool>::setValue(QString changedValue){
    if(changedValue == "True" || changedValue == "1"){
        value = true;
    } else {
        value = false;
    }
}

template <class valueType>
void taDataBool<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readBool();
    } else {
        if(this->file->fileData->textWord() =="true"){
            this->value = true;
        } else {
            this->value = false;
        }
    }
}

template <class valueType>
void taDataBool<valueType>::write(QFile& file){
    BinChanger::byteWrite(file, this->value);
}

/*---------- Float ----------*/


template <>
std::shared_ptr<taData> taDataFloatArray<float>::clone(){
    std::shared_ptr<taDataFloatArray<float>> vectorArrayItem(new taDataFloatArray<float>(*this));
    vectorArrayItem->values = values;
    return vectorArrayItem;
}

template <>
std::shared_ptr<taData> taDataFloat<float>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataFloat<float>> stringItem(new taDataFloat<float>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataFloat<float>::setValue(QString changedValue){
    value = changedValue.toFloat();
}

template <class valueType>
void taDataFloat<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readFloat();
    } else {
        this->value = this->file->fileData->textWord().toFloat();
    }
}

template <class valueType>
void taDataFloat<valueType>::write(QFile& file){
    QByteArray hexFloat = BinChanger::float_to_hex(this->value);
    BinChanger::hexWrite(file, hexFloat);
}

template <class valueType>
QString taDataFloat<valueType>::display(){
    //qDebug() << Q_FUNC_INFO << "Generating float display value";
    return QString::number(this->value, 'g', 5);
}

/*---------- String ----------*/

template <>
std::shared_ptr<taData> taDataString<QString>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataString<QString>> stringItem(new taDataString<QString>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataString<QString>::setValue(QString changedValue){
    value = changedValue;
}

template <class valueType>
void taDataString<valueType>::read(){
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    if(this->file->binary){
        int length = 0;
        length = this->file->fileData->readInt();
        this->value = this->file->fileData->readHex(length);
    } else {
        QString tempRead2 = this->file->fileData->textWord();
        while(tempRead2.count('"') < 2){
            //qDebug() <<Q_FUNC_INFO << "string only contains one quote. this means there's a space in the string. Read again.";
            tempRead2 += " " + this->file->fileData->textWord();
        }
        tempRead2 = tempRead2.remove(quoteRemover);
        //qDebug() << Q_FUNC_INFO << "setting string value to" << tempRead2;
        this->value = tempRead2;
    }
}

template <class valueType>
void taDataString<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->value.length());
    BinChanger::hexWrite(file, this->value.toUtf8());
}

template <class valueType>
QString taDataString<valueType>::stringValue(){
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    return this->value.remove(quoteRemover);
}

/*---------- Int ----------*/

template <>
std::shared_ptr<taData> taDataInteger<int>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataInteger<int>> stringItem(new taDataInteger<int>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataInteger<int>::setValue(QString changedValue){
    value = changedValue.toInt();
}

template <class valueType>
void taDataInteger<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readInt();
    } else {
        this->value = this->file->fileData->textWord().toInt();
    }
}

template <class valueType>
void taDataInteger<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->value);
}

/*---------- Link ----------*/

template <>
std::shared_ptr<taData> taDataLink<uint16_t>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataLink<uint16_t>> stringItem(new taDataLink<uint16_t>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataLink<uint16_t>::setValue(QString changedValue){
    value = changedValue.toInt();
}

template <class valueType>
void taDataLink<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readInt(2);
    } else {
        this->value = this->file->fileData->textWord().toInt();
    }
}

template <class valueType>
void taDataLink<valueType>::write(QFile& file){
    BinChanger::shortWrite(file, this->value);
}

/*---------- Flag ----------*/

template <>
std::shared_ptr<taData> taDataFlag<int>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataFlag<int>> flagItem(new taDataFlag<int>(*this));
    flagItem->value = value;
    return flagItem;
}

template <>
void taDataFlag<int>::setValue(QString changedValue){
    value = changedValue.toInt();
}

template <class valueType>
void taDataFlag<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readInt();
    } else {
        this->value = this->file->fileData->textWord().toInt();
    }
}

template <class valueType>
void taDataFlag<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->value);
}

/*---------- Point ----------*/

template <>
std::shared_ptr<taData> taDataPoint<QVector3D>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataPoint<QVector3D>> stringItem(new taDataPoint<QVector3D>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataPoint<QVector3D>::setValue(QString changedValue){
    QStringList splitValue = changedValue.split(",");
    qDebug() << Q_FUNC_INFO << "Value received:" << changedValue << "split into" << splitValue;
    value = QVector3D(splitValue[0].toFloat(),splitValue[1].toFloat(),splitValue[2].toFloat());
    qDebug() << Q_FUNC_INFO << "new value:" << value;
}

template <class valueType>
void taDataPoint<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->read3DVector();
    } else {
        QString tempx = this->file->fileData->textWord();
        float x = tempx.toFloat();
        QString tempy = this->file->fileData->textWord();
        float y = tempy.toFloat();
        QString tempz = this->file->fileData->textWord();
        float z = tempz.toFloat();
        this->value = QVector3D(x,y,z);
        //qDebug() << Q_FUNC_INFO << "made point" << this->value << "from values" << tempx << x << tempy << y << tempz << z << "at" << this->file->fileData->currentPosition;
    }
}

template <class valueType>
void taDataPoint<valueType>::write(QFile& file){
    QByteArray hexFloat;
    hexFloat = BinChanger::float_to_hex(this->value.x());
    BinChanger::hexWrite(file, hexFloat);
    hexFloat = BinChanger::float_to_hex(this->value.y());
    BinChanger::hexWrite(file, hexFloat);
    hexFloat = BinChanger::float_to_hex(this->value.z());
    BinChanger::hexWrite(file, hexFloat);
}

template <class valueType>
QString taDataPoint<valueType>::backupDisplay(){
    QString displayValue;
    displayValue = QString::number(this->value.x()) + ", " + QString::number(this->value.y()) + ", "+QString::number(this->value.z());
    return displayValue;
}

template <class valueType>
QString taDataPoint<valueType>::definitionOutput(){
    QString outputValue;
    outputValue = QString::number(this->value.x()) + " " + QString::number(this->value.y()) + " "+QString::number(this->value.z());
    outputValue += " \"" + this->comment + "\" ";
    return outputValue;
}

template <class valueType>
QString taDataPoint<valueType>::databaseOutput(){
    QString outputValue;
    outputValue = QString::number(this->value.x()) + " " + QString::number(this->value.y()) + " "+QString::number(this->value.z()) + " ";
    return outputValue;
}

template <class valueType>
QString taDataPoint<valueType>::stringValue(){
    return this->backupDisplay();
}

template <class valueType>
QVector3D taDataPoint<valueType>::vectorValue(){
    return this->value;
}

/*---------- Color ----------*/

template <class valueType>
void taDataColor<valueType>::read(){
    if(this->file->binary){
        int r = this->file->fileData->readInt();
        int g = this->file->fileData->readInt();
        int b = this->file->fileData->readInt();
        int a = this->file->fileData->readInt();
        this->value = QColor(r, g, b, a);
    } else {
        float r = this->file->fileData->textWord().toFloat();
        float g = this->file->fileData->textWord().toFloat();
        float b = this->file->fileData->textWord().toFloat();
        float a = this->file->fileData->textWord().toFloat();
        //qDebug() << Q_FUNC_INFO << "color values on read:" << r << g << b << a;
        this->value = QColor::fromRgbF(r, g, b, a);
        //qDebug() << Q_FUNC_INFO << "Color values on value set:" << this->value;
    }
}

template <class valueType>
void taDataColor<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->value.red());
    BinChanger::intWrite(file, this->value.green());
    BinChanger::intWrite(file, this->value.blue());
    BinChanger::intWrite(file, this->value.alpha());
}

template <class valueType>
QString taDataColor<valueType>::definitionOutput(){
    QString outputValue;
    outputValue = QString::number(this->value.redF(), 'g', 5) + " " + QString::number(this->value.greenF(), 'g', 5)
            + " " + QString::number(this->value.blueF(), 'g', 5) + " " + QString::number(this->value.alphaF(), 'g', 5);
    outputValue += " \"" + this->comment + "\" ";
    return outputValue;
}

template <class valueType>
QString taDataColor<valueType>::databaseOutput(){
    QString outputValue;
    outputValue = QString::number(this->value.redF(), 'g', 5) + " " + QString::number(this->value.greenF(), 'g', 5)
            + " " + QString::number(this->value.blueF(), 'g', 5) + " " + QString::number(this->value.alphaF(), 'g', 5);
    outputValue += " ";
    return outputValue;
}

template <>
std::shared_ptr<taData> taDataColor<QColor>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataColor<QColor>> stringItem(new taDataColor<QColor>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataColor<QColor>::setValue(QString changedValue){
    QStringList splitValue = changedValue.split(",");
    value = QColor(splitValue[0].toInt(),splitValue[1].toInt(),splitValue[2].toInt(),splitValue[3].toInt());
}

/*---------- Quaternion ----------*/

template <>
std::shared_ptr<taData> taDataQuaternion<QQuaternion>::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << value;
    std::shared_ptr<taDataQuaternion<QQuaternion>> stringItem(new taDataQuaternion<QQuaternion>(*this));
    stringItem->value = value;
    return stringItem;
}

template <>
void taDataQuaternion<QQuaternion>::setValue(QString changedValue){
    QStringList splitValue = changedValue.split(",");
    value = QQuaternion(splitValue[0].toFloat(),splitValue[1].toFloat(),splitValue[2].toFloat(),splitValue[3].toFloat());
}

template <class valueType>
void taDataQuaternion<valueType>::read(){
    if(this->file->binary){
        this->value = this->file->fileData->readQuaternion();
    } else {
        int x = this->file->fileData->textWord().toFloat();
        int y = this->file->fileData->textWord().toFloat();
        int z = this->file->fileData->textWord().toFloat();
        int m = this->file->fileData->textWord().toFloat();
        this->value = QQuaternion(m,x,y,z);
    }
}

template <class valueType>
void taDataQuaternion<valueType>::write(QFile& file){
    QByteArray hexFloat;
    hexFloat = BinChanger::float_to_hex(this->value.x());
    BinChanger::hexWrite(file, hexFloat);
    hexFloat = BinChanger::float_to_hex(this->value.y());
    BinChanger::hexWrite(file, hexFloat);
    hexFloat = BinChanger::float_to_hex(this->value.z());
    BinChanger::hexWrite(file, hexFloat);
    hexFloat = BinChanger::float_to_hex(this->value.scalar());
    BinChanger::hexWrite(file, hexFloat);
}

template <class valueType>
QString taDataQuaternion<valueType>::backupDisplay(){
    QString displayValue;
    displayValue = QString::number(this->value.x(), 'g', 5) + ", " + QString::number(this->value.y(), 'g', 5) + ", "
            +QString::number(this->value.z(), 'g', 5) + ", "+QString::number(this->value.scalar(), 'g', 5);
    return displayValue;
}

template <class valueType>
QString taDataQuaternion<valueType>::databaseOutput(){
    QString outputValue;
    outputValue = QString::number(this->value.x()) + " " + QString::number(this->value.y()) + " "+QString::number(this->value.z()) + " "+QString::number(this->value.scalar()) + " ";
    return outputValue;
}

template <class valueType>
QQuaternion taDataQuaternion<valueType>::quatValue(){
    return this->value;
}

/*---------- taDataArray ----------*/

template <class valueType>
QStringList taDataArray<valueType>::options(){
    QStringList displayValue;
    for(int i = 0; i < this->values.size(); i++){
        displayValue.push_back(QVariant(values[i]).value<QString>());
    }
    //qDebug() << Q_FUNC_INFO << "generating display value for" << values << ":" << displayValue << ".";
    /*if(displayValue == ""){
        displayValue = backupDisplay();
    }*/
    return displayValue;
}

template <class valueType>
QString taDataArray<valueType>::definitionOutput(){
    QString outputValue = QString::number(this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        outputValue += " ";
        outputValue += QVariant(values[i]).value<QString>();
    }
    //qDebug() << Q_FUNC_INFO << "generating output value for" << values << ":" << outputValue << ".";
    outputValue += " \"" + this->comment + "\" ";
    return outputValue;
}

template <class valueType>
QString taDataArray<valueType>::databaseOutput(){
    QString outputValue = QString::number(this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        outputValue += " ";
        outputValue += QVariant(values[i]).value<QString>();
    }
    outputValue += " ";
    //qDebug() << Q_FUNC_INFO << "generating output value for" << values << ":" << outputValue << ".";
    return outputValue;
}

/*---------- IntArray ----------*/

template <>
std::shared_ptr<taData> taDataIntArray<int>::clone(){
    std::shared_ptr<taDataIntArray<int>> intArrayItem(new taDataIntArray<int>(*this));
    intArrayItem->values = values;
    return intArrayItem;
}

template <class valueType>
void taDataIntArray<valueType>::read(){
    int arrayCount = 0;
    if(this->file->binary){
        arrayCount = this->file->fileData->readInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->readInt());
        }
    } else {
        arrayCount = this->file->fileData->textWord().toInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->textWord().toInt());
        }
    }
}

template <class valueType>
void taDataIntArray<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        BinChanger::intWrite(file, this->values[i]);
    }
}

template <class valueType>
void taDataIntArray<valueType>::setValue(QString changedValue){
    this->values.clear();
    qDebug() << Q_FUNC_INFO << "Setting values to:" << changedValue;
    QStringList valueList = changedValue.split(", ");
    for(int i = 0; i < valueList.size(); i++){
        qDebug() << Q_FUNC_INFO << "value" << valueList[i] << "being converted to" << valueList[i].toInt();
        this->values.push_back(valueList[i].toInt());
    }
}

/*---------- FloatArray ----------*/

template <class valueType>
void taDataFloatArray<valueType>::read(){
    //All array types are handled the same in definition files
    int arrayCount = 0;
    if(this->file->binary){
        arrayCount = this->file->fileData->readInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->readFloat());
        }
    } else {
        arrayCount = this->file->fileData->textWord().toInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->textWord().toFloat());
        }
    }
}

template <class valueType>
void taDataFloatArray<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        QByteArray hexFloat = BinChanger::float_to_hex(this->values[i]);
        BinChanger::hexWrite(file, hexFloat);
    }
}

template <class valueType>
void taDataFloatArray<valueType>::setValue(QString changedValue){
    this->values.clear();
    QStringList valueList = changedValue.split(", ");
    for(int i = 0; i < valueList.size(); i++){
        this->values.push_back(valueList[i].toFloat());
    }
}

/*---------- LinkArray ----------*/

template <class valueType>
void taDataLinkArray<valueType>::read(){
    //qDebug() << Q_FUNC_INFO << "reading link array value from" << this->file->binary << "true = binary";
    int arrayCount = 0;
    if(this->file->binary){
        arrayCount = this->file->fileData->readInt();
        //qDebug() << Q_FUNC_INFO << "read link array count at" << this->file->fileData->currentPosition << "count=" << arrayCount;
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->readInt(2));
            //qDebug() << Q_FUNC_INFO << "read link array value at" << this->file->fileData->currentPosition;
        }
    } else {
        arrayCount = this->file->fileData->textWord().toInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->textWord().toInt());
        }
    }
}

template <class valueType>
void taDataLinkArray<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        BinChanger::shortWrite(file, this->values[i]);
    }
}

template <>
std::shared_ptr<taData> taDataLinkArray<uint16_t>::clone(){
    std::shared_ptr<taDataLinkArray<uint16_t>> linkArrayItem(new taDataLinkArray<uint16_t>(*this));
    linkArrayItem->values = values;
    //linkArrayItem->file = file;
    return linkArrayItem;
}

template <class valueType>
void taDataLinkArray<valueType>::setValue(QString changedValue){
    this->values.clear();
    QStringList valueList = changedValue.split(", ");
    for(int i = 0; i < valueList.size(); i++){
        this->values.push_back(valueList[i].toUInt());
    }
}

/*---------- StringArray ----------*/

template <>
std::shared_ptr<taData> taDataStringArray<QString>::clone(){
    std::shared_ptr<taDataStringArray<QString>> stringArrayItem(new taDataStringArray<QString>(*this));
    stringArrayItem->values = values;
    return stringArrayItem;
}

template <class valueType>
void taDataStringArray<valueType>::read(){
    int length = 0;
    int arrayCount = 0;
    if(this->file->binary){
        //pretty sure string arrays never occur in binary files
        //wouldn't be surprised if the engine can handle them though
        arrayCount = this->file->fileData->readInt();
        for(int i = 0; i < arrayCount; i++){
            length = this->file->fileData->readInt();
            this->values.push_back(this->file->fileData->readHex(length));
        }
    } else {
        arrayCount = this->file->fileData->textWord().toInt();
        for(int i = 0; i < arrayCount; i++){
            this->values.push_back(this->file->fileData->textWord());
        }
    }
}

template <class valueType>
void taDataStringArray<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->values.size());
    for(int i = 0; i < this->values.size(); i++){
        BinChanger::intWrite(file, this->values[i].length());
        BinChanger::hexWrite(file, this->values[i].toUtf8());
    }
}

template <class valueType>
void taDataStringArray<valueType>::setValue(QString changedValue){
    this->values.clear();
    QStringList valueList = changedValue.split(", ");
    for(int i = 0; i < valueList.size(); i++){
        this->values.push_back(valueList[i]);
    }
}

/*---------- VectorArray ----------*/

template <>
std::shared_ptr<taData> taDataVectorArray<QVector3D>::clone(){
    std::shared_ptr<taDataVectorArray<QVector3D>> vectorArrayItem(new taDataVectorArray<QVector3D>(*this));
    vectorArrayItem->values = values;
    return vectorArrayItem;
}

template <class valueType>
void taDataVectorArray<valueType>::read(){
    int arrayCount = 0;
    if(this->file->binary){
        arrayCount = this->file->fileData->readInt();
        for(int i = 0; i < arrayCount; i++){
            float x = this->file->fileData->readFloat();
            float y = this->file->fileData->readFloat();
            float z = this->file->fileData->readFloat();
            this->values.push_back(QVector3D(x,y,z));
        }
    } else {
        arrayCount = this->file->fileData->textWord().toInt();
        for(int i = 0; i < arrayCount; i++){
            float x = this->file->fileData->textWord().toFloat();
            float y = this->file->fileData->textWord().toFloat();
            float z = this->file->fileData->textWord().toFloat();
            this->values.push_back(QVector3D(x,y,z));
        }
    }
}

template <class valueType>
void taDataVectorArray<valueType>::write(QFile& file){
    BinChanger::intWrite(file, this->values.size());
    QByteArray hexFloat;
    for(int i = 0; i < this->values.size(); i++){
        hexFloat = BinChanger::float_to_hex(this->values[i].x());
        BinChanger::hexWrite(file, hexFloat);
        hexFloat = BinChanger::float_to_hex(this->values[i].y());
        BinChanger::hexWrite(file, hexFloat);
        hexFloat = BinChanger::float_to_hex(this->values[i].z());
        BinChanger::hexWrite(file, hexFloat);
    }
}

template <class valueType>
QString taDataVectorArray<valueType>::backupDisplay(){
    QString displayValue;
    for(int i = 0; i < this->values.size(); i++){
        displayValue = QString::number(this->values[i].x()) + ", " + QString::number(this->values[i].y()) + ", "+QString::number(this->values[i].z());

    }
    return displayValue;
}

/*---------- Enum ----------*/

QStringList taDataEnum::options(){
    return valueOptions;
}

void taDataEnum::addOption(QString optionToAdd){
    valueOptions.push_back(optionToAdd);
}

QString taDataEnum::display(){
    //qDebug() << Q_FUNC_INFO << "Generating display value from enum using:" << defaultValue << "with" << valueOptions.size() << "options";
    return valueOptions[defaultValue];
}

void taDataEnum::setValue(QString changedValue){
    this->isDefault = false;
    if(valueOptions.contains(changedValue)){
        this->defaultValue = valueOptions.indexOf(changedValue);
    } else {
        this->defaultValue = changedValue.toInt();
    }
}

std::shared_ptr<taData> taDataEnum::clone(){
    //qDebug() << Q_FUNC_INFO << "Cloning item of type" << type << "with default value" << defaultValue;
    std::shared_ptr<taDataEnum> stringItem(new taDataEnum(*this));
    stringItem->defaultValue = defaultValue;
    stringItem->valueOptions = valueOptions;
    return stringItem;
}

QString taDataEnum::definitionOutput(){
    QString totalOutput = "\"" + valueOptions[defaultValue] + "\" \"";
    if(this->comment != ""){
        totalOutput += this->comment + " ";
    }
    totalOutput += "[" + valueOptions.join(",") + "]\" ";
    return totalOutput;
}

QString taDataEnum::databaseOutput(){
    return QString::number(this->defaultValue) + " ";
}

void taDataEnum::read(){
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    QString tempList;
    QString value;
    if(file->binary){
        //enums are only present in binary database files, not definition files
        //this means that we only need to get the index of the value
        //qDebug() << Q_FUNC_INFO << "reading binary default at" << file->fileData->currentPosition;
        defaultValue = file->fileData->readInt();
        //qDebug() << Q_FUNC_INFO << "binary default value read as" << defaultValue << "at" << file->fileData->currentPosition;
    } else {
        value = file->fileData->textWord().remove(quoteRemover);
        tempList = file->fileData->textWord();
        if(tempList == ""){
            //textWord will return "" if it attempts to get a word at the end of a line.
            defaultValue = value.toInt();
        }
        else {
            if (tempList.contains("Range")){
                tempList = file->fileData->textWord();
            }
            tempList = tempList.remove(quoteRemover);
            valueOptions = tempList.split(",");
            //qDebug() << Q_FUNC_INFO << "Setting default value of enum to" << value << "found at index" << valueOptions.indexOf(value);
            defaultValue = valueOptions.indexOf(value);
        }
    }
}

void taDataEnum::write(QFile& file){
    //this->file->parent->log("Enum values cannot be written to binary Definition (BMD) files. Incompatible item: " + this->name);
    BinChanger::intWrite(file, this->defaultValue);
}

std::shared_ptr<taData> dictItem::editAttributeValue(QString itemType, std::shared_ptr<taData> itemToEdit){
    bool isDialogOpen = true;
    CustomPopup* dialogGetValue = nullptr;

    QStringList listWindows = {"StringArray", "IntegerArray", "FloatArray", "LinkArray"};

    if(itemType == "Bool"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"checkbox"}, {""});
        dialogGetValue->checkOption->setText("Set value to true?");
    } else if(itemType == "String"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"lineedit"}, {"Value:"});
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->lineOption->setFocus();});
    } else if(itemType == "Float"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"lineedit"}, {"Value:"});
        /*Need to find the actual maximum and minimum*/
        QValidator *validator = new QDoubleValidator(-4000, 4000, 3, dialogGetValue);
        dialogGetValue->lineOption->setValidator(validator);
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->lineOption->setFocus();});
    } else if(itemType == "Quaternion"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"boxset", "4"}, {"x:", "y:", "z:", "scalar:"});
        /*Need to find the actual maximum and minimum*/
        QValidator *validator = new QDoubleValidator(-4000, 4000, 3, dialogGetValue);
        for(int i = 0; i < dialogGetValue->boxList.size(); i++){
            dialogGetValue->boxList[i]->setValidator(validator);
        }
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->boxList[0]->setFocus();});
    } else if(itemType == "Integer"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"lineedit"}, {"Value:"});
        /*Need to find the actual maximum and minimum*/
        QValidator *validator = new QIntValidator(-4000, 4000, dialogGetValue);
        dialogGetValue->lineOption->setValidator(validator);
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->lineOption->setFocus();});
    } else if(itemType == "Link" || itemType == "Flag"){
        /*Currently only limits to short, so this accommodates both Link and Flag. Later, having link pull from
         the list of available Instances would be convenient*/
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"lineedit"}, {"Value:"});
        QValidator *validator = new QIntValidator(0, 65535, dialogGetValue);
        dialogGetValue->lineOption->setValidator(validator);
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->lineOption->setFocus();});
    } /*else if(itemType == "Flag"){
        dialogGetValue = ProgWindow::makeSpecificPopup(isDialogOpen, {"lineedit"}, {"Value:"});
        QValidator *validator = new QIntValidator(0, 65535, dialogGetValue);
        dialogGetValue->lineOption->setValidator(validator);
    }*/else if(itemType == "Point"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"boxset", "3"}, {"x:", "y:", "z:"});
        /*Need to find the actual maximum and minimum*/
        QValidator *validator = new QDoubleValidator(-4000, 4000, 3, dialogGetValue);
        for(int i = 0; i < dialogGetValue->boxList.size(); i++){
            dialogGetValue->boxList[i]->setValidator(validator);
        }
        //dialogGetValue->setFocusProxy(dialogGetValue->boxList[0]); //this does nothing, not sure why
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->boxList[0]->setFocus();});
    } else if(itemType == "Enum"){
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"button","combobox"}, {"Add option", "Value options"});
        for(int i = 0; i < itemToEdit->options().size(); i++){
            dialogGetValue->comboOption->addItem(itemToEdit->options()[i]);
        }
        CustomPopup::connect(dialogGetValue->buttonOption, &QPushButton::released, [&itemToEdit, &dialogGetValue](){
            editEnumDefinition(itemToEdit);
            dialogGetValue->comboOption->clear();
            for(int i = 0; i < itemToEdit->options().size(); i++){
                dialogGetValue->comboOption->addItem(itemToEdit->options()[i]);
            }
        });
    } else if(listWindows.contains(itemType)){
        //Array values will be unsupported on the initial update.
        //need to look into the different options and see which one will be easiest for these
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"list"}, {itemType + " values:"});
        CustomPopup::connect(dialogGetValue->listOption->itemDelegate(), &QAbstractItemDelegate::commitData, dialogGetValue, [dialogGetValue]() {
            if(dialogGetValue->listOption->itemAt(0, dialogGetValue->listOption->count())->text() != ""){
                dialogGetValue->addBlankItem();
            }
        });
        dialogGetValue->addBlankItem();
    } /*else if(itemType == "VectorArray"){
    }*/ else if(itemType == "Color"){
        //there is a color select dialog - implement this at some point
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"boxset", "4"}, {"r:", "g:", "b:", "a:"});
        QValidator *validator = new QIntValidator(0, 255, dialogGetValue);
        for(int i = 0; i < dialogGetValue->boxList.size(); i++){
            dialogGetValue->boxList[i]->setValidator(validator);
        }
        QTimer::singleShot(0, dialogGetValue, [dialogGetValue]{dialogGetValue->boxList[0]->setFocus();});
    } else {
        qDebug() << Q_FUNC_INFO << "Data type" << itemType << "hasn't been implemented yet.";
        return itemToEdit;
    }

    if(dialogGetValue == nullptr){
        return nullptr;
    }

    dialogGetValue->setWindowTitle("Set Attribute Value");

    qDebug() << Q_FUNC_INFO << "making window for item type:" << itemType;

    dialogGetValue->open();
    while(isDialogOpen){
        exWindowBase::ForceProcessEvents();
    }
    int resultDialog = dialogGetValue->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return itemToEdit;
    }

    if(itemType == "Bool"){
        if(dialogGetValue->checkOption->checkState()){
            itemToEdit->setValue("True");
        } else {
            itemToEdit->setValue("False");
        }
    } else if(itemType == "String"){
        itemToEdit->setValue(dialogGetValue->lineOption->text());
    } else if(itemType == "Float"){
        itemToEdit->setValue(dialogGetValue->lineOption->text());
    } else if(itemType == "Quaternion"){
        QString finalValue;
        finalValue = dialogGetValue->boxList[0]->text() + ", ";
        finalValue += dialogGetValue->boxList[1]->text() + ", ";
        finalValue += dialogGetValue->boxList[2]->text() + ", ";
        finalValue += dialogGetValue->boxList[3]->text();
        itemToEdit->setValue(finalValue);
    } else if(itemType == "Integer"){
        itemToEdit->setValue(dialogGetValue->lineOption->text());
    } else if(itemType == "Link"){
        itemToEdit->setValue(dialogGetValue->lineOption->text());
    } else if(itemType == "Flag"){
        itemToEdit->setValue(dialogGetValue->lineOption->text());
    } else if(itemType == "Point"){
        QString finalValue;
        finalValue = dialogGetValue->boxList[0]->text() + ", ";
        finalValue += dialogGetValue->boxList[1]->text() + ", ";
        finalValue += dialogGetValue->boxList[2]->text();
        itemToEdit->setValue(finalValue);
    } else if(itemType == "Enum"){
        itemToEdit->setValue(dialogGetValue->comboOption->currentText());
    } else if(itemType == "StringArray"){
        QStringList finalValues;
        for(int i = 0; i < dialogGetValue->listOption->count()-1; i++){
            finalValues.push_back(dialogGetValue->listOption->item(i)->text());
        }
        itemToEdit->setValue(finalValues.join(", "));
    } else if(itemType == "IntegerArray"){
        QStringList finalValues;
        for(int i = 0; i < dialogGetValue->listOption->count()-1; i++){
            finalValues.push_back(dialogGetValue->listOption->item(i)->text());
        }
        itemToEdit->setValue(finalValues.join(", "));
    } else if(itemType == "FloatArray"){
        QStringList finalValues;
        for(int i = 0; i < dialogGetValue->listOption->count()-1; i++){
            finalValues.push_back(dialogGetValue->listOption->item(i)->text());
        }
        itemToEdit->setValue(finalValues.join(", "));
    } else if(itemType == "VectorArray"){
    } else if(itemType == "LinkArray"){
        QStringList finalValues;
        for(int i = 0; i < dialogGetValue->listOption->count()-1; i++){
            finalValues.push_back(dialogGetValue->listOption->item(i)->text());
        }
        itemToEdit->setValue(finalValues.join(", "));
    } else if(itemType == "Color"){
        //there is a color select dialog - implement this at some point
        QString finalValue;
        finalValue = dialogGetValue->boxList[0]->text() + ",";
        finalValue += dialogGetValue->boxList[1]->text() + ",";
        finalValue += dialogGetValue->boxList[2]->text() + ",";
        finalValue += dialogGetValue->boxList[3]->text();
        itemToEdit->setValue(finalValue);
    } else {
        qDebug() << Q_FUNC_INFO << "Data type" << itemType << "hasn't been implemented yet.";
        return itemToEdit;
    }

    return itemToEdit;
}

std::shared_ptr<taData> dictItem::editEnumDefinition(std::shared_ptr<taData> itemToEdit){
    bool isDialogOpen = true;
    CustomPopup* dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"combobox"}, {""});
    dialogGetValue->setWindowTitle("Select edit type.");
    QStringList editOptions = {"Change default value", "Add new value"};

    for(int i = 0; i < editOptions.size(); i++){
        dialogGetValue->comboOption->addItem(editOptions[i]);
    }

    dialogGetValue->open();
    while(isDialogOpen){
        exWindowBase::ForceProcessEvents();
    }
    int resultDialog = dialogGetValue->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return itemToEdit;
    }

    int selectedEdit = editOptions.indexOf(dialogGetValue->comboOption->currentText());

    switch(selectedEdit){
    case 0: //change value
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"combobox"}, {"Value options:"});
        for(int i = 0; i < itemToEdit->options().size(); i++){
            dialogGetValue->comboOption->addItem(itemToEdit->options()[i]);
        }
        dialogGetValue->setWindowTitle("Select value");
        break;
    case 1: //add value
        dialogGetValue = exWindowBase::MakeSpecificPopup(isDialogOpen, {"lineedit"}, {"New value:"});
        dialogGetValue->setWindowTitle("Add value");
        break;
    default: //unknown option
        qDebug() << Q_FUNC_INFO << "Unknown option selected";
        break;
    }


    isDialogOpen = true;
    dialogGetValue->open();
    while(isDialogOpen){
        exWindowBase::ForceProcessEvents();
    }
    resultDialog = dialogGetValue->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return itemToEdit;
    }

    switch(selectedEdit){
    case 0: //change value
        itemToEdit->setValue(dialogGetValue->comboOption->currentText());
        itemToEdit->isDefault = true;
        break;
    case 1: //add value
        itemToEdit->addOption(dialogGetValue->lineOption->text());
        break;
    default: //unknown option
        qDebug() << Q_FUNC_INFO << "Unknown option selected";
        break;
    }

    return itemToEdit;
}

int dictItem::addAttribute(QString itemType){
    //make popup window with all options
    /*for an attribute definition, we need:
        type
        "False"
        Name
        Value
        Available values or range
    */

    /*easiest way to handle this is probably an initial popup that has all of the
    available data types, then a second popup that's defined by the type chosen*/

    //Those are all the ones we should need for now. The rest can be added later.
    QStringList valueTypes = {"Cancel", "Enum", "Float", "Point", "String", "Bool", "Integer", "Link", "Quaternion", "Flag"
                              , "StringArray", "IntegerArray", "FloatArray", "LinkArray", "Color"};

    bool isDialogOpen = true;

    CustomPopup* dialogCreateAttribute = exWindowBase::MakeSpecificPopup(isDialogOpen, {"combobox", "lineedit"}, {"Attribute type:", "Attribute Name:"});
    dialogCreateAttribute->setWindowTitle("Create New Attribute");

    for(int i = 0; i < valueTypes.size(); i++){
        dialogCreateAttribute->comboOption->insertItem(i, valueTypes[i]);
    }

    dialogCreateAttribute->open();
    while(isDialogOpen){
        exWindowBase::ForceProcessEvents();
    }
    int resultDialog = dialogCreateAttribute->result();

    if(resultDialog == 0 || dialogCreateAttribute->comboOption->currentIndex() == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return 1;
    }

    itemType = dialogCreateAttribute->comboOption->currentText();

    std::shared_ptr<taData> genericData = nullptr;

    /*I hate needing this ifelse structure twice, but idk how else to handle it.*/
    if(itemType == "Bool"){
        std::shared_ptr<taDataBool<bool>> boolItem(new taDataBool<bool>);
        genericData = boolItem;
    } else if(itemType == "String"){
        std::shared_ptr<taDataString<QString>> stringItem(new taDataString<QString>);
        genericData = stringItem;
    } else if(itemType == "Float"){
        std::shared_ptr<taDataFloat<float>> floatItem(new taDataFloat<float>);
        genericData = floatItem;
    } else if(itemType == "Quaternion"){
        std::shared_ptr<taDataQuaternion<QQuaternion>> quatItem(new taDataQuaternion<QQuaternion>);
        genericData = quatItem;
    } else if(itemType == "Integer"){
        std::shared_ptr<taDataInteger<int>> intItem(new taDataInteger<int>);
        genericData = intItem;
    } else if(itemType == "Link"){
        std::shared_ptr<taDataLink<uint16_t>> linkItem(new taDataLink<uint16_t>);
        genericData = linkItem;
    } else if(itemType == "Flag"){
        std::shared_ptr<taDataFlag<int>> flagItem(new taDataFlag<int>);
        genericData = flagItem;
    } else if(itemType == "Point"){
        std::shared_ptr<taDataPoint<QVector3D>> pointItem(new taDataPoint<QVector3D>);
        genericData = pointItem;
    } else if(itemType == "Enum"){
        std::shared_ptr<taDataEnum> enumItem(new taDataEnum);
        genericData = enumItem;
    } else if(itemType == "StringArray"){
        std::shared_ptr<taDataStringArray<QString>> stringArrayItem(new taDataStringArray<QString>);
        genericData = stringArrayItem;
    } else if(itemType == "IntegerArray"){
        std::shared_ptr<taDataIntArray<int>> intArrayItem(new taDataIntArray<int>);
        genericData = intArrayItem;
    } else if(itemType == "FloatArray"){
        std::shared_ptr<taDataFloatArray<float>> floatArrayItem(new taDataFloatArray<float>);
        genericData = floatArrayItem;
    } else if(itemType == "VectorArray"){
        std::shared_ptr<taDataVectorArray<QVector3D>> vectorArrayItem(new taDataVectorArray<QVector3D>);
        genericData = vectorArrayItem;
    } else if(itemType == "LinkArray"){
        std::shared_ptr<taDataLinkArray<uint16_t>> linkArrayItem(new taDataLinkArray<uint16_t>);
        genericData = linkArrayItem;
    } else if(itemType == "Color"){
        std::shared_ptr<taDataColor<QColor>> colorItem(new taDataColor<QColor>);
        //there is a color select dialog - implement this at some point
        genericData = colorItem;
    } else {
        qDebug() << Q_FUNC_INFO << "Data type" << itemType << "hasn't been implemented yet.";
        return 1;
    }

    genericData = editAttributeValue(itemType, genericData);

    genericData->type = itemType;
    genericData->name = dialogCreateAttribute->lineOption->text();
    genericData->inherited = false;
    genericData->active = false;

    qDebug() << Q_FUNC_INFO << "data value set to:" << genericData->display();

    attributes.push_back(genericData);

    return 0;
}
