#include "Headers/UI/exSettings.h"
#include "Headers/Main/exUpdater.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/FileManagement/Zebrafish.h"
#include "Headers/ISOManager/IsoBuilder.h"
#include "Headers/Databases/DataHandler.h"
#include "Headers/Randomizer/Randomizer.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    exUpdater();
    exSettings mainSettings = exSettings();
    exWindowMain mainWindow = exWindowMain();
    exDebugger mainDebug = exDebugger(&mainWindow);
    zlManager mainZebrafish = zlManager(&mainWindow, &mainSettings);
    DataHandler mainDataHandler = DataHandler(&mainWindow, &mainZebrafish);
    Randomizer mainRandomizer = Randomizer(&mainWindow, &mainZebrafish, &mainDataHandler);



    return a.exec();
}
