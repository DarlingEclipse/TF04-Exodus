#ifndef EXDEBUGGER_H
#define EXDEBUGGER_H

#include <QListWidget>

class exWindow;

/*3/2/2025 - isolated from mainwindow.h as part of The Great Refactoring (Part 1)
 *This class acts as a universal debugger and logger*/

class exDebugger{
public:
    exDebugger();
    ~exDebugger();

    static exDebugger* s_instance;

    exWindow* m_UI;

    QListWidget* m_logPrintout;

    static exDebugger& GetInstance();

    /*This just calls qdebug for now, we'll see if that changes*/
    static void Debug(QString message, QString function);

    void MessageError(QString message);
    void MessageSuccess(QString message);
    void Log(QString message);

    void ClearLog();

};

inline exDebugger& exDebugger::GetInstance(){
    return *s_instance;
}


#endif // EXDEBUGGER_H
