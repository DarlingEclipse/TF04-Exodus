cmake_minimum_required(VERSION 3.19)
project(Exodus LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

include_directories(../Common/include/)
include_directories(include/)
include_directories(../WinSparkle-0.9.1/include/)
link_directories(../WinSparkle-0.9.1/x64/Release/)

add_library(WinSparkle SHARED IMPORTED ../WinSparkle-0.9.1/x64/Release/WinSparkle.lib)

qt_add_executable(Exodus
    WIN32 MACOSX_BUNDLE
    main.cpp
    include/Audio/ToneLibraries.h
    include/Databases/Database.h
    include/Databases/DataHandler.h
    include/Databases/DistanceCalculator.h
    include/Databases/taMinicon.h
    include/Databases/TextDatabase.h
    include/FileManagement/taFileSystemObject.h
    include/FileManagement/Zebrafish.h
    include/ISOManager/IsoBuilder.h
    include/ISOManager/ModHandler.h
    include/Models/Antioch2.h
    include/Models/LevelGeo.h
    include/Models/Mesh.h
    include/Models/vbin.h
    include/Randomizer/Randomizer.h
    include/Textures/itf.h
    Source/Audio/ToneLibrary.cpp
    Source/Databases/Database.cpp
    Source/Databases/DatabaseItems.cpp
    Source/Databases/DatabaseModel.cpp
    Source/Databases/DataHandler.cpp
    Source/Databases/DistanceCalculator.cpp
    Source/Databases/taMinicon.cpp
    Source/FileManagement/taFileSystemObject.cpp
    Source/FileManagement/Zebrafish.cpp
    Source/ISOManager/IsoBuilder.cpp
    Source/ISOManager/ModHandler.cpp
    Source/Models/Antioch2.cpp
    Source/Models/LevelGeo.cpp
    Source/Models/Mesh.cpp
    Source/Models/VBIN.cpp

    Source/Randomizer/Randomizer.cpp
    Source/Textures/ITF.cpp
    Resources/Exodus_Version.rc
    ../Common/Source/UI/exCustomQT.cpp
    ../Common/Source/UI/exWindow.cpp
    ../Common/Source/Utility/BinChanger.cpp
    ../Common/Source/Utility/exDebugger.cpp
    ../Common/Source/Utility/exSettings.cpp
    ../Common/Source/Utility/exUpdater.cpp
    ../Common/include/UI/CustomQT.h
    ../Common/include/UI/exWindow.h
    ../Common/include/UI/ui_mainwindow.h
    ../Common/include/Utility/BinChanger.h
    ../Common/include/Utility/exDebugger.h
    ../Common/include/Utility/exSettings.h
    ../Common/include/Utility/exUpdater.h
    ../WinSparkle-0.9.1/include/winsparkle.h
)

target_link_libraries(Exodus
    PRIVATE
        Qt::Core
        Qt::Widgets
)
target_link_libraries(Exodus
    PUBLIC
        winsparkle
)


include(GNUInstallDirs)


install(TARGETS Exodus
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Exodus
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
