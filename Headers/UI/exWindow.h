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
 * This provides the main windows for the program and handles all methods related to the user interface
 * exWindowMain is initialized when the program begins and is the default UI given to all new objects that use the UI
 * However, there are plans to allow screens to pop out as their own window. I'm not sure what best practice is
 * for this, so those will use exWindowBase for now. */

class exWindowBase : public QMainWindow
{
    Q_OBJECT

public:
    exWindowBase(QWidget *parent = nullptr);
    //~exWindowBase();

    Ui::MainWindow *m_ui;

    exSettings *m_setW;
    int m_hSize = 1200;
    int m_vSize = 768;

    std::vector<QWidget*> m_currentWidgets;

    QWidget *m_centralContainer;
    QMessageBox *m_messagePopup;

    QProgressBar *m_loadingBar;

    void MessageError(QString message);
    void MessageSuccess(QString message);

    static QMessageBox* MakeOkayPopup();
    static QDialog* MakeYesNoPopup(bool &finished);
    static CustomPopup* MakeSpecificPopup(bool &finished, QStringList addons, QStringList labels);
    static QPoint ScreenCenter();
    static void ForceProcessEvents();

    void ClearWindow();

    void ResizeEvent(QResizeEvent* event);
    void UpdateBackground();
    void UpdateCenter();

    QMenu* AddMenu(QString name);
    QAction* AddAction(QMenu* targetMenu, QString name);

    void UpdateLoadingBar(int currentValue, int maxValue);
    void UpdateLoadingBar(); //increments the progress bar by 1
    void UpdateLoadingBar(int currentValue); //sets progress to currentvalue

    virtual void SetLeftWindow(QWidget* widgetToSet);
    virtual void SetRightWindow(QWidget* widgetToSet);
    virtual void SetBottomWindow(QWidget* widgetToSet);
};

class exWindowMain : public exWindowBase {
public:
    exWindowMain(QWidget *parent = nullptr);
    ~exWindowMain();


    QDockWidget *m_leftSidebar = nullptr;
    QDockWidget *m_rightSidebar = nullptr;
    QDockWidget *m_bottomSidebar = nullptr;

    void SetLeftWindow(QWidget* widgetToSet);
    void SetRightWindow(QWidget* widgetToSet);
    void SetBottomWindow(QWidget* widgetToSet);
};

#endif // EXWINDOW_H
