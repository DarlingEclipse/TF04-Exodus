#include "Headers/Main/mainwindow.h"

/*
Randomizer class needs to have 3 lists of locations:
original, static list of positions  QMap<QVector3D, int> locationList; https://doc.qt.io/qt-6/qmap.html
    - this might technically just be generated with the constructor into the next list
working list of available locations
final randomized list of locations that will be written back to the files

calling randomizer rng generator "placemaster" for now

Currently errors for "no matching function" for erase and generate
generate was the wrong function for what I wanted - bounded is correct
erase needs an iterator instead of an int https://cplusplus.com/reference/vector/vector/erase/


check this out, make a test function with it:
https://doc.qt.io/qt-6/qbytearray.html#qUncompress-1
*/

Randomizer::Randomizer(ProgWindow *parentPass){
    //load minicons and locations here
    parent = parentPass;
    parent->modHandler = new ModHandler(parentPass);

    qDebug() << Q_FUNC_INFO << "loading database files";
    if(parent->loadDatabases() != 0){
        parent->log("Failed to find database files. Randomizer could not load.");
        return;
    }
    parent->clearWindow();

    parent->isoBuilder->setCopyPath("Randomizer");

    parent->dataHandler->loadLevels();
    parent->dataHandler->loadMinicons();
    parent->dataHandler->loadDatacons();
    parent->dataHandler->loadAutobots();
    parent->dataHandler->loadCustomLocations();

    for(int i = 0; i < parent->dataHandler->miniconList.size(); i++){
        int team = parent->dataHandler->miniconList[i].searchAttributes<int>("Team");
        int power = parent->dataHandler->miniconList[i].searchAttributes<int>("PowerCost");
        qDebug() << Q_FUNC_INFO << "minicon" << parent->dataHandler->miniconList[i].pickupToSpawn << "has team:" << team << "and power cost" << power;
    }

    seed = 0;
    randSettings.slipstreamDifficulty = 0;
    randSettings.highjumpDifficulty = 0;
    randSettings.overallDifficulty = 0;
    //generateDatacons = 0;
    settingsValue = "0000";

    QPushButton* buttonRandomize = new QPushButton("Randomize", parent->centralContainer);
    buttonRandomize->setGeometry(QRect(QPoint(50,220), QSize(150,30)));
    QAbstractButton::connect(buttonRandomize, &QPushButton::released, parent, [this] {randomize();});
    parent->currentModeWidgets.push_back(buttonRandomize);
    buttonRandomize->show();

    /*since the value of the settings and seed boxes will need to update when the settings/seed do,
    they need to be objects of the class so we can refer to them*/
    editSeed = new QLineEdit("", parent->centralContainer);
    editSeed->setGeometry(QRect(QPoint(200,220), QSize(150,30)));
    QLineEdit::connect(editSeed, &QLineEdit::textEdited, parent, [this](QString value) {setSeed(value);});
    parent->currentModeWidgets.push_back(editSeed);
    editSeed->show();

    editSettings = new QLineEdit("", parent->centralContainer);
    editSettings->setGeometry(QRect(QPoint(200,320), QSize(150,30)));
    parent->currentModeWidgets.push_back(editSettings);
    editSettings->show();

    QPushButton* buttonSetSettings = new QPushButton("Import Settings", parent->centralContainer);
    buttonSetSettings->setGeometry(QRect(QPoint(50,320), QSize(150,30)));
    QAbstractButton::connect(buttonSetSettings, &QPushButton::released, parent, [this] {manualSettings();});
    parent->currentModeWidgets.push_back(buttonSetSettings);
    buttonSetSettings->show();

    QPushButton* buttonGetSettings = new QPushButton("Export Settings", parent->centralContainer);
    buttonGetSettings->setGeometry(QRect(QPoint(50,350), QSize(150,30)));
    QAbstractButton::connect(buttonGetSettings, &QPushButton::released, parent, [this] {exportSettings();});
    parent->currentModeWidgets.push_back(buttonGetSettings);
    buttonGetSettings->show();

    QGroupBox *groupRandomizerOptions = new QGroupBox("Randomizer Options", parent->centralContainer);
    groupRandomizerOptions->setGeometry(QRect(QPoint(360,100), QSize(200,500)));
    parent->currentModeWidgets.push_back(groupRandomizerOptions);

    QCheckBox *checkDatacon = new QCheckBox("Place Datacons", groupRandomizerOptions);
    checkDatacon->setGeometry(QRect(QPoint(20,20), QSize(200,30)));
    QAbstractButton::connect(checkDatacon, &QCheckBox::stateChanged, parent, [checkDatacon, this]
        {randSettings.generateDatacons = checkDatacon->isChecked();
        qDebug() << Q_FUNC_INFO << "Datacon placement set to" << randSettings.generateDatacons;
        settingsValue.replace(3, 1, QString::number(randSettings.generateDatacons));});
    checkDatacon->show();
    checkDatacon->toggle();
    //parent->currentModeWidgets.push_back(checkDatacon);

    QSlider* sliderOverall  = new QSlider(Qt::Horizontal, groupRandomizerOptions);
    QLabel* labelOverall = new QLabel("General difficulty", groupRandomizerOptions);
    sliderOverall->setTickPosition(QSlider::TicksBelow);
    sliderOverall->setTickInterval(1);
    sliderOverall->setMaximum(5);
    labelOverall->setGeometry(QRect(QPoint(20,60), QSize(150,30)));
    sliderOverall->setGeometry(QRect(QPoint(20,90), QSize(150,30)));
    QSlider::connect(sliderOverall, &QAbstractSlider::valueChanged, parent, [this](int value) {setOverallDifficulty(value);});
    //parent->currentModeWidgets.push_back(sliderOverall);
    //labelOverall->setStyleSheet("QLabel { background-color : white;}");
    labelOverall->show();
    sliderOverall->show();

    QSlider* sliderSlipstream  = new QSlider(Qt::Horizontal, groupRandomizerOptions);
    QLabel* labelSlipstream = new QLabel("Slipstream difficulty", groupRandomizerOptions);
    sliderSlipstream->setTickPosition(QSlider::TicksBelow);
    sliderSlipstream->setTickInterval(1);
    sliderSlipstream->setMaximum(5);
    labelSlipstream->setGeometry(QRect(QPoint(20,120), QSize(150,30)));
    sliderSlipstream->setGeometry(QRect(QPoint(20,150), QSize(150,30)));
    QSlider::connect(sliderSlipstream, &QAbstractSlider::valueChanged, parent, [this](int value) {setSlipstreamDifficulty(value);});
    //parent->currentModeWidgets.push_back(sliderSlipstream);
    //labelSlipstream->setStyleSheet("QLabel { background-color : white;}");
    labelSlipstream->show();
    sliderSlipstream->show();

    QSlider* sliderHighjump  = new QSlider(Qt::Horizontal, groupRandomizerOptions);
    QLabel* labelHighjump = new QLabel("Highjump difficulty", groupRandomizerOptions);
    sliderHighjump->setTickPosition(QSlider::TicksBelow);
    sliderHighjump->setTickInterval(1);
    sliderHighjump->setMaximum(5);
    labelHighjump->setGeometry(QRect(QPoint(20,180), QSize(150,30)));
    sliderHighjump->setGeometry(QRect(QPoint(20,210), QSize(150,30)));
    QSlider::connect(sliderHighjump, &QAbstractSlider::valueChanged, parent, [this](int value) {setHighjumpDifficulty(value);});
    //parent->currentModeWidgets.push_back(sliderHighjump);
    //labelHighjump->setStyleSheet("QLabel { background-color : white;}");
    labelHighjump->show();
    sliderHighjump->show();

    randSettings.autoBuild = false;
    QCheckBox *checkAutoBuild = new QCheckBox("Automatically Build", groupRandomizerOptions);
    checkAutoBuild->setGeometry(QRect(QPoint(20,240), QSize(200,30)));
    QAbstractButton::connect(checkAutoBuild, &QCheckBox::stateChanged, parent, [checkAutoBuild, this] {randSettings.autoBuild = checkAutoBuild->isChecked();});
    checkAutoBuild->toggle();
    checkAutoBuild->show();
    //parent->currentModeWidgets.push_back(checkAutoBuild);

    QCheckBox *checkPowerBalance = new QCheckBox("Balanced Power Levels", groupRandomizerOptions);
    checkPowerBalance->setGeometry(QRect(QPoint(40,300), QSize(200,30)));
    checkPowerBalance->setToolTip("If checked, each Minicon's power will be within 1 of its original power (can't go over 4). "
                                  "If unchecked, each Minicon will get a random power requirement.");
    QAbstractButton::connect(checkPowerBalance, &QCheckBox::stateChanged, parent, [checkPowerBalance, this] {randSettings.balancedPower = checkPowerBalance->isChecked();});
    checkPowerBalance->toggle();
    checkPowerBalance->hide();

    QCheckBox *checkPower = new QCheckBox("Randomize Power Levels", groupRandomizerOptions);
    checkPower->setGeometry(QRect(QPoint(20,270), QSize(200,30)));
    checkPower->setToolTip("Randomizes the power needed for each Minicon. NOTE: Minicons with a power of 3 will not display correctly in HQ.");
    QAbstractButton::connect(checkPower, &QCheckBox::stateChanged, parent, [checkPower, checkPowerBalance, this]
        {randSettings.randomizePower = checkPower->isChecked();
        checkPowerBalance->setVisible(checkPower->isChecked());});
    checkPower->show();

    QCheckBox *checkTeamBalance = new QCheckBox("Balanced Team Colors", groupRandomizerOptions);
    checkTeamBalance->setGeometry(QRect(QPoint(40,360), QSize(200,30)));
    checkTeamBalance->setToolTip("Checking this means there there will be as many of each team color after randomization."
                                 "Leaving this unchecked will make a random number of each team.");
    QAbstractButton::connect(checkTeamBalance, &QCheckBox::stateChanged, parent, [checkTeamBalance, this] {randSettings.balancedTeams = checkTeamBalance->isChecked();});
    checkTeamBalance->toggle();
    checkTeamBalance->hide();

    QCheckBox *checkTeam = new QCheckBox("Randomize Team Colors", groupRandomizerOptions);
    checkTeam->setGeometry(QRect(QPoint(20,330), QSize(200,30)));
    checkTeam->setToolTip("Randomizes all Minicon team colors.");
    QAbstractButton::connect(checkTeam, &QCheckBox::stateChanged, parent, [checkTeam, checkTeamBalance, this]
        {randSettings.randomizeTeams = checkTeam->isChecked();
        checkTeamBalance->setVisible(checkTeam->isChecked());});
    checkTeam->show();

    QCheckBox *checkAutobots = new QCheckBox("Randomize Autobot Stats", groupRandomizerOptions);
    checkAutobots->setGeometry(QRect(QPoint(20,390), QSize(200,30)));
    checkAutobots->setToolTip("Slightly randomizes some Autobot stats (Health, height, dash speed). More will be added after further research.");
    QAbstractButton::connect(checkAutobots, &QCheckBox::stateChanged, parent, [checkAutobots, this]
        {randSettings.randomizeAutobotStats = checkAutobots->isChecked();});
    checkAutobots->show();

    groupRandomizerOptions->show();

    parent->centralContainer->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QLabel{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                            "QToolTip{color: rgb(0,0,0);}");

    QGroupBox *groupModOptions = new QGroupBox("Mod Options", parent->centralContainer);
    groupModOptions->setGeometry(QRect(QPoint(600,100), QSize(200,300)));
    /*groupModOptions->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                   "QToolTip{color: rgb(0,0,0);}");*/
    parent->currentModeWidgets.push_back(groupModOptions);

    for(int i = 0; i < parent->modHandler->modList.size(); i++){
        //this will need to be edited later for when we have more mods than will fit in the box to move to the next column. or scroll?

        QCheckBox *modCheck = new QCheckBox(parent->modHandler->modList[i].name, groupModOptions);
        modCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        //modCheck->setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);}");
        QAbstractButton::connect(modCheck, &QCheckBox::stateChanged, parent, [i, modCheck, this] {parent->modHandler->modList[i].enabled = modCheck->isChecked();});
        modCheck->setToolTip(parent->modHandler->modList[i].description);
        modCheck->show();
        //parent->currentModeWidgets.push_back(modCheck);
    }

    QGroupBox *groupLocations = new QGroupBox("Custom Locations", parent->centralContainer);
    groupLocations->setGeometry(QRect(QPoint(800,100), QSize(200,300)));
    /*groupLocations->setStyleSheet("QGroupBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                  "QCheckBox{color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0);} "
                                  "QToolTip{color: rgb(0,0,0);}");*/
    parent->currentModeWidgets.push_back(groupLocations);

    for(int i = 0; i < parent->dataHandler->customLocationList.size(); i++){
        qDebug() << Q_FUNC_INFO << "location name for" << i << ":" << parent->dataHandler->customLocationList[i].name;
        QCheckBox *locationCheck = new QCheckBox(parent->dataHandler->customLocationList[i].name, groupLocations);
        locationCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        QAbstractButton::connect(locationCheck, &QCheckBox::stateChanged, parent, [i, locationCheck, this]
            {parent->dataHandler->customLocationList[i].enabled = locationCheck->isChecked();});
        locationCheck->setToolTip(parent->dataHandler->customLocationList[i].description);
        locationCheck->toggle();
        locationCheck->show();
        //parent->currentModeWidgets.push_back(locationCheck);
    }


    groupModOptions->show();
    groupLocations->show();
    //checkBalancePatch->hide();
    //checkAlwaysGlider->hide();

    qDebug() << Q_FUNC_INFO << "testing randomization";

    //testAllPlacements();
    /*Uncommenting the above also requires commenting out the lines to spoil slipstream and highjump
    plus the removal of available locations in the place function*/

    //randomize();
}

