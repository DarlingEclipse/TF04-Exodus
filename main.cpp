#include "Utility/exSettings.h"
#include "Utility/exUpdater.h"
#include "UI/exWindow.h"
#include "Utility/exDebugger.h"
#include "FileManagement/Zebrafish.h"
#include "ISOManager/IsoBuilder.h"
#include "Databases/DataHandler.h"
#include "Randomizer/Randomizer.h"

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
