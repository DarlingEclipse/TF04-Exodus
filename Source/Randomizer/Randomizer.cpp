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

    for(int i = 0; i < parent->dataHandler->exodusData.miniconList.size(); i++){
        taMinicon* gameMinicon = parent->dataHandler->getGameMinicon(parent->dataHandler->exodusData.miniconList[i].miniconID);
        int team = static_cast<int>(gameMinicon->team);
        //int team = parent->dataHandler->exodusData.miniconList[i].searchAttributes<int>("Team");
        int power = gameMinicon->powerCost;
        //int power = parent->dataHandler->exodusData.miniconList[i].searchAttributes<int>("PowerCost");
        qDebug() << Q_FUNC_INFO << "minicon" << parent->dataHandler->exodusData.miniconList[i].pickupToSpawn << "has team:" << team << "and power cost" << power;
    }

    seed = 0;
    //generateDatacons = 0;

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
        qDebug() << Q_FUNC_INFO << "Datacon placement set to" << randSettings.generateDatacons;});
    checkDatacon->show();
    checkDatacon->toggle();
    //parent->currentModeWidgets.push_back(checkDatacon);

    SettingSlider* sliderBalancing = new SettingSlider("Progression Balancing", groupRandomizerOptions);
    sliderBalancing->setGeometry(QRect(QPoint(20,60), QSize(150,30)));
    QSlider::connect(sliderBalancing->slider, &QAbstractSlider::valueChanged, parent, [this](int value) {randSettings.progressionBalancing = value;});
    sliderBalancing->show();

    SettingSlider* sliderVariety = new SettingSlider("Location Challenge", groupRandomizerOptions);
    sliderVariety->setGeometry(QRect(QPoint(20,120), QSize(150,30)));
    QSlider::connect(sliderVariety->slider, &QAbstractSlider::valueChanged, parent, [this](int value) {randSettings.locationChallenge = value;});
    sliderVariety->show();

    SettingSlider* sliderSlipstream = new SettingSlider("Slipstream Difficulty", groupRandomizerOptions);
    sliderSlipstream->setGeometry(QRect(QPoint(20,180), QSize(150,30)));
    QSlider::connect(sliderSlipstream->slider, &QAbstractSlider::valueChanged, parent, [this](int value) {randSettings.slipstreamDifficulty = value;});
    sliderSlipstream->show();

    SettingSlider* sliderHighjump = new SettingSlider("Highjump Difficulty", groupRandomizerOptions);
    sliderHighjump->setGeometry(QRect(QPoint(20,240), QSize(150,30)));
    QSlider::connect(sliderHighjump->slider, &QAbstractSlider::valueChanged, parent, [this](int value) {randSettings.highjumpDifficulty = value;});
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

    for(int i = 0; i < parent->dataHandler->exodusData.customLocationList.size(); i++){
        qDebug() << Q_FUNC_INFO << "location name for" << i << ":" << parent->dataHandler->exodusData.customLocationList[i].name;
        QCheckBox *locationCheck = new QCheckBox(parent->dataHandler->exodusData.customLocationList[i].name, groupLocations);
        locationCheck->setGeometry(QRect(QPoint(20,20 + (40*i)), QSize(200,30)));
        QAbstractButton::connect(locationCheck, &QCheckBox::stateChanged, parent, [i, locationCheck, this]
            {parent->dataHandler->exodusData.customLocationList[i].enabled = locationCheck->isChecked();});
        locationCheck->setToolTip(parent->dataHandler->exodusData.customLocationList[i].description);
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
    parent->log("This option currently doesn't work. Other players will need to manually match your settings.");
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
    randSettings.slipstreamDifficulty = slipstreamCheck;
    randSettings.highjumpDifficulty = highjumpCheck;


    parent->log("Settings imported. Note that sliders will not change visibly.");
}

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
    qDebug() << Q_FUNC_INFO << "difficulties - slipstream:" << randSettings.slipstreamDifficulty << "highjump" << randSettings.highjumpDifficulty;
    qDebug() << Q_FUNC_INFO << "progression" << randSettings.progressionBalancing << "locations" << randSettings.locationChallenge;



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

    if(randSettings.randomizeLevelOrder){
        randomizeLevels();
    }

    for(int i = 0; i < parent->dataHandler->exodusData.loadedLevels.size(); i++){
        exEpisode currentEpisode = parent->dataHandler->exodusData.loadedLevels[i];
        availableLocations.insert(availableLocations.end(), currentEpisode.spawnLocations.begin(), currentEpisode.spawnLocations.end());
    }

    qDebug() << Q_FUNC_INFO << "Starting randomization. Available locations:" << availableLocations.size();

    parent->log("Randomizing progression Minicons...");
    placeSlipstream();
    qDebug() << "Step 1 complete";
    placeHighjump();
    qDebug() << "Step 2 complete";
    placeRangefinder();
    qDebug() << "Step 3 complete";
    placeShepherd();
    qDebug() << "Step 4 complete";
    if(randSettings.progressionBalancing == 0 && randSettings.locationChallenge < 4){
        placeStarterWeapon();
    }
    qDebug() << "Step 5 complete";

    parent->log("Randomizing remaining Minicons...");
    placeAll();
    qDebug() << "Step 6 complete";

    parent->log("Minicon placements randomized. Writing database files...");

    std::sort(placedLocations.begin(), placedLocations.end());

    editDatabases();
    parent->log("Writing spoiler log...");
    writeSpoilers();

    parent->log("Replacing randomized files...");
    randomFileReplacements();
    parent->log("Applying mods...");
    parent->modHandler->applyModifications();

    for(int i = 0; i < parent->modHandler->folderOptions.size(); i++){
        if(parent->modHandler->folderOptions[i].folderName == "TFA"){
            parent->modHandler->folderOptions[i].moddedSource = true;
        }
    }
    if(randSettings.autoBuild){
        parent->log("Building ISO...");
        parent->isoBuilder->packModded("Randomizer");
    }
    parent->log("Randomization process complete.");
    availableLocations.clear();
}