void Randomizer::testAllPlacements(){
    for(int i = 0; i < availableLocations.size(); i++){
        placeMinicon(27, availableLocations[i].uniqueID);
    }
    editDatabases();
    writeSpoilers();
}

void Randomizer::setSeed(QString value){
    //need to have a check if the seed is a valid number
    seed = value.toUInt();
}

void Randomizer::exportSettings(){
    parent->log("Current settings:");
    parent->log(settingsValue);
}

void Randomizer::manualSettings(){
    //check and set each setting if value is valid
    //if value is invalid, display error and reset value
    //if all values are valid, set settingsValue to new value
    QString checkSettings = editSettings->displayText();
    bool realInt = false;
    int checkSettingsInt = checkSettings.toInt(&realInt);
    if(!realInt){
        parent->log("Invalid settings value entered.");
        return;
    }
    /*currently 4 settings:
    generate datacons
    overall difficulty
    slipstream difficulty
    highjump difficulty
    */
    int dataconCheck = 0;
    int overallCheck = 0;
    int slipstreamCheck = 0;
    int highjumpCheck = 0;
    for(int i = 0; i < 4; i++){
        switch(i){
        case 0: //datacons
            dataconCheck = checkSettingsInt%10;
            checkSettingsInt /= 10;
            break;
        case 1: //overall difficulty
            overallCheck = checkSettingsInt%10;
            checkSettingsInt /= 10;
            break;
        case 2: //slipstream difficulty
            slipstreamCheck = checkSettingsInt%10;
            checkSettingsInt /= 10;
            break;
        case 3: //highjump difficulty
            highjumpCheck = checkSettingsInt%10;
            checkSettingsInt /= 10;
            break;
        default:
            parent->log("Invalid settings value entered.");
            return;
        }

    }
    if(dataconCheck > 1 || overallCheck > 4 || slipstreamCheck > 4 || highjumpCheck > 4){
        parent->log("Invalid settings value entered.");
        return;
    }

    randSettings.generateDatacons = dataconCheck;
    randSettings.overallDifficulty = overallCheck;
    randSettings.slipstreamDifficulty = slipstreamCheck;
    randSettings.highjumpDifficulty = highjumpCheck;

    settingsValue = checkSettings;

    parent->log("Settings imported. Note that sliders will not change visibly.");
}


