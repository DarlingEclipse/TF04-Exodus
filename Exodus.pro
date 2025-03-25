QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Source/Databases/DataHandler.cpp \
    Source/Databases/DatabaseModel.cpp \
    Source/Databases/DistanceCalculator.cpp \
    Source/Databases/taMinicon.cpp \
    Source/FileManagement/Zebrafish.cpp \
    Source/FileManagement/taFileSystemObject.cpp \
    Source/ISOManager/IsoBuilder.cpp \
    Source/ISOManager/ModHandler.cpp \
    Source/Main/exDebugger.cpp \
    Source/Main/exUpdater.cpp \
    Source/Models/Antioch2.cpp \
    Source/Main/BinChanger.cpp \
    Source/Databases/Database.cpp \
    Source/Databases/DatabaseItems.cpp \
    Source/Randomizer/Randomizer.cpp \
    Source/Textures/ITF.cpp \
    Source/Models/LevelGeo.cpp \
    Source/Models/Mesh.cpp \
    Source/Audio/ToneLibrary.cpp \
    Source/Models/VBIN.cpp \
    Source/Main/main.cpp \
    Source/UI/exCustomQT.cpp \
    Source/UI/exSettings.cpp \
    Source/UI/exWindow.cpp

HEADERS += \
    Headers/Databases/DataHandler.h \
    Headers/Databases/DistanceCalculator.h \
    Headers/Databases/taMinicon.h \
    Headers/FileManagement/Zebrafish.h \
    Headers/FileManagement/taFileSystemObject.h \
    Headers/ISOManager/IsoBuilder.h \
    Headers/ISOManager/ModHandler.h \
    Headers/Main/CustomQT.h \
    Headers/Main/exDebugger.h \
    Headers/Main/exUpdater.h \
    Headers/Models/Antioch2.h \
    Headers/Main/BinChanger.h \
    Headers/Databases/Database.h \
    Headers/Models/LevelGeo.h \
    Headers/Models/Mesh.h \
    Headers/Audio/ToneLibraries.h \
    Headers/Randomizer/Randomizer.h \
    Headers/Textures/itf.h \
    Headers/Models/vbin.h \
    Headers/UI/exSettings.h \
    Headers/UI/exWindow.h \
    winsparkle.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_FILE += \
    Exodus_Version.rc

DISTFILES += \
    Exodus_Version.rc

INCLUDEPATH += \
    $$PWD/WinSparkle-0.8.0/include

win32: LIBS += -L$$PWD/WinSparkle-0.8.0/x64/release/ -lWinSparkle

INCLUDEPATH += $$PWD/WinSparkle-0.8.0/x64/Release
DEPENDPATH += $$PWD/WinSparkle-0.8.0/x64/Release
