#include "Headers/UI/exSettings.h"
#include "Headers/Main/exUpdater.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/FileManagement/Zebrafish.h"
#include "Headers/ISOManager/IsoBuilder.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    exUpdater();
    exWindow mainWindow = exWindow();
    exDebugger mainDebug = exDebugger();
    exSettings mainSettings = exSettings();
    zlManager mainZebrafish = zlManager(&mainSettings);



    return a.exec();
}