void Randomizer::setHighjumpDifficulty(int value){
    randSettings.highjumpDifficulty = value;
    qDebug() << Q_FUNC_INFO << "Highjump difficulty set to:" << randSettings.highjumpDifficulty;
    settingsValue.replace(0, 1, QString::number(randSettings.highjumpDifficulty));
}

void Randomizer::setSlipstreamDifficulty(int value){
    randSettings.slipstreamDifficulty = value;
    qDebug() << Q_FUNC_INFO << "Slipstream difficulty set to:" << randSettings.slipstreamDifficulty;
    settingsValue.replace(1, 1, QString::number(randSettings.slipstreamDifficulty));
}

void Randomizer::setOverallDifficulty(int value){
    randSettings.overallDifficulty = value;
    qDebug() << Q_FUNC_INFO << "Overall difficulty set to:" << randSettings.overallDifficulty;
    settingsValue.replace(2, 1, QString::number(randSettings.overallDifficulty));
}

/*void Randomizer::setDataconOutput(bool placeDatacons){
    randSettings.generateDatacons = placeDatacons;
    qDebug() << Q_FUNC_INFO << "Datacon placement set to" << randSettings.generateDatacons;
    settingsValue.replace(3, 1, QString::number(randSettings.generateDatacons));
}*/

void Randomizer::reset(){
    placedLocations.clear();
    availableLocations.clear();

    parent->dataHandler->resetLevels();
    parent->dataHandler->resetMinicons();
    parent->dataHandler->resetDatacons();

}

void Randomizer::randomize(){
    if(placedLocations.size() > 0){
        reset();
    }
    if(seed == 0){
        parent->log("No seed set - generating random seed.");
        seed = QRandomGenerator::global()->generate();
        editSeed->setText(QString::number(seed));
    }
    qDebug() << Q_FUNC_INFO << "difficulties - slipstream:" << randSettings.slipstreamDifficulty << "highjump" << randSettings.highjumpDifficulty << "overall" << randSettings.overallDifficulty;


    qDebug() << Q_FUNC_INFO << "Starting randomization. Available locations:" << availableLocations.size();
    placemaster.seed(seed);

    if(randSettings.randomizePower){
        randomizePowers();
    }

    if(randSettings.randomizeTeams){
        randomizeTeamColors();
    }

    if(randSettings.randomizeAutobotStats){
        randomizeAutobotStats();
    }

    //move this to datahandler, please
    for(int i = 0; i < parent->dataHandler->customLocationList.size(); i++){
        if(parent->dataHandler->customLocationList[i].enabled){
            for(int j = 0; j < parent->dataHandler->customLocationList[i].locationList.size(); j++){
                parent->dataHandler->loadedLocations.push_back(parent->dataHandler->customLocationList[i].locationList[j]);
            }
        }
    }
    //sort minicon list by enumID
    availableLocations = parent->dataHandler->loadedLocations;

    placeSlipstream();
    qDebug() << "Step 1 complete";
    placeHighjump();
    qDebug() << "Step 2 complete";
    placeRangefinder();
    qDebug() << "Step 3 complete";
    placeShepherd();
    qDebug() << "Step 4 complete";
    if(randSettings.overallDifficulty == 0){
        placeStarterWeapon();
    }
    qDebug() << "Step 5 complete";

    placeAll();
    qDebug() << "Step 6 complete";

    parent->log("Minicon placements randomized.");

    std::sort(placedLocations.begin(), placedLocations.end());

    editDatabases();
    writeSpoilers();

    randomFileReplacements();
    parent->modHandler->applyModifications();

    for(int i = 0; i < parent->modHandler->folderOptions.size(); i++){
        if(parent->modHandler->folderOptions[i].folderName == "TFA"){
            parent->modHandler->folderOptions[i].moddedSource = true;
        }
    }
    if(randSettings.autoBuild){
        parent->isoBuilder->packModded("Randomizer");
    }
    parent->log("Database and spoiler files written. Randomization complete.");
    availableLocations.clear();
}

void Randomizer::randomFileReplacements(){
    for(int i = 0; i < parent->modHandler->replacementList.size(); i++){
        /*Uncommend this when we add a file replacement menu to the randomizer, otherwise it skips all replacements*/
        /*if(!parent->modHandler->replacementList[i].enabled){
            continue;
        }*/
        int replaceChance = placemaster.generate();
        qDebug() << Q_FUNC_INFO << "Checking if replacement happens:" << replaceChance << parent->modHandler->replacementList[i].rarity << replaceChance % parent->modHandler->replacementList[i].rarity;
        if(replaceChance % parent->modHandler->replacementList[i].rarity == 0){
            parent->modHandler->replacementList[i].enabled = true;
            parent->modHandler->replaceFile(parent->modHandler->replacementList[i]);
        }
    }

    qDebug() << Q_FUNC_INFO << "Replacing title screen.";
    parent->modHandler->replaceFile("ARMADALOGO.ITF", "/TFA/USERINTERFACE/TEXTURES");
    return;
}

