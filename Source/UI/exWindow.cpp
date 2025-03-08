#include "Headers/UI/exWindow.h"
#include "Headers/UI/exSettings.h"
#include "ui_mainwindow.h"
#include <QDockWidget>
#include <QPushButton>
#include <QScreen>
#include <QLineEdit>

exWindow* exWindow::s_instance = nullptr;

exWindow::exWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainWindow)
{
    s_instance = this;
    m_ui->setupUi(this);
    m_setW = new exSettings(this);
    //this->setPalette(palette);

    m_messagePopup = new QMessageBox(this);
    m_messagePopup->setGeometry(QRect(QPoint(int(m_hSize*0.5),int(m_vSize*0.5)), QSize(120,30)));
    menuBar()->setGeometry(QRect(QPoint(int(m_hSize*0),int(m_vSize*0)), QSize(int(m_hSize*1),25)));

    m_centralContainer = new QWidget(this);
    setCentralWidget(m_centralContainer);
    m_centralContainer->setGeometry(QRect(QPoint(m_hSize*0.15,0), QSize(m_hSize*0.6,m_vSize)));
    UpdateBackground();

    setWindowTitle("Exodus 0.7.4.0");
    setGeometry(QRect(QPoint(0,0), QSize(m_hSize,m_vSize)));
    //w.resize(1200, 600);


    QMenu *menuVBIN = menuBar()->addMenu("Model");
    QMenu *menuITF = menuBar()->addMenu("Texture");
    QMenu *menuSFX = menuBar()->addMenu("Sound");
    QMenu *menuDatabase = menuBar() -> addMenu("Database");
    QMenu *menuCalculator = menuBar()->addMenu("Calculator");
    QMenu *menuRandomizer = menuBar()->addMenu("Randomizer");
    QMenu *menuBuild = menuBar()->addMenu("Build");
    QMenu *menuClear = menuBar()->addMenu("Clear");
    QMenu *menuSettings = menuBar()->addMenu("Settings");

    QAction *actionLoadVBIN = menuVBIN->addAction("Load VBIN");
    QAction *actionLoadMeshVBIN = menuVBIN->addAction("Load Mesh VBIN");
    QAction *actionSaveModel = menuVBIN ->addAction("Export Model");
    QAction *actionBulkLoadModel = menuVBIN->addAction("Bulk Load Model");
    QAction *actionBulkSaveModel = menuVBIN->addAction("Bulk Export Model");

    QAction *actionLoadITF = menuITF ->addAction("Load ITF");
    QAction *actionLoadQImage = menuITF ->addAction("Import Image");
    QAction *actionSaveITF = menuITF->addAction("Export Texture");
    QAction *actionBulkLoadITF = menuITF->addAction("Bulk Load Texture");
    QAction *actionBulkSaveITF = menuITF->addAction("Bulk Export Texture");

    QAction *actionLoadVAC = menuSFX ->addAction("Load VAC");
    QAction *actionSaveVAC = menuSFX->addAction("Export VAC");
    QAction *actionBulkVAC = menuSFX->addAction("Bulk Export Sound");

    QAction *actionCreateDefinition = menuDatabase->addAction("Create Definition");
    QAction *actionCreateDatabase = menuDatabase->addAction("Create Database");
    QAction *actionLoadTMD = menuDatabase ->addAction("Load TMD");
    QAction *actionLoadTDB = menuDatabase ->addAction("Load TDB");
    QAction *actionLoadBMD = menuDatabase ->addAction("Load BMD");
    QAction *actionLoadBDB = menuDatabase ->addAction("Load BDB");
    QAction *actionSaveDatabase = menuDatabase ->addAction("Export Database");
    QAction *actionBulkDatabase = menuDatabase->addAction("Bulk Export Database");
//    QAction *actionSaveTDB = menuDatabase ->addAction("Save TDB");
//    QAction *actionSaveBMD = menuDatabase ->addAction("Save BMD");
//    QAction *actionSaveBDB = menuDatabase ->addAction("Save BDB");

    QAction *actionOpenCalculator = menuCalculator -> addAction("Warpgate Distance Calculator");
    QAction *actionRandomizer = menuRandomizer -> addAction("Load Randomizer");

    QAction *actionUnpackISO = menuBuild->addAction("Unpack ISO");
    QAction *actionUnzipZips = menuBuild->addAction("Unpack special ZIPs");
    QAction *actionModInterface = menuBuild->addAction("Mods and Replacements");
    QAction *actionZipBuildISO = menuBuild->addAction("Zip and Build ISO");
    QAction *actionBuildISO = menuBuild->addAction("Build ISO");
    QAction *actionPackRandom = menuBuild->addAction("Build Randomizer ISO");

    QAction *actionClearFiles = menuClear->addAction("Clear Loaded Files");

    QAction *actionSettings = menuSettings -> addAction("Settings");

    //rightSidebar = new QDockWidget(this);
    //addDockWidget(Qt::RightDockWidgetArea, rightSidebar);
    //rightSidebar->setFloating(false);
    //rightSidebar->show();

    /*QDockWidget *bottomLogbar = new QDockWidget(this);
    addDockWidget(Qt::BottomDockWidgetArea, bottomLogbar);
    bottomLogbar->setFloating(false);*/


    /*hiding SFX menu for this patch since this system is far from ready*/
    //menuSFX->setVisible(false);

    m_loadingBar = nullptr;

    connect(actionSettings, &QAction::triggered, this, &exWindow::HandleSettings);

    setWindowState(Qt::WindowMaximized);
    /*time_t lastCheck = win_sparkle_get_last_check_time();
    time(&lastCheck);
    messageSuccess("Last update check occurred at " + QString(ctime(&lastCheck)));*/

    show();
}