void Randomizer::randomizeLevels(){
    qDebug() << Q_FUNC_INFO << "Here's where we would randomize the level order.";
}

void Randomizer::randomFileReplacements(){
    for(int i = 0; i < parent->modHandler->replacementList.size(); i++){
        /*Uncomment this when we add a file replacement menu to the randomizer, otherwise it skips all replacements*/
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
        if((availableLocations[i].level <= World::MidAtlantic && randSettings.progressionBalancing < 4)
                || randSettings.progressionBalancing > 3){
            rangefinderLocations.push_back(availableLocations[i]);
        }
    }
    int locationNumber = placemaster.bounded(rangefinderLocations.size());
    qDebug() << Q_FUNC_INFO << "Placing Rangefinder at" << rangefinderLocations[locationNumber].uniqueID;
    placeMinicon(43, rangefinderLocations[locationNumber].uniqueID);
}

void Randomizer::placeStarterWeapon(){
    std::vector<exMinicon> starterMinicons;
    qDebug() << Q_FUNC_INFO << "Placing starter weapon. total minicon options:" << parent->dataHandler->exodusData.miniconList.size();
    for(int i = 0; i < parent->dataHandler->exodusData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking if minicon" << i << parent->dataHandler->exodusData.miniconList[i].pickupToSpawn << "is a weapon";
        if(parent->dataHandler->exodusData.miniconList[i].isWeapon){
            qDebug() << Q_FUNC_INFO << "it is, adding to list";
            starterMinicons.push_back(parent->dataHandler->exodusData.miniconList[i]);
        }
    }
    qDebug() << Q_FUNC_INFO << "A total of" << starterMinicons.size() << "minicons are available";
    int miniconNumber = placemaster.bounded(starterMinicons.size());
    qDebug() << Q_FUNC_INFO << "Placing starter weapon" << starterMinicons[miniconNumber].pickupToSpawn;
    placeMinicon(starterMinicons[miniconNumber].pickupID, 42069); //locationID tbd
}

