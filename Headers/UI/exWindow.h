#ifndef EXWINDOW_H
#define EXWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QMessageBox>
#include <QProgressBar>

#include "Headers/Main/CustomQT.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class exSettings;

/* 2/28/25 - isolated from mainwindow.h as part of The Great Refactoring (Part 1)
 * This provides the main window for the program and handles all methods related to the user interface
 * As such, this is a singleton instance - we should only have one main window per execution
 * There may be smaller, simpler windows (see exSettings) but there will be only one exWindow*/

class exWindow : public QMainWindow
{
    Q_OBJECT

public:
    exWindow(QWidget *parent = nullptr);
    ~exWindow();

    static exWindow* s_instance;
    Ui::MainWindow *m_ui;

    exSettings *m_setW;
    int m_hSize = 1200;
    int m_vSize = 768;

    /*This should be move to the file manager - exWindow has no business knowing what this is*/
    QListWidget* m_fileBrowser;


    std::vector<QWidget*> m_currentModeWidgets;
    QDockWidget *m_leftSidebar = nullptr;
    QDockWidget *m_rightSidebar = nullptr;
    QDockWidget *m_bottomSidebar = nullptr;

    /*This should be moved to the logger - exWindow has no business knowing what this is*/
    QListWidget* m_logPrintout;

    QWidget *m_centralContainer;
    QMessageBox *m_messagePopup;

    QProgressBar *m_loadingBar;

    static exWindow& GetInstance();

    void MessageError(QString message);
    void MessageSuccess(QString message);

    /*Move to logger alongside logPrintout*/
    void Log(QString message);

    static QMessageBox* MakeOkayPopup();
    static QDialog* MakeYesNoPopup(bool &finished);
    static CustomPopup* MakeSpecificPopup(bool &finished, QStringList addons, QStringList labels);
    static QPoint ScreenCenter();
    static void ForceProcessEvents();

    void ClearWindow();

    QMenu* AddMenu(QString name);
    QAction* AddAction(QMenu* targetMenu, QString name);

    void ResizeEvent(QResizeEvent* event);
    void UpdateBackground();
    void UpdateCenter();
    void HandleSettings();
    void UpdateLoadingBar(int currentValue, int maxValue);
    void UpdateLoadingBar(); //increments the progress bar by 1
    void UpdateLoadingBar(int currentValue); //sets progress to currentvalue

    void SetLeftWindow(QWidget* widgetToSet);
    void SetRightWindow(QWidget* widgetToSet);
    void SetBottomWindow(QWidget* widgetToSet);
};

inline exWindow& exWindow::GetInstance(){
    return *s_instance;
}

#endif // EXWINDOW_H
