#include "Headers/UI/exSettings.h"
#include "qcoreapplication.h"

exSettings::exSettings(QWidget *sentParent){

    m_savedChanges = true;
    setWindowTitle("Exodus Settings");
    m_sendUpdate = new QPushButton("Save Settings", this);
    m_sendUpdate -> setGeometry(QRect(QPoint(25,25), QSize(200,25)));

    connect(m_sendUpdate, &QPushButton::released, this, &exSettings::UpdateSettings);
}

void exSettings::UpdateSettings(){
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString fileOut = QCoreApplication::applicationDirPath() + "/VBINsettings.txt";
    QFile outputFile(fileOut);

    if (!m_savedChanges){
        qDebug() << Q_FUNC_INFO << "Settings changed";
        outputFile.open(QIODevice::WriteOnly);
        for(int i = 0; i < m_settingsNames.length(); i++){
            if(i!= 0){
                outputFile.write("\n");
            }
            outputFile.write(m_settingsNames[i].toUtf8()+": " + m_settingsValues[i].toUtf8());
        }
        m_savedChanges = true;
    }

    qDebug() << Q_FUNC_INFO << "Settings updated";
}

void exSettings::WriteSettings(QFile *outputFile){
    qDebug() << Q_FUNC_INFO << "Settings file not found. Creating a default settings file.";
    outputFile->open(QIODevice::WriteOnly);
    for(int i = 0; i < m_settingsNames.length(); i++){
        if(i!= 0){
            outputFile->write("\n");
        }
        outputFile->write(m_settingsNames[i].toUtf8()+": " + m_defaultSettingsValues[i].toUtf8());
    }
}

QString exSettings::GetValue(QString settingName){
    QString foundSetting = "";
    LoadSettings();
    //qDebug() << Q_FUNC_INFO << "Searching for" << settingName;
    //qDebug() << Q_FUNC_INFO << "names list contains:" << settingsNames.size() << settingsNames;
    //qDebug() << Q_FUNC_INFO << "values list contains:" << settingsValues.size() << settingsValues;
    for(int i = 0; i < m_settingsNames.size(); i++){
        if(m_settingsNames[i] == settingName){
            foundSetting = m_settingsValues[i];
        }
    }
    return foundSetting;
}

void exSettings::SetValue(QString settingName, QString settingValue){
    LoadSettings();
    //qDebug() << Q_FUNC_INFO << "Searching for" << settingName;
    //qDebug() << Q_FUNC_INFO << "names list contains:" << settingsNames.size() << settingsNames;
    //qDebug() << Q_FUNC_INFO << "values list contains:" << settingsValues.size() << settingsValues;
    for(int i = 0; i < m_settingsNames.size(); i++){
        if(m_settingsNames[i] == settingName){
            //TODO: update this line to new logging system (when it exists)
            //parent->log("Setting changed: " + settingName + " is now set to " + settingValue);
            m_settingsValues[i] = settingValue;
            m_savedChanges = false;
            UpdateSettings();
        }
    }
}

void exSettings::LoadSettings(){
    qDebug() << Q_FUNC_INFO << QCoreApplication::applicationDirPath();
    QString fileIn = QCoreApplication::applicationDirPath() + "/VBINsettings.txt";
    QString settingsRead;
    QStringList settingsSplit;
    QFile inputFile(fileIn);
    if (inputFile.exists()){
        inputFile.open(QIODevice::ReadOnly);
        settingsRead = inputFile.readAll();
        settingsSplit = settingsRead.split("\n");
        m_settingsValues.clear();
        if(settingsSplit.size() != m_settingsNames.size()){
            m_settingsValues = m_defaultSettingsValues;
            WriteSettings(&inputFile);
        } else {
            for (int i = settingsSplit.length()-1; i >= 0; i--){
                //qDebug() << Q_FUNC_INFO << settingsSplit[i];
                if (settingsSplit[i] != ""){
                    settingsSplit[i] = settingsSplit[i].right(settingsSplit[i].length() - (settingsSplit[i].indexOf(":")+1));
                    //setW->settingsValues.push_front(settingsSplit[i].split(":")[1].trimmed());
                    m_settingsValues.push_front(settingsSplit[i].trimmed());
                }
            }
        }
    } else {
        //settings file doesn't exist, write a file with the default settings
        m_settingsValues = m_defaultSettingsValues;
        WriteSettings(&inputFile);
    }
}

void exSettings::Open(){
    m_settingsEdit = new QTableWidget(m_settingsNames.size(), 2, this);
    m_settingsEdit->setGeometry(QRect(QPoint(50,50), QSize(300,300)));
    LoadSettings();
    for (int i =0; i < m_settingsNames.size();i++) {
        QTableWidgetItem *nextSetName = m_settingsEdit->item(i ,0);
        QTableWidgetItem *nextSetValue = m_settingsEdit->item(i ,1);
        qDebug() << Q_FUNC_INFO << m_settingsNames[i] << ": " << m_settingsValues[i];
        if(!nextSetName){
            nextSetName = new QTableWidgetItem;
            nextSetName->setFlags(Qt::ItemIsEditable);
            m_settingsEdit->setItem(i ,0, nextSetName);
        }
        nextSetName->setText(m_settingsNames[i]);
        if(!nextSetValue){
            nextSetValue = new QTableWidgetItem;
            m_settingsEdit->setItem(i ,1, nextSetValue);
        }
        nextSetValue->setText(m_settingsValues[i]);
    }
    connect(m_settingsEdit, &QTableWidget::cellChanged, this, [this](int row, int column) {ChangeSetting(row, column);});

    show();
}

void exSettings::ChangeSetting(int row, int column){
    m_savedChanges = false;
    m_settingsValues[row] = m_settingsEdit->item(row,1)->text();
}

exSettings::~exSettings()
{
    delete m_sendUpdate;
}