exWindow::~exWindow()
{
    delete m_ui;
}

QMenu* exWindow::AddMenu(QString name){
    return menuBar()->addMenu(name);
}

QAction* exWindow::AddAction(QMenu* targetMenu, QString name){
    return targetMenu->addAction(name);
}

void exWindow::UpdateLoadingBar(int currentValue, int maxValue){
    if(m_loadingBar == nullptr){
        m_loadingBar = new QProgressBar(m_centralContainer);
        m_loadingBar->setOrientation(Qt::Horizontal);
        m_loadingBar->setGeometry(QRect(m_centralContainer->width()-200,m_centralContainer->height()-30,200, 30));
        m_loadingBar->setRange(0,maxValue);
        m_loadingBar->show();
    }
    m_loadingBar->setValue(currentValue);
    if(currentValue >= maxValue){
        m_loadingBar->deleteLater();
        m_loadingBar->hide();
        m_loadingBar = nullptr;
    }
    ForceProcessEvents();
}

void exWindow::UpdateLoadingBar(){
    if(m_loadingBar == nullptr){
        return;
    }
    m_loadingBar->setValue(m_loadingBar->value() + 1);
    if(m_loadingBar->value() >= m_loadingBar->maximum()){
        m_loadingBar->deleteLater();
        m_loadingBar->hide();
        m_loadingBar = nullptr;
    }
    ForceProcessEvents();
}

void exWindow::UpdateLoadingBar(int currentValue){
    if(m_loadingBar == nullptr){
        return;
    }
    m_loadingBar->setValue(currentValue);
    if(m_loadingBar->value() >= m_loadingBar->maximum()){
        m_loadingBar->deleteLater();
        m_loadingBar->hide();
        m_loadingBar = nullptr;
    }
    ForceProcessEvents();
}

void exWindow::UpdateBackground(){
    //background.load(QCoreApplication::applicationDirPath() + "/assets/background.png");
    //background = background.scaled(centralContainer->size());
    m_centralContainer->setAutoFillBackground(true);
    //palette.setBrush(QPalette::Window, QBrush(background));
    QPalette palette;
    palette.setColor(QPalette::Window, Qt::black);
    m_centralContainer->setPalette(palette);
    m_centralContainer->show();
}

void exWindow::ClearWindow(){

    for (int i = 0; i < m_currentModeWidgets.size(); i++) {
        qDebug() << Q_FUNC_INFO << "checking widget" << m_currentModeWidgets[i]->metaObject()->className();
        m_currentModeWidgets[i]->setVisible(false);
        delete m_currentModeWidgets[i];
    }
    m_currentModeWidgets.clear();
    repaint(); //the buttons will visually remain, despite being set to not visible, until the program is told to repaint.
    //this will only work if the background image is usable
}

void exWindow::ResizeEvent(QResizeEvent* event){
    /*Resizes the background to fit the window. Will eventually add element placements so it doesn't look terrible if full-screened.*/
    QMainWindow::resizeEvent(event);
    m_hSize = this->size().width();
    m_vSize = this->size().height();
    //qDebug() << Q_FUNC_INFO << "hsize" << hSize << "vSize" << vSize;
    menuBar()->setGeometry(QRect(QPoint(int(m_hSize*0),int(m_vSize*0)), QSize(int(m_hSize*1),25)));
    //updateBackground();
    //this->setPalette(palette);
}