void Randomizer::placeSlipstream(){
    qDebug() << Q_FUNC_INFO << "Placing slipstream. Available locations" << availableLocations.size();
    std::vector<exPickupLocation> slipstreamLocations;
    for(int i = 0; i < availableLocations.size(); i++){
        if(availableLocations[i].slipstreamDifficulty <= randSettings.slipstreamDifficulty){
            slipstreamLocations.push_back(availableLocations[i]);
        }
    }
    qDebug() << Q_FUNC_INFO << "Available slipstream locations:" << slipstreamLocations.size();
    int locationNumber = placemaster.bounded(slipstreamLocations.size());
    qDebug() << Q_FUNC_INFO << "Placing slipstream at" << slipstreamLocations[locationNumber].uniqueID;
    placeMinicon(28, slipstreamLocations[locationNumber].uniqueID);
    if(randSettings.slipstreamDifficulty > 3){
        placeSlipstreamRequirement(44, slipstreamLocations[locationNumber].uniqueID);
        placeSlipstreamRequirement(14, slipstreamLocations[locationNumber].uniqueID);
        placeSlipstreamRequirement(49, slipstreamLocations[locationNumber].uniqueID);
    }
}

void Randomizer::placeHighjump(){
    qDebug() << Q_FUNC_INFO << "Placing highjump. Available locations" << availableLocations.size();
    std::vector<exPickupLocation> highjumpLocations;
    for(int i = 0; i < availableLocations.size(); i++){
        if(availableLocations[i].highjumpDifficulty <= randSettings.highjumpDifficulty){
            highjumpLocations.push_back(availableLocations[i]);
        }
    }
    int locationNumber = placemaster.bounded(highjumpLocations.size());
    qDebug() << Q_FUNC_INFO << "Placing highjump at" << highjumpLocations[locationNumber].uniqueID;
    placeMinicon(50, highjumpLocations[locationNumber].uniqueID);
}

void Randomizer::placeRangefinder(){
    qDebug() << Q_FUNC_INFO << "Placing rangefinder. Available locations" << availableLocations.size();
    std::vector<exPickupLocation> rangefinderLocations;
    for(int i = 0; i < availableLocations.size(); i++){
        if((availableLocations[i].gameID[0] <= 3 && randSettings.overallDifficulty < 4)
                || randSettings.overallDifficulty > 3){
            rangefinderLocations.push_back(availableLocations[i]);
        }
    }
    int locationNumber = placemaster.bounded(rangefinderLocations.size());
    qDebug() << Q_FUNC_INFO << "Placing Rangefinder at" << rangefinderLocations[locationNumber].uniqueID;
    placeMinicon(43, rangefinderLocations[locationNumber].uniqueID);
}

void Randomizer::placeStarterWeapon(){
    std::vector<exMinicon> starterMinicons;
    qDebug() << Q_FUNC_INFO << "Placing starter weapon. total minicon options:" << parent->dataHandler->miniconList.size();
    for(int i = 0; i < parent->dataHandler->miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking if minicon" << i << parent->dataHandler->miniconList[i].pickupToSpawn << "is a weapon";
        if(parent->dataHandler->miniconList[i].isWeapon){
            qDebug() << Q_FUNC_INFO << "it is, adding to list";
            starterMinicons.push_back(parent->dataHandler->miniconList[i]);
        }
    }
    qDebug() << Q_FUNC_INFO << "A total of" << starterMinicons.size() << "minicons are available";
    int miniconNumber = placemaster.bounded(starterMinicons.size());
    qDebug() << Q_FUNC_INFO << "Placing starter weapon" << starterMinicons[miniconNumber].pickupToSpawn;
    placeMinicon(starterMinicons[miniconNumber].enumID, 42069); //locationID tbd
}

void Randomizer::placeShepherd(){
    qDebug() << Q_FUNC_INFO << "Finding placement for shepherd. available locations:" << availableLocations.size();
    std::vector<exPickupLocation> shepherdLocations;
    for(int i = 0; i < availableLocations.size(); i++){
        if((parent->dataHandler->bunkerList.contains(availableLocations[i].uniqueID) && randSettings.overallDifficulty > 3)
                || (availableLocations[i].gameID[0] == 7 && randSettings.overallDifficulty == 3)
                || (randSettings.overallDifficulty == 2)
                || (availableLocations[i].gameID[0] < 5 && randSettings.overallDifficulty < 2)){
            shepherdLocations.push_back(availableLocations[i]);
        }
    }
    int locationNumber = placemaster.bounded(shepherdLocations.size());
    qDebug() << Q_FUNC_INFO << "Placing shepherd at" << shepherdLocations[locationNumber].uniqueID;
    placeMinicon(42, shepherdLocations[locationNumber].uniqueID);
}

void Randomizer::placeSlipstreamRequirement(int miniconID, int placementID){
    //for use in higher difficulty slipstream settings
    //ex. hailstorm + comeback for stasis lock ragdoll launches
    std::vector<exPickupLocation> prereqLocations;
    int placementLevel = 0;
    //looking through placedlocations since slipstream should already be placed
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].uniqueID == placementID){
            placementLevel  = placedLocations[i].gameID[0];
            //qDebug() << Q_FUNC_INFO << "prereq slipstream level found as" << placementLevel;
        }
    }
    //qDebug() << Q_FUNC_INFO << "determining prerequisite locations for" << miniconID << "and placement ID" << placementID;
    for(int i = 0; i < availableLocations.size(); i++){
        if((availableLocations[i].slipstreamDifficulty <= 3)
                && (availableLocations[i].gameID[0] <= placementLevel)
                && (availableLocations[i].uniqueID != 42069)){ //we don't want one of the requirements spawning right at the start - the player asked for pain.
            //qDebug() << Q_FUNC_INFO << "prereq location" << availableLocations[i].uniqueID << "name" << availableLocations[i].locationName << "added to possible list.";
            prereqLocations.push_back(availableLocations[i]);
        }
    }
    //qDebug() << Q_FUNC_INFO << "prereq locations:" << prereqLocations.size();
    int locationNumber = placemaster.bounded(prereqLocations.size());
    placeMinicon(miniconID, prereqLocations[locationNumber].uniqueID);
}

