#ifndef EXSETTINGS_H
#define EXSETTINGS_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QFile>

/* 2/28/25 - isolated from mainwindow.h as part of The Great Refactoring (Part 1)
 * This class exists to load settings and provide an interface for the user to edit those settings
 * I think this class still extends a bit beyond its necessary scope and could be split up, but that'll be a problem for another day
 * My thinking is that the loading of the settings should be separate from the window
 * In reality, the editor for the settings won't be much different from the editor for the game's txt files*/

class exSettings : public QWidget {
    Q_OBJECT

public:
    explicit exSettings(QWidget *sentParent = nullptr);
    ~exSettings();
    const QStringList m_settingsNames = {"Starting window height","Starting window width","Address offset","Starting address","Long scroll length","Short scroll length"
                                ,"Game extract path","Modded game path","ImgBurn EXE path", "7Zip EXE path"};
    const QStringList m_defaultSettingsValues = {"1024","768","0","0","16","4", "", "", "", ""};
    QStringList m_settingsValues;
    QTableWidget *m_settingsEdit;

    void Open();
    void WriteSettings(QFile *outputFile);
    void ChangeSetting(int row, int column);
    QString GetValue(QString settingName);
    void SetValue(QString settingName, QString settingValue);
    void LoadSettings();

private slots:
    void UpdateSettings();
private:
    QPushButton *m_sendUpdate;
    bool m_savedChanges;
};

#endif // EXSETTINGS_H
