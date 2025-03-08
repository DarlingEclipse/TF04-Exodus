#include "Headers/Main/exDebugger.h"
#include "Headers/UI/exWindow.h"
#include "qguiapplication.h"

exDebugger* exDebugger::s_instance;

exDebugger::exDebugger()
{
    s_instance = this;
    m_UI = &exWindow::GetInstance();

    m_logPrintout = new QListWidget;
    m_UI->SetBottomWindow(m_logPrintout);

    QMenu *menuLog = m_UI->AddMenu("Log");
    QAction *actionClearLog = m_UI->AddAction(menuLog, "Clear Log");

    QAbstractButton::connect(actionClearLog, &QAction::triggered, m_UI, [this]{ClearLog();});

}

exDebugger::~exDebugger()
{

}

void exDebugger::Debug(QString message, QString function){
    qDebug() << function << message;
}

void exDebugger::Log(QString message){
    /*Add file writing to this if possible*/
    qDebug() << Q_FUNC_INFO << message;
    m_logPrintout->addItem(message);
    qApp->processEvents();
}

void exDebugger::ClearLog(){
    m_logPrintout->clear();
}

void exDebugger::MessageError(QString message){
    QMessageBox *messagePopup = m_UI->MakeOkayPopup();
    messagePopup->setText(message);
    messagePopup->setWindowTitle("Error!");
    messagePopup->exec();
}

void exDebugger::MessageSuccess(QString message){
    QMessageBox *messagePopup = m_UI->MakeOkayPopup();
    messagePopup->setText(message);
    messagePopup->setWindowTitle("Success!");
    messagePopup->exec();
}