void Randomizer::placeMinicon(int miniconToPlace, int placementID){
    //assign the minicon to the location
    //copy the location to the placed list
    //then remove that location and minicon from the working lists
    qDebug() << Q_FUNC_INFO << "Placing minicon" << miniconToPlace << "at" << placementID;
    exMinicon* chosenMinicon = nullptr;
    for(int i = 0; i < parent->dataHandler->miniconList.size(); i++){
        if(parent->dataHandler->miniconList[i].enumID == miniconToPlace){
            //miniconList[i].placed = true;
            chosenMinicon = &parent->dataHandler->miniconList[i];
        }
    }
    if(chosenMinicon == nullptr){
        parent->log("Minicon "  + QString::number(miniconToPlace) + " was not found. | " + QString(Q_FUNC_INFO));
        return;
    }
    for(int i = 0; i < availableLocations.size(); i++){
        if(availableLocations[i].uniqueID != placementID){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "found location ID" << placementID << "with linked location" << availableLocations[i].linkedLocationID;
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        int linkedLocation = availableLocations[i].linkedLocationID;
        availableLocations[i].assignPickup(chosenMinicon);
        placedLocations.push_back(availableLocations[i]);
        parent->dataHandler->levelList[availableLocations[i].gameID[0]].miniconCount++;
        availableLocations.erase(availableLocations.begin() + i);
        if(linkedLocation != 999){
            qDebug() << Q_FUNC_INFO << "searching for linked location" << linkedLocation;
            bool foundLocation = false;
            for(int j = 0; j < availableLocations.size(); j++){
                //qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocation;
                if(availableLocations[j].uniqueID != linkedLocation){
                    continue;
                }
                qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocation;
                foundLocation = true;
                availableLocations[j].assignPickup(chosenMinicon);
                placedLocations.push_back(availableLocations[j]);
                parent->dataHandler->levelList[availableLocations[j].gameID[0]].miniconCount++;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placementID << availableLocations[j].uniqueID << ":" << miniconToPlace << "vs" << availableLocations[j].assignedPickup()->enumID;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placedLocations[placedLocations.size()-1].assignedPickup()->enumID;
                availableLocations.erase(availableLocations.begin() + j);
            }
            if(!foundLocation){
                qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocation;
            }
        }
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        chosenMinicon->placed = true;
        return;
    }
    parent->log("Location "  + QString::number(placementID) + " was not found. | " + QString(Q_FUNC_INFO));
}

void Randomizer::placeDatacon(int dataconToPlace, int placementID){
    //assign the datacon to the location
    //copy the location to the placed list
    //then remove that location and datacon from the working lists
    qDebug() << Q_FUNC_INFO << "Placing datacon" << dataconToPlace << "at" << placementID;
    exPickup* chosenDatacon = nullptr;
    for(int i = 0; i < parent->dataHandler->dataconList.size(); i++){
        if(parent->dataHandler->dataconList[i].dataID == dataconToPlace){
            chosenDatacon = &parent->dataHandler->dataconList[i];
        }
    }
    if(chosenDatacon == nullptr){
        parent->log("Datacon "  + QString::number(dataconToPlace) + " was not found. | " + QString(Q_FUNC_INFO));
        return;
    }
    for(int i = 0; i < availableLocations.size(); i++){
        if(availableLocations[i].uniqueID != placementID){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "found location ID" << placementID << "with linked location" << availableLocations[i].linkedLocationID;
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        int linkedLocation = availableLocations[i].linkedLocationID;
        availableLocations[i].assignPickup(chosenDatacon);
        placedLocations.push_back(availableLocations[i]);
        parent->dataHandler->levelList[availableLocations[i].gameID[0]].dataconCount++;
        qDebug() << Q_FUNC_INFO << "after assigning location" << availableLocations[i].uniqueID << "erasing location" << (availableLocations.begin() + i)->uniqueID;
        availableLocations.erase(availableLocations.begin() + i);
        if(linkedLocation != 999){
            qDebug() << Q_FUNC_INFO << "searching for linked location" << linkedLocation;
            bool foundLocation = false;
            for(int j = 0; j < availableLocations.size(); j++){
                qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocation;
                if(availableLocations[j].uniqueID != linkedLocation){
                    continue;
                }
                qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocation;
                foundLocation = true;
                availableLocations[j].assignPickup(chosenDatacon);
                placedLocations.push_back(availableLocations[j]);
                parent->dataHandler->levelList[availableLocations[j].gameID[0]].dataconCount++;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placementID << availableLocations[j].uniqueID << ":" << dataconToPlace << "vs" << availableLocations[j].assignedPickup()->enumID;
                qDebug() << Q_FUNC_INFO << "after assigning location" << availableLocations[j].uniqueID << "erasing location" << (availableLocations.begin() + j)->uniqueID;
                availableLocations.erase(availableLocations.begin() + j);
            }
            if(!foundLocation){
                qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocation;
            }
        }
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        chosenDatacon->placed = true;
        return;
    }
    parent->log("Location "  + QString::number(placementID) + " was not found. | " + QString(Q_FUNC_INFO));
}

void Randomizer::placeAll(){
    //use std::shuffle with the randomizer's rng
    qDebug() << Q_FUNC_INFO << "Placing all remaining minicons. Currently" << parent->dataHandler->miniconList.size() << "pickups to go through and place";
    //std::shuffle(std::begin(availableLocations), std::end(availableLocations), placemaster);
    //with this, we have three lists: the minicon list, the location list, and the list of placed locations
    //since we'll have more locations than minicons, we can shuffle the location list
    //and then assign minicons to it 1-to-1, copying those to the placed locations list.
    //then the placed locations combos would be written back to the database files
    int locationNumber = 0;
    int availableLevel = 0;
    for(int i = 0; i < parent->dataHandler->miniconList.size(); i++){
        if(!parent->dataHandler->miniconList[i].placed && parent->dataHandler->miniconList[i].hasVanillaPlacement){
            qDebug() << Q_FUNC_INFO << "Placing minicon.";
            locationNumber = placemaster.bounded(availableLocations.size());

            //The below doesn't account for special placement counts (slipstream, highjump, etc) so some levels end up stacked anyway.
            availableLevel = availableLocations[locationNumber].gameID[0];
            while(parent->dataHandler->levelList[availableLevel].miniconCount > 6 /*|| (levelList[availableLevel].removedInstances < levelList[availableLevel].miniconCount)*/){
                qDebug() << Q_FUNC_INFO << "Level" << parent->dataHandler->levelList[availableLevel].levelName << "has too many placements. Rerolling.";
                qDebug() << Q_FUNC_INFO << "Max placements:" << parent->dataHandler->levelList[availableLevel].removedInstances;
                qDebug() << Q_FUNC_INFO << "current placements:" << parent->dataHandler->levelList[availableLevel].miniconCount;
                locationNumber = placemaster.bounded(availableLocations.size());
                availableLevel = availableLocations[locationNumber].gameID[0];
            }
            placeMinicon(parent->dataHandler->miniconList[i].enumID, availableLocations[locationNumber].uniqueID);
        } else {
            qDebug() << Q_FUNC_INFO << "Skipping placement for" << parent->dataHandler->miniconList[i].pickupToSpawn;
        }
    }

    qDebug() << Q_FUNC_INFO << "running pre-datacon check";
    for(int i = 0; i < placedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "placed location" << placedLocations[i].uniqueID << "has assigned pickup" << placedLocations[i].assignedPickup()->enumID;
    }
    /*for(int i = 0; i < availableLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "available location" << availableLocations[i].uniqueID << "has assigned pickup" << availableLocations[i].assignedPickup()->enumID;
    }*/

    if(randSettings.generateDatacons){
        qDebug() << Q_FUNC_INFO << "Placing datacons.";
        for(int i = 0; i < parent->dataHandler->dataconList.size(); i++){
            if(!parent->dataHandler->dataconList[i].placed){
                qDebug() << Q_FUNC_INFO << "Placing datacon.";
                qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size();
                qDebug() << Q_FUNC_INFO << "reaches point 0. available locations:" << availableLocations.size();
                locationNumber = placemaster.bounded(availableLocations.size());
                qDebug() << Q_FUNC_INFO << "reaches point 1";
                availableLevel = availableLocations[locationNumber].gameID[0];
                qDebug() << Q_FUNC_INFO << "reaches point 2";
                while(parent->dataHandler->levelList[availableLevel].dataconCount > 12){
                    qDebug() << Q_FUNC_INFO << "Level" << parent->dataHandler->levelList[availableLevel].levelName << "has too many datacons. Rerolling.";
                    qDebug() << Q_FUNC_INFO << "Max placements:" << parent->dataHandler->levelList[availableLevel].removedInstances;
                    qDebug() << Q_FUNC_INFO << "current datacons:" << parent->dataHandler->levelList[availableLevel].dataconCount << "available locations:" << availableLocations.size();
                    locationNumber = placemaster.bounded(availableLocations.size());
                    availableLevel = availableLocations[locationNumber].gameID[0];
                }
                qDebug() << Q_FUNC_INFO << "reaches point 3";
                placeDatacon(&parent->dataHandler->dataconList[i], availableLocations[locationNumber]);
            } else {
                qDebug() << Q_FUNC_INFO << "Skipping placement.";
            }
        }
    }
    qDebug() << Q_FUNC_INFO << "running post-datacon check";
    for(int i = 0; i < placedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "placed location" << placedLocations[i].uniqueID << "has assigned pickup" << placedLocations[i].assignedPickup()->enumID;
    }

}

void Randomizer::removeLocation(exPickupLocation locationToRemove){
    qDebug() << Q_FUNC_INFO << "available location count before removal:" << availableLocations.size();
    for(int i = 0; i < availableLocations.size(); i++){
        if(availableLocations[i].uniqueID == locationToRemove.uniqueID){
            availableLocations.erase(availableLocations.begin() + i);
        }
    }
    qDebug() << Q_FUNC_INFO << "available location count after removal:" << availableLocations.size();
}

void Randomizer::placeDatacon(exPickup* dataconToPlace, exPickupLocation location){
    qDebug() << Q_FUNC_INFO << "placing datacon at location ID" << location.uniqueID << "with linked location" << location.linkedLocationID;
    qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
    int linkedLocation = location.linkedLocationID;
    location.assignPickup(dataconToPlace);
    placedLocations.push_back(location);
    parent->dataHandler->levelList[location.gameID[0]].dataconCount++;
    removeLocation(location);
    if(linkedLocation != 999){
        qDebug() << Q_FUNC_INFO << "searching for linked location" << linkedLocation;
        bool foundLocation = false;
        for(int j = 0; j < availableLocations.size(); j++){
            //qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocation;
            if(availableLocations[j].uniqueID != linkedLocation){
                continue;
            }
            qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocation;
            foundLocation = true;
            availableLocations[j].assignPickup(dataconToPlace);
            placedLocations.push_back(availableLocations[j]);
            parent->dataHandler->levelList[availableLocations[j].gameID[0]].dataconCount++;
            availableLocations.erase(availableLocations.begin() + j);
        }
        if(!foundLocation){
            qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocation;
        }
    }
    qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
    dataconToPlace->placed = true;
}

void Randomizer::spoilMinicon(exPickupLocation placement, QTextStream& stream){
    //qDebug() << Q_FUNC_INFO << "attempting to place" << placement.uniqueID << "with assigned minicon"
    //         << placement.assignedMinicon << "out of" << miniconList.size() << "minicons";
    if(placement.assignedPickup()->enumID == 3){
        qDebug() << Q_FUNC_INFO << "Attempted to spoil a datacon position. We don't do that (yet).";
        return;
    }
    QVector3D placedPosition = placement.position;
    qDebug() << Q_FUNC_INFO << "sending miniconID" << placement.assignedPickup()->enumID << "to be spoiled";
    stream << placement.assignedPickup()->pickupToSpawn << " is located at " << placement.locationName << " id " << placement.uniqueID << ", or x" <<
                                                             placedPosition.x() << " y" << placedPosition.y() << " z" << placedPosition.z() << Qt::endl;
    placement.spoiled = true;
}

void Randomizer::spoilMinicon(int miniconID, QTextStream& stream){
    int placementIndex;
    qDebug() << Q_FUNC_INFO << "placedlocations count:" << placedLocations.size();
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].assignedPickup()->enumID == miniconID){
            placementIndex = i;
            //placedLocations[i].spoiled = true;
        }
    }
    if(placedLocations[placementIndex].uniqueID == 0){
        parent->log("Could not find minicon " + QString::number(miniconID) + " at any placement." );
    }
    QVector3D placedPosition = placedLocations[placementIndex].position;
    qDebug() << Q_FUNC_INFO << "spoiling minicon" << miniconID << "at location" << placedLocations[placementIndex].uniqueID;
    QString miniconName = parent->dataHandler->getMinicon(miniconID)->pickupToSpawn;
    stream << miniconName << " is located at " << parent->dataHandler->levelList[placedLocations[placementIndex].gameID[0]].levelName << "'s " << placedLocations[placementIndex].locationName << " id "
           << placedLocations[placementIndex].uniqueID << ", or x" << placedPosition.x() << " y" << placedPosition.y() << " z" << placedPosition.z() << Qt::endl;
}