void exWindow::MessageError(QString message){
    m_messagePopup->setText(message);
    m_messagePopup->setWindowTitle("Error!");
    m_messagePopup->exec();
}

void exWindow::MessageSuccess(QString message){
    m_messagePopup->setText(message);
    m_messagePopup->setWindowTitle("Success.");
    m_messagePopup->exec();
}

void exWindow::HandleSettings(){
    //open a settings window
    //should probably just make the connection to settingswindow::open but this will do for now
    m_setW->Open();
}

QPoint exWindow::ScreenCenter(){
    return QGuiApplication::primaryScreen()->geometry().center();
}

void exWindow::ForceProcessEvents(){
    QApplication::processEvents();
}

QMessageBox* exWindow::MakeOkayPopup(){
    QMessageBox *dialogWindow = new QMessageBox();
    QPushButton *buttonConfirm = new QPushButton("Okay", dialogWindow);
    dialogWindow->setGeometry(QRect(ScreenCenter() - QPoint(125,125), QSize(250,250)));
    buttonConfirm->setGeometry(QRect(QPoint(20,135), QSize(200,30)));

    connect(buttonConfirm, &QPushButton::released, dialogWindow, &QDialog::accept);

    return dialogWindow;
}

QDialog* exWindow::MakeYesNoPopup(bool &finished){
    QDialog *dialogWindow = new QDialog();
    QPushButton *buttonConfirm = new QPushButton("Confirm", dialogWindow);
    QPushButton *buttonCancel = new QPushButton("Cancel", dialogWindow);
    dialogWindow->setGeometry(QRect(ScreenCenter() - QPoint(125,125), QSize(250,250)));
    buttonConfirm->setGeometry(QRect(QPoint(20,120), QSize(200,30)));
    buttonCancel->setGeometry(QRect(QPoint(20,150), QSize(200,30)));

    connect(buttonConfirm, &QPushButton::released, dialogWindow, &QDialog::accept);
    connect(buttonCancel, &QPushButton::released, dialogWindow, &QDialog::reject);
    connect(dialogWindow, &QDialog::finished, [&finished]() {finished = false;});

    return dialogWindow;
}

