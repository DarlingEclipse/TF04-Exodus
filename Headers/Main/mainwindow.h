#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>
#include <QIODevice>
#include <QComboBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QRadioButton>
#include <QCheckBox>
#include <QHeaderView>
#include <QQuaternion>
#include <QLabel>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QMenu>
#include <QMenuBar>
#include <QScreen>
#include <QDockWidget>
#include <QImageWriter>
#include <QGroupBox>
#include <QProgressBar>

#include <winsparkle.h>
#include <vector>

#include "Headers/Models/vbin.h"
#include "Headers/Textures/itf.h"
#include "Headers/Models/Mesh.h"
#include "BinChanger.h"
#include "CustomQT.h"
#include "Headers/ISOManager/IsoBuilder.h"
#include "Headers/ISOManager/ModHandler.h"
#include "Headers/Models/LevelGeo.h"
#include "Headers/Databases/Database.h"
#include "Headers/Databases/DataHandler.h"
#include "Headers/Audio/ToneLibraries.h"
#include "Headers/Databases/DistanceCalculator.h"
#include "Headers/Randomizer/Randomizer.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class SettingsWindow : public QWidget {
    Q_OBJECT

public:
    explicit SettingsWindow(ProgWindow *sentParent = nullptr);
    ~SettingsWindow();
    ProgWindow *parent;
    QStringList settingsNames = {"Starting window height","Starting window width","Address offset","Starting address","Long scroll length","Short scroll length"
                                ,"Game extract path","Modded game path","ImgBurn EXE path", "7Zip EXE path"};
    QStringList defaultSettingsValues = {"1024","768","0","0","16","4", "", "", "", ""};
    QStringList settingsValues;
    QTableWidget *settingsEdit;

    void open();
    void writeSettings(QFile *outputFile);
    void changeSetting(int row, int column);
    QString getValue(QString settingName);
    void setValue(QString settingName, QString settingValue);
    void loadSettings();

private slots:
    void updateSettings();
private:
    QPushButton *sendUpdate;
    bool savedChanges;
};

class CustomPopup : public QDialog{
public:
    QCheckBox* checkOption;
    QComboBox* comboOption;
    QLineEdit* lineOption;
    QTextEdit* multiOption;
    QListWidget* listOption;
    std::vector<QLineEdit*> boxList;

    void addBlankItem();
};

class ProgWindow : public QMainWindow
{
    Q_OBJECT

public:
    ProgWindow(QWidget *parent = nullptr);
    ~ProgWindow();

    BinChanger binChanger;
    VACFile* vacFile;
    DistanceCalculator* warpgateCalculator;
    Randomizer* randomizer;
    IsoBuilder* isoBuilder;
    ModHandler* modHandler;
    DataHandler* dataHandler;
    SettingsWindow *setW;
    std::vector<std::shared_ptr<TFFile>> loadedFiles;
    std::vector<std::shared_ptr<DatabaseFile>> databaseList;
    QStringList loadedFileNames;
    QListWidget* fileBrowser;
    int hSize = 1200;
    int vSize = 768;
    double version = 6.6;
    QString gamePath = "";

    QString fileMode;

    Ui::MainWindow *ui;
    QLabel *ClosestWarpgate;

    std::vector<QWidget*> currentModeWidgets;
    QDockWidget *leftSidebar;
    QListWidget* logPrintout;

    QWidget *centralContainer;
    //QPushButton *ButtonRemoveItem;
    QPushButton *ButtonCalculate;
    QLineEdit *CalculateXValue;
    QLineEdit *CalculateYValue;
    QLineEdit *CalculateZValue;
    QLineEdit *DBNewValue;
    QComboBox *ListAnimation;
    QComboBox *ListFrame;
    FileData fileData;
    QMessageBox *MessagePopup;
    int numColors;
    QString whichModel;
    std::vector<std::vector<std::vector<int>>> lodArrays;
    std::vector<int> usedIndecies;
    QPixmap background;
    QPalette palette;
    QProgressBar *loadingBar;


    void messageError(QString message);
    void messageSuccess(QString message);
    void log(QString message);

    static QDialog* makePopup(bool &finished);
    static CustomPopup* makeSpecificPopup(bool &finished, QStringList addons, QStringList labels);
    static QPoint screenCenter();
    static void forceProcessEvents();

    void clearWindow();
    void clearFiles();
    void clearLog();

    std::shared_ptr<TFFile> matchFile(QString fileNameFull);

    //visitor functions - used for getting pointers to database files for DatabaseCalculator
    //I would really _really_ like to find a better way to handle this, but I'm at my limit here
    void visit(TFFile dataFile);
    void visit(DatabaseFile dataFile);

    void resizeEvent(QResizeEvent* event);
    void openWarpgateCalculator();
    void openModHandler();
    void openRandomizer();
    void updateBackground();
    void updateCenter();
    void saveFile(QString fromType, QString givenPath = "");
    void bulkSave(QString category);
    void handleSettings();
    void updateLoadingBar(int currentValue, int maxValue);
    void updateLoadingBar(); //increments the progress bar by 1
    void updateLoadingBar(int currentValue); //sets progress to currentvalue

    void bulkOpen(QString fileType);
    template <typename theFile>
    void loadBulkFile(theFile fileToOpen);

    int loadDatabases();
    int createDefinitionFile();
    int createDatabaseFile();

    template <typename theFile>
    void loadFile(theFile fileToOpen, QString givenPath = "");
    void openFile(QString fileType, QString givenPath = "");

};


#endif // MAINWINDOW_H