int Randomizer::writeSpoilers(){
    //use game directory, make new directory in same directory as game
    //this will actually be read earlier in the process, just doing this here for now
    QString outputFile = parent->isoBuilder->copyOutputPath + "/Spoilers.txt";
    QFile spoilerOut(outputFile);
    QFile file(outputFile);
    qDebug() << Q_FUNC_INFO << "checking if randomizer directory exists:" << file.exists();
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();
    qDebug() << Q_FUNC_INFO << "checking if randomizer directory exists again:" << file.exists();


    int currentLevel = -1;
    qDebug() << Q_FUNC_INFO << "attempting to write spoiler file";
    if (spoilerOut.open(QIODevice::ReadWrite)){
        QTextStream fileStream(&spoilerOut);

        fileStream << "!SPOILERS FOR SEED " << QString::number(seed) << " BELOW!" << Qt::endl;
        qDebug() << Q_FUNC_INFO << "Attempting to write spoilers";
        qDebug() << Q_FUNC_INFO << "Placed locations:" << placedLocations.size();
        //write slipstream location
        spoilMinicon(28, fileStream);

        //if slipstream requirements are active, write those
        if(randSettings.slipstreamDifficulty > 4){ //actual difficulty tbd
            for(int i = 0; i < placedLocations.size(); i++){
                if(placedLocations[i].assignedPickup()->enumID == 1){
                    spoilMinicon(placedLocations[i], fileStream);
                }
            }
            for(int i = 0; i < placedLocations.size(); i++){
                if(placedLocations[i].assignedPickup()->enumID == 1){
                    spoilMinicon(placedLocations[i], fileStream);
                }
            }
        }

        //write highjump location
        spoilMinicon(50, fileStream);
        //write all others, in order of level
        //qDebug() << Q_FUNC_INFO << "Writing all locations. Total placements:" << placedLocations.size();
        for(int i = 0; i < placedLocations.size(); i++){
            if(currentLevel != placedLocations[i].gameID[0]){
                currentLevel++;
                fileStream << Qt::endl;
                fileStream << Qt::endl;
                fileStream << "Level " << QString::number(currentLevel+1) << ": " << parent->dataHandler->levelList[currentLevel].levelName << Qt::endl;
                fileStream << "Contains " << QString::number(parent->dataHandler->levelList[currentLevel].dataconCount) << " Datacons";
                fileStream << Qt::endl;
            }
            qDebug() << Q_FUNC_INFO << "writing location" << placedLocations[i].uniqueID << "with assigned minicon" << placedLocations[i].assignedPickup()->enumID << "to location" << placedLocations[i].position;
            if(!placedLocations[i].spoiled){
                spoilMinicon(placedLocations[i], fileStream);
            }
        }

        //include location name and coords
    } else {
        qDebug() << Q_FUNC_INFO << "failed to open file for writing.";
        return 1;
    }
    return 0;
}