CustomPopup* exWindow::MakeSpecificPopup(bool &finished, QStringList addons, QStringList labels){
    CustomPopup *dialogWindow = new CustomPopup();
    int marginSize = 20;
    int windowHeight = marginSize + (addons.size()+1) * 30 + marginSize;
    int nextItemVertical = marginSize;
    int nextItemHorizontal = marginSize;
    int windowWidth = marginSize + 150 + marginSize + 150 + marginSize; //margin - button - space - button - margin

    /*Do some prep work on the lists:
        check that the caller sent enough labels, pad out the label list if not
        increase the window height for larger items*/
    int labelsNeeded = 0;
    for(int i = 0; i < addons.size(); i++){
        if(addons[i] == "boxset"){
            i++;
            int boxCount = addons[i].toInt();
            qDebug() << Q_FUNC_INFO << "adding" << boxCount << "boxes. starting window width:" << windowWidth;
            labelsNeeded += boxCount;
            windowWidth += std::max(boxCount-2, 0)*(150+marginSize);
            qDebug() << Q_FUNC_INFO << "ending window width:" << windowWidth;
        }
        if(addons[i] != "checkbox"){
            labelsNeeded++;
        }
        if(addons[i] == "textedit" || addons[i] == "list"){
            windowHeight += 60;
        }
    }

    if(labels.size() < labelsNeeded){
        for(int i = labels.size(); i < addons.size(); i++){
            labels.push_back("");
        }
    }

    windowHeight += (labels.size() * 30);


    /*Beginning of window creation. We need:
        Confirm & cancel buttons. These will always be at the bottom of the dialog.
        */
    QPushButton *buttonConfirm = new QPushButton("Confirm", dialogWindow);
    QPushButton *buttonCancel = new QPushButton("Cancel", dialogWindow);
    dialogWindow->setGeometry(QRect(ScreenCenter() - QPoint(125,125), QSize(windowWidth, windowHeight)));
    buttonConfirm->setGeometry(QRect(QPoint(windowWidth/2 - 150 - marginSize/2, windowHeight - marginSize - 30), QSize(150,30)));
    buttonCancel->setGeometry(QRect(QPoint(windowWidth/2 + marginSize/2, windowHeight - marginSize - 30), QSize(150,30)));

    int currentLabel = 0;
    for(int i = 0; i < addons.size(); i++){
        if(addons[i] != "checkbox" && addons[i] != "boxset" && addons[i] != "button"){
            QLabel* currentItemLabel = new QLabel(labels[currentLabel], dialogWindow);
            currentItemLabel->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,30)));
            nextItemVertical += 30;
            currentLabel++;
        }
        if(addons[i] == "checkbox"){
            dialogWindow->checkOption = new QCheckBox(dialogWindow);
            dialogWindow->checkOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,30)));
            dialogWindow->checkOption->show();
        }
        if(addons[i] == "button"){
            dialogWindow->buttonOption = new QPushButton(labels[currentLabel], dialogWindow);
            dialogWindow->buttonOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,30)));
            dialogWindow->buttonOption->show();
            currentLabel++;
        }
        if(addons[i] == "combobox"){
            dialogWindow->comboOption = new QComboBox(dialogWindow);
            dialogWindow->comboOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,30)));
            dialogWindow->comboOption->show();
        }
        if(addons[i] == "lineedit"){
            dialogWindow->lineOption = new QLineEdit(dialogWindow);
            dialogWindow->lineOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,30)));
            dialogWindow->lineOption->show();
        }
        if(addons[i] == "textedit"){
            dialogWindow->multiOption = new QTextEdit(dialogWindow);
            dialogWindow->multiOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,90)));
            dialogWindow->multiOption->show();
            nextItemVertical += 60;
        }
        if(addons[i] == "list"){
            dialogWindow->listOption = new QListWidget(dialogWindow);
            dialogWindow->listOption->setGeometry(QRect(QPoint(marginSize, nextItemVertical), QSize(250,90)));
            dialogWindow->listOption->show();
            nextItemVertical += 60;
        }
        if(addons[i] == "boxset"){
            i++;
            int boxes = addons[i].toInt();
            dialogWindow->boxList.resize(boxes);
            for(int j = 0; j < boxes; j++){
                QLabel* currentItemLabel = new QLabel(labels[currentLabel], dialogWindow);
                currentItemLabel->setGeometry(QRect(QPoint(nextItemHorizontal, nextItemVertical), QSize(150,30)));
                currentLabel++;

                dialogWindow->boxList[j] = new QLineEdit(dialogWindow);
                dialogWindow->boxList[j]->setGeometry(QRect(QPoint(nextItemHorizontal, nextItemVertical+30), QSize(150,30)));
                nextItemHorizontal += 150 + marginSize;
            }
            nextItemVertical += 30;
        }
        nextItemVertical += 30;
    }

    connect(buttonConfirm, &QPushButton::released, dialogWindow, &QDialog::accept);
    connect(buttonCancel, &QPushButton::released, dialogWindow, &QDialog::reject);
    connect(dialogWindow, &QDialog::finished, [&finished]() {finished = false;});

    return dialogWindow;
}

void exWindow::SetLeftWindow(QWidget* widgetToSet){
    if(m_leftSidebar == nullptr){
        m_leftSidebar = new QDockWidget(this);
        addDockWidget(Qt::LeftDockWidgetArea, m_leftSidebar);
        m_leftSidebar->setFloating(false);
        m_leftSidebar->show();
    }
    m_leftSidebar->setWidget(widgetToSet);
}

void exWindow::SetRightWindow(QWidget* widgetToSet){
    if(m_rightSidebar == nullptr){
        m_rightSidebar = new QDockWidget(this);
        addDockWidget(Qt::RightDockWidgetArea, m_rightSidebar);
        m_rightSidebar->setFloating(false);
        m_rightSidebar->show();
    }
    m_rightSidebar->setWidget(widgetToSet);
}

void exWindow::SetBottomWindow(QWidget* widgetToSet){
    if(m_bottomSidebar == nullptr){
        m_bottomSidebar = new QDockWidget(this);
        addDockWidget(Qt::BottomDockWidgetArea, m_bottomSidebar);
        m_bottomSidebar->setFloating(false);
        m_bottomSidebar->show();
    }
    m_bottomSidebar->setWidget(widgetToSet);
}