void Randomizer::placeShepherd(){
    qDebug() << Q_FUNC_INFO << "Finding placement for shepherd. available locations:" << availableLocations.size();
    std::vector<exPickupLocation> shepherdLocations;
    for(int i = 0; i < availableLocations.size(); i++){
        if((parent->dataHandler->bunkerList.contains(availableLocations[i].uniqueID) && randSettings.progressionBalancing > 3)
                || (availableLocations[i].level == World::PacificIsland && randSettings.progressionBalancing == 3)
                || (randSettings.progressionBalancing == 2)
                || (availableLocations[i].level < World::Spaceship && randSettings.progressionBalancing < 2)){
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
    World placementLevel = World::Amazon;
    //looking through placedlocations since slipstream should already be placed
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].uniqueID == placementID){
            placementLevel  = placedLocations[i].level;
            //qDebug() << Q_FUNC_INFO << "prereq slipstream level found as" << placementLevel;
        }
    }
    //qDebug() << Q_FUNC_INFO << "determining prerequisite locations for" << miniconID << "and placement ID" << placementID;
    for(int i = 0; i < availableLocations.size(); i++){
        if((availableLocations[i].slipstreamDifficulty <= 3)
                && (availableLocations[i].level <= placementLevel)
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
    for(int i = 0; i < parent->dataHandler->exodusData.miniconList.size(); i++){
        if(parent->dataHandler->exodusData.miniconList[i].pickupID == miniconToPlace){
            //miniconList[i].placed = true;
            chosenMinicon = &parent->dataHandler->exodusData.miniconList[i];
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
        qDebug() << Q_FUNC_INFO << "found location ID" << placementID << "with linked location" << availableLocations[i].linkedLocationIDs;
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        std::vector<int> linkedLocations = availableLocations[i].linkedLocationIDs;
        availableLocations[i].assignPickup(chosenMinicon);
        placedLocations.push_back(availableLocations[i]);
        parent->dataHandler->exodusData.loadedLevels[static_cast<int>(availableLocations[i].level)].assignedMinicons++;
        availableLocations.erase(availableLocations.begin() + i);
        if(!linkedLocations.empty()){
            qDebug() << Q_FUNC_INFO << "searching for linked locations" << linkedLocations;
            bool foundLocation = false;
            for(int j = 0; j < availableLocations.size(); j++){
                //qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocation;
                if(std::find(linkedLocations.begin(), linkedLocations.end(), availableLocations[j].uniqueID) != linkedLocations.end()){
                    continue;
                }
                qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocations;
                foundLocation = true;
                availableLocations[j].assignPickup(chosenMinicon);
                placedLocations.push_back(availableLocations[j]);
                parent->dataHandler->exodusData.loadedLevels[static_cast<int>(availableLocations[j].level)].assignedMinicons++;
                //parent->dataHandler->gameData.levelList[availableLocations[j].gameID[0]].miniconCount++;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placementID << availableLocations[j].uniqueID << ":" << miniconToPlace << "vs" << availableLocations[j].assignedPickup()->pickupID;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placedLocations[placedLocations.size()-1].assignedPickup()->pickupID;
                availableLocations.erase(availableLocations.begin() + j);
            }
            if(!foundLocation){
                qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocations;
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
    for(int i = 0; i < parent->dataHandler->exodusData.dataconList.size(); i++){
        if(parent->dataHandler->exodusData.dataconList[i].dataID == dataconToPlace){
            chosenDatacon = &parent->dataHandler->exodusData.dataconList[i];
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
        qDebug() << Q_FUNC_INFO << "found location ID" << placementID << "with linked location" << availableLocations[i].linkedLocationIDs;
        qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
        std::vector<int> linkedLocations = availableLocations[i].linkedLocationIDs;
        availableLocations[i].assignPickup(chosenDatacon);
        placedLocations.push_back(availableLocations[i]);
        parent->dataHandler->exodusData.loadedLevels[static_cast<int>(availableLocations[i].level)].assignedDatacons++;
        //parent->dataHandler->gameData.levelList[availableLocations[i].gameID[0]].dataconCount++;
        qDebug() << Q_FUNC_INFO << "after assigning location" << availableLocations[i].uniqueID << "erasing location" << (availableLocations.begin() + i)->uniqueID;
        availableLocations.erase(availableLocations.begin() + i);
        if(!linkedLocations.empty()){
            qDebug() << Q_FUNC_INFO << "searching for linked location" << linkedLocations;
            bool foundLocation = false;
            for(int j = 0; j < availableLocations.size(); j++){
                qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocations;
                if(std::find(linkedLocations.begin(), linkedLocations.end(), availableLocations[j].uniqueID) != linkedLocations.end()){
                    continue;
                }
                qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocations;
                foundLocation = true;
                availableLocations[j].assignPickup(chosenDatacon);
                placedLocations.push_back(availableLocations[j]);
                parent->dataHandler->exodusData.loadedLevels[static_cast<int>(availableLocations[j].level)].assignedDatacons++;
                //parent->dataHandler->gameData.levelList[availableLocations[j].gameID[0]].dataconCount++;
                qDebug() << Q_FUNC_INFO << "confirming linked placement for locations" << placementID << availableLocations[j].uniqueID << ":" << dataconToPlace << "vs" << availableLocations[j].assignedPickup()->pickupID;
                qDebug() << Q_FUNC_INFO << "after assigning location" << availableLocations[j].uniqueID << "erasing location" << (availableLocations.begin() + j)->uniqueID;
                availableLocations.erase(availableLocations.begin() + j);
            }
            if(!foundLocation){
                qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocations;
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
    qDebug() << Q_FUNC_INFO << "Placing all remaining minicons. Currently" << parent->dataHandler->exodusData.miniconList.size() << "pickups to go through and place";
    //std::shuffle(std::begin(availableLocations), std::end(availableLocations), placemaster);
    //with this, we have three lists: the minicon list, the location list, and the list of placed locations
    //since we'll have more locations than minicons, we can shuffle the location list
    //and then assign minicons to it 1-to-1, copying those to the placed locations list.
    //then the placed locations combos would be written back to the database files
    int locationNumber = 0;
    int availableLevel = 0;
    for(int i = 0; i < parent->dataHandler->exodusData.miniconList.size(); i++){
        if(!parent->dataHandler->exodusData.miniconList[i].placed && parent->dataHandler->exodusData.miniconList[i].hasVanillaPlacement){
            qDebug() << Q_FUNC_INFO << "Placing minicon.";
            locationNumber = placemaster.bounded(availableLocations.size());

            //The below doesn't account for special placement counts (slipstream, highjump, etc) so some levels end up stacked anyway.
            availableLevel = static_cast<int>(availableLocations[locationNumber].level);
            while(parent->dataHandler->gameData.levelList[availableLevel].miniconCount > 6 /*|| (levelList[availableLevel].removedInstances < levelList[availableLevel].miniconCount)*/){
                qDebug() << Q_FUNC_INFO << "Level" << parent->dataHandler->exodusData.loadedLevels[availableLevel].logName << "has too many placements. Rerolling.";
                qDebug() << Q_FUNC_INFO << "current placements:" << parent->dataHandler->exodusData.loadedLevels[availableLevel].assignedMinicons;
                locationNumber = placemaster.bounded(availableLocations.size());
                availableLevel = static_cast<int>(availableLocations[locationNumber].level);
            }
            placeMinicon(parent->dataHandler->exodusData.miniconList[i].pickupID, availableLocations[locationNumber].uniqueID);
        } else {
            qDebug() << Q_FUNC_INFO << "Skipping placement for" << parent->dataHandler->exodusData.miniconList[i].pickupToSpawn;
        }
    }

    qDebug() << Q_FUNC_INFO << "running pre-datacon check";
    for(int i = 0; i < placedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "placed location" << placedLocations[i].uniqueID << "has assigned pickup" << placedLocations[i].assignedPickup()->pickupID;
    }
    /*for(int i = 0; i < availableLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "available location" << availableLocations[i].uniqueID << "has assigned pickup" << availableLocations[i].assignedPickup()->pickupID;
    }*/

    if(!randSettings.generateDatacons){
        return;
    }

    qDebug() << Q_FUNC_INFO << "Placing datacons.";
    for(int i = 0; i < parent->dataHandler->exodusData.dataconList.size(); i++){
        if(!parent->dataHandler->exodusData.dataconList[i].placed){
            qDebug() << Q_FUNC_INFO << "Placing datacon.";
            qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size();
            qDebug() << Q_FUNC_INFO << "reaches point 0. available locations:" << availableLocations.size();
            locationNumber = placemaster.bounded(availableLocations.size());
            qDebug() << Q_FUNC_INFO << "reaches point 1";
            availableLevel = static_cast<int>(availableLocations[locationNumber].level);
            qDebug() << Q_FUNC_INFO << "reaches point 2";
            while(parent->dataHandler->gameData.levelList[availableLevel].dataconCount > 12){
                qDebug() << Q_FUNC_INFO << "Level" << parent->dataHandler->exodusData.loadedLevels[availableLevel].logName << "has too many datacons. Rerolling.";
                qDebug() << Q_FUNC_INFO << "current datacons:" << parent->dataHandler->exodusData.loadedLevels[availableLevel].assignedDatacons << "available locations:" << availableLocations.size();
                locationNumber = placemaster.bounded(availableLocations.size());
                availableLevel = static_cast<int>(availableLocations[locationNumber].level);
            }
            qDebug() << Q_FUNC_INFO << "reaches point 3";
            placeDatacon(&parent->dataHandler->exodusData.dataconList[i], availableLocations[locationNumber]);
        } else {
            qDebug() << Q_FUNC_INFO << "Skipping placement.";
        }
    }
    qDebug() << Q_FUNC_INFO << "running post-datacon check";
    for(int i = 0; i < placedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << i << "placed location" << placedLocations[i].uniqueID << "has assigned pickup" << placedLocations[i].assignedPickup()->pickupID;
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
    qDebug() << Q_FUNC_INFO << "placing datacon at location ID" << location.uniqueID << "with linked location" << location.linkedLocationIDs;
    qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
    std::vector<int> linkedLocations = location.linkedLocationIDs;
    location.assignPickup(dataconToPlace);
    placedLocations.push_back(location);
    parent->dataHandler->gameData.levelList[static_cast<int>(location.level)].dataconCount++;
    removeLocation(location);
    if(!linkedLocations.empty()){
        qDebug() << Q_FUNC_INFO << "searching for linked location" << linkedLocations;
        bool foundLocation = false;
        for(int j = 0; j < availableLocations.size(); j++){
            //qDebug() << Q_FUNC_INFO << "checking linked location ID" << availableLocations[j].uniqueID << "vs" << linkedLocation;
            if(std::find(linkedLocations.begin(), linkedLocations.end(), availableLocations[j].uniqueID) != linkedLocations.end()){
                continue;
            }
            qDebug() << Q_FUNC_INFO << "found linked location ID" << linkedLocations;
            foundLocation = true;
            availableLocations[j].assignPickup(dataconToPlace);
            placedLocations.push_back(availableLocations[j]);
            parent->dataHandler->exodusData.loadedLevels[static_cast<int>(availableLocations[j].level)].assignedDatacons++;
            //parent->dataHandler->gameData.levelList[availableLocations[j].gameID[0]].dataconCount++;
            availableLocations.erase(availableLocations.begin() + j);
        }
        if(!foundLocation){
            qDebug() << Q_FUNC_INFO << "did not find location" << linkedLocations;
        }
    }
    qDebug() << Q_FUNC_INFO << "current placed pickups" << placedLocations.size() << "currently availlable locations" << availableLocations.size();
    dataconToPlace->placed = true;
}

void Randomizer::spoilMinicon(exPickupLocation placement, QTextStream& stream){
    //qDebug() << Q_FUNC_INFO << "attempting to place" << placement.uniqueID << "with assigned minicon"
    //         << placement.assignedMinicon << "out of" << miniconList.size() << "minicons";
    if(placement.assignedPickup()->pickupID == 3){
        qDebug() << Q_FUNC_INFO << "Attempted to spoil a datacon position. We don't do that (yet).";
        return;
    }
    QVector3D placedPosition = placement.position;
    qDebug() << Q_FUNC_INFO << "sending miniconID" << placement.assignedPickup()->pickupID << "to be spoiled";
    stream << placement.assignedPickup()->pickupToSpawn << " is located at " << placement.locationName << " id " << placement.uniqueID << ", or x" <<
                                                             placedPosition.x() << " y" << placedPosition.y() << " z" << placedPosition.z() << Qt::endl;
    placement.spoiled = true;
}

void Randomizer::spoilMinicon(int pickupID, QTextStream& stream){
    int placementIndex;
    qDebug() << Q_FUNC_INFO << "placedlocations count:" << placedLocations.size();
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].assignedPickup()->pickupID == pickupID){
            placementIndex = i;
            //placedLocations[i].spoiled = true;
        }
    }
    if(placedLocations[placementIndex].uniqueID == 0){
        parent->log("Could not find minicon " + QString::number(pickupID) + " at any placement." );
    }
    QVector3D placedPosition = placedLocations[placementIndex].position;
    qDebug() << Q_FUNC_INFO << "spoiling minicon" << pickupID << "at location" << placedLocations[placementIndex].uniqueID;
    QString miniconName = parent->dataHandler->getExodusMinicon(pickupID)->pickupToSpawn;
    stream << miniconName << " is located at " << placedLocations[placementIndex].levelName << "'s " << placedLocations[placementIndex].locationName << " id "
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
                if(placedLocations[i].assignedPickup()->pickupID == 1){
                    spoilMinicon(placedLocations[i], fileStream);
                }
            }
            for(int i = 0; i < placedLocations.size(); i++){
                if(placedLocations[i].assignedPickup()->pickupID == 1){
                    spoilMinicon(placedLocations[i], fileStream);
                }
            }
        }

        //write highjump location
        spoilMinicon(50, fileStream);
        //write all others, in order of level
        //qDebug() << Q_FUNC_INFO << "Writing all locations. Total placements:" << placedLocations.size();
        for(int i = 0; i < placedLocations.size(); i++){
            if(currentLevel != static_cast<int>(placedLocations[i].level)){
                currentLevel++;
                fileStream << Qt::endl;
                fileStream << Qt::endl;
                fileStream << "Level " << QString::number(currentLevel+1) << ": " << parent->dataHandler->exodusData.loadedLevels[currentLevel].logName << Qt::endl;
                fileStream << "Contains " << QString::number(parent->dataHandler->gameData.levelList[currentLevel].dataconCount) << " Datacons";
                fileStream << Qt::endl;
            }
            qDebug() << Q_FUNC_INFO << "writing location" << placedLocations[i].uniqueID << "with assigned minicon" << placedLocations[i].assignedPickup()->pickupID << "to location" << placedLocations[i].position;
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
    for(int i = 0; i<parent->dataHandler->gameData.levelList.size(); i++){
        qDebug() << Q_FUNC_INFO << "removing all pickups from" << parent->dataHandler->exodusData.loadedLevels[i].levelFile->fileName;
        parent->dataHandler->exodusData.loadedLevels[i].levelFile->removeAll("PickupPlaced");
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
        qDebug() << Q_FUNC_INFO << "checking level for location" << i << "uniqueID" << placedLocations[i].uniqueID << "level" << static_cast<int>(placedLocations[i].level);
        if(static_cast<int>(placedLocations[i].level) != level){
            qDebug() << Q_FUNC_INFO << "output path for this level will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->exodusData.loadedLevels[level].outputFileName + "/CREATURE.TDB");
            levelPath = parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->exodusData.loadedLevels[level].outputFileName;
            //get containing directory
            //create Randomizer folder in that directory
            qDebug() << Q_FUNC_INFO << "path being checked:" << levelPath;
            QDir checkDir(levelPath);
            if(!checkDir.exists()){
                checkDir.mkpath(".");
            }
            //since the placedLocations list should be sorted by level
            //this just saves on a couple hundred loops
            parent->dataHandler->exodusData.loadedLevels[level].levelFile->outputPath = levelPath + "/CREATURE.TDB";
            parent->dataHandler->exodusData.loadedLevels[level].levelFile->save("TDB");

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
        qDebug() << Q_FUNC_INFO << "checking level list size:" << parent->dataHandler->gameData.levelList.size();
        qDebug() << Q_FUNC_INFO << "does level have a file?:" << parent->dataHandler->exodusData.loadedLevels[level].levelFile->fileName;
        placedLocations[i].inputDatabaseInstance = parent->dataHandler->exodusData.loadedLevels[level].levelFile->addInstance(parent->dataHandler->createGamePickupPlaced(placedLocations[i]));
        //placedLocations[i].inputDatabaseInstance = parent->dataHandler->exodusData.loadedLevels[level].levelFile->addInstance(placedLocations[i]);
        placedLocations[i].locationName = tempNameStorage;
    }
    qDebug() << Q_FUNC_INFO << "Correcting Pacific island bunker links";
    fixBunkerLinks(level);
    qDebug() << Q_FUNC_INFO << "output path for this level will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->exodusData.loadedLevels[level].outputFileName + "/CREATURE.TDB");
    levelPath = parent->isoBuilder->copyOutputPath + "/TFA/LEVELS/EPISODES/" + parent->dataHandler->exodusData.loadedLevels[level].outputFileName;
    //get containing directory
    //create Randomizer folder in that directory
    qDebug() << Q_FUNC_INFO << "path being checked:" << levelPath;
    QDir checkDir(levelPath);
    if(!checkDir.exists()){
        checkDir.mkpath(".");
    }
    //since the placedLocations list should be sorted by level
    //this just saves on a couple hundred loops
    parent->dataHandler->exodusData.loadedLevels[level].levelFile->outputPath = levelPath + "/CREATURE.TDB";
    parent->dataHandler->exodusData.loadedLevels[level].levelFile->save("TDB");
    level++;


    bool metagameEdited = false;
    for(int i = 0; i<parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking databse file" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            qDebug() << Q_FUNC_INFO << "setting metagame file to:" << parent->databaseList[i]->fileName;
            parent->dataHandler->gameData.metagameFile = parent->databaseList[i];
        }
    }

    QStringList miniconTypes = {"Minicon", "MiniconDamageBonus", "MiniconArmor", "MiniconEmergencyWarpgate", "MiniconRangeBonus", "MiniconRegeneration"};
    if(randSettings.randomizePower || randSettings.randomizeTeams){
        //this could be an issue - some minicons are their own classes and inherit from minicon.
        metagameEdited = true;
        for(int i = 0; i < miniconTypes.size(); i++){
            parent->dataHandler->gameData.metagameFile->removeAll(miniconTypes[i]);
        }
        for(int i = 0; i < parent->dataHandler->exodusData.miniconList.size(); i++){
            dictItem itemToAdd = parent->dataHandler->createMetagameMinicon(parent->dataHandler->gameData.miniconList[i]);
            parent->dataHandler->gameData.metagameFile->addInstance(itemToAdd);
        }
    }

    if(randSettings.randomizeAutobotStats){
        metagameEdited = true;
        parent->dataHandler->gameData.metagameFile->removeAll("Autobot");
        for(int i = 0; i < parent->dataHandler->gameData.autobotList.size(); i++){
            parent->dataHandler->gameData.metagameFile->addInstance(parent->dataHandler->gameData.autobotList[i]);
        }
    }

    if(metagameEdited){
        qDebug() << Q_FUNC_INFO << "output path for METAGAME will be" << QString(parent->isoBuilder->copyOutputPath + "/TFA/METAGAME.TDB");
        QString metagamePath = parent->isoBuilder->copyOutputPath + "/TFA/METAGAME.TDB";
        parent->dataHandler->gameData.metagameFile->outputPath = metagamePath;
        parent->dataHandler->gameData.metagameFile->save("TDB");
    }

    return 0;
}

void Randomizer::fixBunkerLinks(int level){
    std::shared_ptr<DatabaseFile> pacificFile = parent->dataHandler->exodusData.loadedLevels[level].levelFile;
    std::vector<int> usedBunkers;
    qDebug() << Q_FUNC_INFO << "bunker file level name:" << pacificFile->fullFileName();
    for(int i = 0; i < placedLocations.size(); i++){
        if(placedLocations[i].bunkerID == 0 || placedLocations[i].assignedPickup()->pickupID != 3){
            //skipping if the assigned pickup is a minicon because the reward system only works with datacons - for now.
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
                pacificFile->instances[j].setAttribute("Reward_PickupLink", QString::number(placedLocations[i].inputDatabaseInstance));
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

void Randomizer::randomizeTeamColors(){
    /*this should be reworked, all the casting from int to enum and back makes me think there's a better way*/
    int teamIndex = 0;
    uint shuffleValue = 0;
    std::vector<int> teamList;
    if(randSettings.balancedTeams){
        for(int i = 0; i <parent->dataHandler->exodusData.miniconList.size(); i++){
            if(!parent->dataHandler->exodusData.miniconList[i].hasVanillaPlacement){
                continue;
            }
            taMinicon* gameMinicon = parent->dataHandler->getGameMinicon(parent->dataHandler->exodusData.miniconList[i].miniconID);
            teamList.push_back(static_cast<int>(gameMinicon->team));
        }
    }
    for(int i = 0; i <parent->dataHandler->exodusData.miniconList.size(); i++){
        if(!parent->dataHandler->exodusData.miniconList[i].hasVanillaPlacement){
            continue;
        }
        taMinicon* gameMinicon = parent->dataHandler->getGameMinicon(parent->dataHandler->exodusData.miniconList[i].miniconID);
        teamIndex = static_cast<int>(gameMinicon->team);
        if(randSettings.balancedTeams){
            /*Randomize the teams in a balanced way - there will be as many of each team in the final result as the base game*/
            shuffleValue = placemaster.bounded(teamList.size());
            gameMinicon->team = static_cast<Team>(teamList[shuffleValue]);
            //parent->dataHandler->exodusData.miniconList[i].setAttribute("Team", QString::number(teamList[shuffleValue]));
        } else {
            /*Otherwise, assign a random team value with no regard to team count.*/
            shuffleValue = placemaster.generate();
            qDebug() << Q_FUNC_INFO << "setting team value to" << shuffleValue%5 << "from" << shuffleValue;
            gameMinicon->team = static_cast<Team>(shuffleValue%5);
            //parent->dataHandler->exodusData.miniconList[i].setAttribute("Team", QString::number(shuffleValue%5));
        }
    }
}


void Randomizer::randomizePowers(){
    int powerLevel = 0;
    uint shuffleValue = 0;
    for(int i = 0; i <parent->dataHandler->exodusData.miniconList.size(); i++){
        shuffleValue = placemaster.generate();
        taMinicon* gameMinicon = parent->dataHandler->getGameMinicon(parent->dataHandler->exodusData.miniconList[i].miniconID);
        powerLevel = powerLevel = gameMinicon->powerCost;
        //powerLevel = parent->dataHandler->exodusData.miniconList[i].searchAttributes<int>("PowerCost");
        if(randSettings.balancedPower){
            /*Randomize the powers in a balanced way all powers will stay close to their original*/
            switch(powerLevel){
            case 10: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    gameMinicon->powerCost = powerLevel - 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    gameMinicon->powerCost = powerLevel + 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 20: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    gameMinicon->powerCost = powerLevel - 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    gameMinicon->powerCost = powerLevel + 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 30: //power can go up, down, or stay the same
                if(shuffleValue % 3 == 0){
                    gameMinicon->powerCost = powerLevel - 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                } else if(shuffleValue % 3 == 1) {
                    gameMinicon->powerCost = powerLevel + 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel + 10));
                }
                break;
            case 40: //power can only go down or stay the same
                if(shuffleValue % 2 == 0){
                    gameMinicon->powerCost = powerLevel - 10;
                    //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number(powerLevel - 10));
                }
                break;
            default:
                break;
            }
        } else {
            /*Otherwise, assign a random power cost value with no regard to original value.*/
            qDebug() << Q_FUNC_INFO << "setting power value to" << (shuffleValue%5)*10 << "from" << shuffleValue;
            gameMinicon->powerCost = (shuffleValue%5)*10;
            //parent->dataHandler->exodusData.miniconList[i].setAttribute("PowerCost", QString::number((shuffleValue%5)*10));
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
    for(int i = 0; i < parent->dataHandler->gameData.autobotList.size(); i++){
        //Health
        float currentHealth = parent->dataHandler->gameData.autobotList[i].searchAttributes<float>("Health");
        float changedHealth = 0;
        switch(placemaster.bounded(2)){
            case 0: //unchanged
            break;
            case 1: //lowered
            changedHealth = currentHealth + currentHealth*randomFloat(-0.3, -0.1);
            parent->dataHandler->gameData.autobotList[i].setAttribute("Health", QString::number(changedHealth, 'f', 2));
            break;
            case 2: //increased
            changedHealth = currentHealth + currentHealth*randomFloat(0.1, 0.3);
            parent->dataHandler->gameData.autobotList[i].setAttribute("Health", QString::number(changedHealth, 'f', 2));
            break;
            default: //unchanged
            break;
        }

        //Height
        float currentHeight = parent->dataHandler->gameData.autobotList[i].searchAttributes<float>("Height");
        float changedHeight = currentHeight + randomFloat(-1, 1);
        parent->dataHandler->gameData.autobotList[i].setAttribute("Height", QString::number(changedHeight, 'f', 2));

        //Name
        QStringList nameOptions = {"Hot Shot", "Red Alert", "Optimus Prime", "Hot Shot?"
            , "Red Alert?", "Optimus Prime?", "tohS toH", "trelA deR", "emirP sumitpO"
            , "Hot Rod", "Ratchet", "Convoy", "Hotter Shot", "Green Alert"
            , "Optimum Pride", "Rodimus Prime", "ALERTA", "Euro Truck Simulator"
        };
        parent->dataHandler->gameData.autobotList[i].setAttribute("Name", nameOptions[placemaster.bounded(nameOptions.size())]);

        //PowerCapacity
        int currentPower = parent->dataHandler->gameData.autobotList[i].searchAttributes<int>("PowerCapacity");
        int changedPower = currentPower + placemaster.bounded(-2, 2)*10;
        parent->dataHandler->gameData.autobotList[i].setAttribute("PowerCapacity", QString::number(changedPower));

        //Lava Damage Value
        //bounded of 4 means 20% chance - balance as needed.
        switch(placemaster.bounded(4)){
            case 0: //lethal lava land
            parent->dataHandler->gameData.autobotList[i].setAttribute("LavaDamageValue", QString::number(200, 'f', 2));
            break;
            default: //no change
            break;
        }

        //Dash Speed + time
        float currentSpeed = parent->dataHandler->gameData.autobotList[i].searchAttributes<float>("DashSpeed");
        float changedSpeed = currentSpeed + randomFloat(-10.0, 20.0);
        parent->dataHandler->gameData.autobotList[i].setAttribute("DashSpeed", QString::number(changedSpeed, 'f', 2));
        parent->dataHandler->gameData.autobotList[i].setAttribute("DashTime", QString::number(randomFloat(0.23, 0.43), 'f', 2));

    }
}

float Randomizer::randomFloat(float minimum, float maximum){
    return (placemaster.generateDouble() * (maximum - minimum)) + minimum;
}