/*Maybe the database files can just have Warpgate, Minicon, and Datacon lists
then this system can use that instead of trying to add that system to the base file

then these files would have a home in the database section instead of being loose

Placement Objects could become its own header, containing the definitions for warpgates, minis, and datas
This also makes assigning specific properties to each type easier, like mini and data spawn difficulty

*/

int Randomizer::editDatabases(){
    /*
    specific concerns with clearing pickupplaced:
        Check deep amazon to see if the dropship not being able to find its minicon causes problems
        check bunkers to make sure that script isn't sad without a datacon
    */
    for(int i = 0; i<parent->dataHandler->levelList.size(); i++){
        qDebug() << Q_FUNC_INFO << "removing all pickups from" << parent->dataHandler->levelList[i].fromFile->fileName;
        parent->dataHandler->levelList[i].fromFile->removeAll("PickupPlaced");
    }
    QString modFileDirectory = QDir::currentPath();
    QString slipstreamInPath = modFileDirectory + "/ASSETS/INTROGLIDING.CS";
    QFile slipstreamFix(slipstreamInPath);
    bool didItWork = false;

    QString tempNameStorage; //I don't like this, but changing it permanently will prevent the spoiler file from writing correctly
    QString levelPath;
    QString slipstreamOutPath;
    exMinicon itemToAdd;
    int level = 0;
    for(int i = 0; i < placedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking level for location" << i << "uniqueID" << placedLocations[i].uniqueID << "level" << placedLocations[i].gameID[0];
        if(placedLocations[i].gameID[0] != level){
            qDebug() << Q_FUNC_INFO << "output path for this level will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->levelList[level].outputName + "/CREATURE.TDB");
            levelPath = parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->levelList[level].outputName;
            //get containing directory
            //create Randomizer folder in that directory
            qDebug() << Q_FUNC_INFO << "path being checked:" << levelPath;
            QDir checkDir(levelPath);
            if(!checkDir.exists()){
                checkDir.mkpath(".");
            }
            //since the placedLocations list should be sorted by level
            //this just saves on a couple hundred loops
            parent->dataHandler->levelList[level].fromFile->outputPath = levelPath + "/CREATURE.TDB";
            parent->dataHandler->levelList[level].fromFile->save("TDB");

            //I am purposefully leaving the slipstream fix out of pacific island
            //if you get that far without it, you don't get the cutscene or the option to equip. You clearly don't need it anyway.
            slipstreamOutPath = levelPath + "/CINEMA";
            qDebug() << Q_FUNC_INFO << "Slipstream cinema output directory:" << slipstreamOutPath;
            checkDir.setPath(slipstreamOutPath);
            qDebug() << Q_FUNC_INFO << "Slipstream output directory exists?" << checkDir.exists();
            if(!checkDir.exists()){
                checkDir.mkpath(".");
            }
            slipstreamOutPath += "/INTROGLIDING.CS";
            if(slipstreamFix.exists()){
                didItWork = QFile::copy(slipstreamInPath, slipstreamOutPath);
                qDebug() << Q_FUNC_INFO << "did the slipstream copy work?" << didItWork;
            } else {
                parent->log("Could not place Slipstream-fix cinema file.");
            }

            level++;
        }
        //itemToAdd = *getMinicon(placedLocations[i].assignedMinicon());
        tempNameStorage = placedLocations[i].locationName;
        qDebug() << Q_FUNC_INFO << "Storing minicon category" << tempNameStorage;
        placedLocations[i].locationName = "PickupPlaced";
        /*In converting to the new logic, this will be the line with the biggest impact*/
        qDebug() << Q_FUNC_INFO << "checking level list size:" << parent->dataHandler->levelList.size();
        qDebug() << Q_FUNC_INFO << "does level have a file?:" << parent->dataHandler->levelList[level].fromFile->fileName;
        parent->dataHandler->levelList[level].fromFile->addInstance(parent->dataHandler->createExodusPickupLocation(placedLocations[i]));
        //placedLocations[i].instanceIndex = parent->dataHandler->levelList[level].levelFile->addInstance(placedLocations[i]);
        placedLocations[i].locationName = tempNameStorage;
        qDebug() << Q_FUNC_INFO << "placed location instance index is" << placedLocations[i].instanceIndex;
    }
    qDebug() << Q_FUNC_INFO << "Correcting Pacific island bunker links";
    fixBunkerLinks(level);
    qDebug() << Q_FUNC_INFO << "output path for this level will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->levelList[level].outputName + "/CREATURE.TDB");
    levelPath = parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->levelList[level].outputName;
    //get containing directory
    //create Randomizer folder in that directory
    qDebug() << Q_FUNC_INFO << "path being checked:" << levelPath;
    QDir checkDir(levelPath);
    if(!checkDir.exists()){
        checkDir.mkpath(".");
    }
    //since the placedLocations list should be sorted by level
    //this just saves on a couple hundred loops
    parent->dataHandler->levelList[level].fromFile->outputPath = levelPath + "/CREATURE.TDB";
    parent->dataHandler->levelList[level].fromFile->save("TDB");
    level++;


    std::shared_ptr<DatabaseFile> metagameFile;
    bool metagameEdited = false;
    for(int i = 0; i<parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking databse file" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            qDebug() << Q_FUNC_INFO << "setting metagame file to:" << parent->databaseList[i]->fileName;
            metagameFile = parent->databaseList[i];
        }
    }

    QStringList miniconTypes = {"Minicon", "MiniconDamageBonus", "MiniconArmor", "MiniconEmergencyWarpgate", "MiniconRangeBonus", "MiniconRegeneration"};
    if(randSettings.randomizePower || randSettings.randomizeTeams){
        //this could be an issue - some minicons are their own classes and inherit from minicon.
        metagameEdited = true;
        for(int i = 0; i < miniconTypes.size(); i++){
            metagameFile->removeAll(miniconTypes[i]);
        }
        for(int i = 0; i < parent->dataHandler->miniconList.size(); i++){
            metagameFile->addInstance(parent->dataHandler->miniconList[i]);
        }
    }

    if(randSettings.randomizeAutobotStats){
        metagameEdited = true;
        metagameFile->removeAll("Autobot");
        for(int i = 0; i < parent->dataHandler->autobotList.size(); i++){
            metagameFile->addInstance(parent->dataHandler->autobotList[i]);
        }
    }

    if(metagameEdited){
        qDebug() << Q_FUNC_INFO << "output path for METAGAME will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/METAGAME.TDB");
        QString metagamePath = parent->isoBuilder->copyOutputPath + "/TFA/METAGAME.TDB";
        metagameFile->outputPath = metagamePath;
        metagameFile->save("TDB");
    }

    return 0;
}

void Randomizer::fixBunkerLinks(int level){
    std::shared_ptr<DatabaseFile> pacificFile = parent->dataHandler->levelList[level].fromFile;
    std::vector<int> usedBunkers;
    qDebug() << Q_FUNC_INFO << "bunker file level name:" << pacificFile->fullFileName();
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].bunkerID == 0 || placedLocations[i].assignedPickup()->enumID != 3){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "bunker ID for pickup" << placedLocations[i].uniqueID << "is" << placedLocations[i].bunkerID;
        for(int j = 0; j < pacificFile->instances.size(); j++){
            if(pacificFile->instances[j].instanceIndex != placedLocations[i].bunkerID){
                continue;
            }
            qDebug() << Q_FUNC_INFO << "Matching bunker index ID found at" << pacificFile->instances[j].instanceIndex;
            usedBunkers.push_back(pacificFile->instances[j].instanceIndex);
            for(int attribute = 0; attribute < pacificFile->instances[j].attributes.size(); attribute++){
                pacificFile->instances[j].setAttribute("Reward_PickupLink", QString::number(placedLocations[i].instanceIndex));
            }
        }
    }

    if (usedBunkers.empty()){
        qDebug() << Q_FUNC_INFO << "no datacons assigned to bunkers.";
        return;
    }
    for(int j = 0; j < pacificFile->instances.size(); j++){
        if(std::find(usedBunkers.begin(), usedBunkers.end(), pacificFile->instances[j].instanceIndex) != usedBunkers.end()){
            continue;
        }
        //qDebug() << Q_FUNC_INFO << "Unused bunker index ID found at" << pacificFile->instances[j].instanceIndex;
        for(int attribute = 0; attribute < pacificFile->instances[j].attributes.size(); attribute++){
            pacificFile->instances[j].setAttributeDefault("Reward_PickupLink");
        }
    }
}

/*These don't work currently because we're loading all the
creature database files, not the metagame database. These will currently
never find the powercost and team attributes*/
void Randomizer::randomizeTeamColors(){
    int teamIndex = 0;
    uint shuffleValue = 0;
    std::vector<int> teamList;
    if(randSettings.balancedTeams){
        for(int i = 0; i <parent->dataHandler->miniconList.size(); i++){
            if(!parent->dataHandler->miniconList[i].hasVanillaPlacement){
                continue;
            }
            teamList.push_back(parent->dataHandler->miniconList[i].searchAttributes<int>("Team"));
        }
    }
    for(int i = 0; i <parent->dataHandler->miniconList.size(); i++){
        if(!parent->dataHandler->miniconList[i].hasVanillaPlacement){
            continue;
        }
        teamIndex = parent->dataHandler->miniconList[i].searchAttributes<int>("Team");
        if(randSettings.balancedTeams){
            /*Randomize the teams in a balanced way - there will be as many of each team in the final result as the base game*/
            shuffleValue = placemaster.bounded(teamList.size());
            parent->dataHandler->miniconList[i].setAttribute("Team", QString::number(teamList[shuffleValue]));
        } else {
            /*Otherwise, assign a random team value with no regard to team count.*/
            shuffleValue = placemaster.generate();
            qDebug() << Q_FUNC_INFO << "setting team value to" << shuffleValue%5 << "from" << shuffleValue;
            parent->dataHandler->miniconList[i].setAttribute("Team", QString::number(shuffleValue%5));
        }
    }
}


void Randomizer::randomizePowers(){
    int powerLevel = 0;
    uint shuffleValue = 0;
    for(int i = 0; i <parent->dataHandler->miniconList.size(); i++){
        shuffleValue = placemaster.generate();
        powerLevel = parent->dataHandler->miniconList[i].searchAttributes<int>("PowerCost");
        if(randSettings.balancedPower){
            /*Randomize the powers in a balanced way all powers will stay close to their original*/
            switch(powerLevel){
            case 10: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 20: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 30: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 40: //power can only go down or stay the same
                if(shuffleValue % 2 == 0){
                    parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                }
                break;
            default:
                break;
            }
        } else {
            /*Otherwise, assign a random power cost value with no regard to original value.*/
            qDebug() << Q_FUNC_INFO << "setting power value to" << (shuffleValue%5)*10 << "from" << shuffleValue;
            parent->dataHandler->miniconList[i].setAttribute("PowerCost", QString::number((shuffleValue%5)*10));
        }
    }
}

void Randomizer::randomizeAutobotStats(){
    parent->dataHandler->loadAutobots();
    /*Randomizable stats:
        Health
        Walking speed min and max
        Running speed min and max
        Gravity and flight gravity (maybe not these)
        Dash speed
        height
        lava damage (nobody will notice this one, make it extreme)
        sidekick energy retention level (how much health you get back from powerlinx)
        Zero to full acceleration time
        power capacity (minicon equip max)
        name (not ID, the enum value at the top. the name.) This does nothing as far as I can tell. The actual name is defined in the STRINGS file
        */
    for(int i = 0; i < parent->dataHandler->autobotList.size(); i++){
        //Health
        float currentHealth = parent->dataHandler->autobotList[i].searchAttributes<float>("Health");
        float changedHealth = 0;
        switch(placemaster.bounded(2)){
            case 0: //unchanged
            break;
            case 1: //lowered
            changedHealth = currentHealth + currentHealth*randomFloat(-0.3, -0.1);
            parent->dataHandler->autobotList[i].setAttribute("Health", QString::number(changedHealth, 'f', 2));
            break;
            case 2: //increased
            changedHealth = currentHealth + currentHealth*randomFloat(0.1, 0.3);
            parent->dataHandler->autobotList[i].setAttribute("Health", QString::number(changedHealth, 'f', 2));
            break;
            default: //unchanged
            break;
        }

        //Height
        float currentHeight = parent->dataHandler->autobotList[i].searchAttributes<float>("Height");
        float changedHeight = currentHeight + randomFloat(-1, 1);
        parent->dataHandler->autobotList[i].setAttribute("Height", QString::number(changedHeight, 'f', 2));

        //Name
        QStringList nameOptions = {"Hot Shot", "Red Alert", "Optimus Prime", "Hot Shot?"
            , "Red Alert?", "Optimus Prime?", "tohS toH", "trelA deR", "emirP sumitpO"
            , "Hot Rod", "Ratchet", "Convoy", "Hotter Shot", "Green Alert"
            , "Optimum Pride", "Rodimus Prime", "ALERTA", "Euro Truck Simulator"
        };
        parent->dataHandler->autobotList[i].setAttribute("Name", nameOptions[placemaster.bounded(nameOptions.size())]);

        //PowerCapacity
        int currentPower = parent->dataHandler->autobotList[i].searchAttributes<int>("PowerCapacity");
        int changedPower = currentPower + placemaster.bounded(-2, 2)*10;
        parent->dataHandler->autobotList[i].setAttribute("PowerCapacity", QString::number(changedPower));

        //Lava Damage Value
        //bounded of 4 means 20% chance - balance as needed.
        switch(placemaster.bounded(4)){
            case 0: //lethal lava land
            parent->dataHandler->autobotList[i].setAttribute("LavaDamageValue", QString::number(200, 'f', 2));
            break;
            default: //no change
            break;
        }

        //Dash Speed + time
        float currentSpeed = parent->dataHandler->autobotList[i].searchAttributes<float>("DashSpeed");
        float changedSpeed = currentSpeed + randomFloat(-10.0, 20.0);
        parent->dataHandler->autobotList[i].setAttribute("DashSpeed", QString::number(changedSpeed, 'f', 2));
        parent->dataHandler->autobotList[i].setAttribute("DashTime", QString::number(randomFloat(0.23, 0.43), 'f', 2));

    }
}

float Randomizer::randomFloat(float minimum, float maximum){
    return (placemaster.generateDouble() * (maximum - minimum)) + minimum;
}
