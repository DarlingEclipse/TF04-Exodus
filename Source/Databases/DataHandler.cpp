#include "Headers/Main/mainwindow.h"

/*std::vector<Pickup> convertPickups(std::vector<dictItem> itemList){

}

std::vector<Minicon> convertMinicons(std::vector<dictItem> itemList){

}

std::vector<Warpgate> convertWarpgates(std::vector<dictItem> itemList){
    std::vector<Warpgate> warpgateList;
    for(int i = 0; i < itemList.size(); i++){
        if(itemList[i].name == "CreatureWarpGate"){
            Warpgate nextWarpgate = Warpgate(itemList[i]);
            warpgateList.push_back(nextWarpgate);
        }
    }
    return warpgateList;
}*/


Pickup::Pickup(dictItem copyItem){
    dataID = 99;
    this->name = copyItem.name;
    this->instanceIndex = copyItem.instanceIndex;
    this->pickupToSpawn = copyItem.searchAttributes<QString>("Name");
    this->attributes = copyItem.attributes;
    for(int i = 0; i < copyItem.attributes.size(); i++){
        if(copyItem.attributes[i]->name == "PickupToSpawn"){
            this->pickupToSpawn = copyItem.attributes[i]->display();
            this->enumID = copyItem.attributes[i]->intValue();
        }
        if(copyItem.attributes[i]->name == "ProductionArt" && !this->isMinicon()){
            this->pickupToSpawn = copyItem.attributes[i]->display();
            this->dataID = copyItem.attributes[i]->intValue();
        }
    }
    qDebug() << Q_FUNC_INFO << "adding pickup" << copyItem.instanceIndex << "name" << copyItem.name << "|" << pickupToSpawn << "has pickupID" << enumID << "is minicon?" << isMinicon();
    placed = false;
}

Pickup::Pickup(){
    dataID = 99;
    isWeapon = false;
    setAttribute("Position", "0, 0, 0");
}

exPickup::exPickup(taPickupPlaced copyItem){
    this->pickupID = static_cast<int>(copyItem.pickupToSpawn);
    this->placed = false;
    this->dataID = static_cast<int>(copyItem.productArt);
}

exPickup::exPickup(dictItem copyItem){
    this->pickupID = copyItem.searchAttributes<int>("PickupToSpawn");
    this->pickupToSpawn = copyItem.searchAttributes<QString>("PickupToSpawn");
    this->dataID = copyItem.searchAttributes<int>("ProductionArt");
    this->placed = false;
}

exPickup::exPickup(){
    this->pickupID = 99;
    this->pickupToSpawn = "";
    this->dataID = 99;
    this->placed = false;
}

taMinicon::taMinicon(){
    this->name = "UNLOADED";
    this->minicon = MiniconNames::LIST;
}

taMinicon::taMinicon(dictItem copyItem){

    this->activationType = static_cast<ActivationType>(copyItem.searchAttributes<int>("ActivationType"));
    this->chargeDrainRate_Commander = copyItem.searchAttributes<float>("ChargeDrainMultiplier_Commander");
    this->chargeDrainRate_Veteran = copyItem.searchAttributes<float>("ChargeDrainMultiplier_Veteran");
    this->chargeDrainTime = copyItem.searchAttributes<float>("ChargeDrainTime");
    this->coolDownTime = copyItem.searchAttributes<float>("CoolDownTime");
    this->coolDownTimeDepleted = copyItem.searchAttributes<float>("CoolDownTimeDepleted");
    this->crosshairIndex = static_cast<CrosshairIndex>(copyItem.searchAttributes<int>("CrosshairIndex"));
    this->equipInHQ = copyItem.searchAttributes<bool>("EquipInHQ");
    this->icon = static_cast<Icon>(copyItem.searchAttributes<int>("Icon"));
    this->minicon = static_cast<MiniconNames>(copyItem.searchAttributes<int>("Minicon"));
    this->minimumChargeToUse = copyItem.searchAttributes<float>("MinimumChargeToUse");
    this->minimumChargeToUsePerShot = copyItem.searchAttributes<float>("MinimumChargeToUsePerShot");
    this->name = copyItem.searchAttributes<QString>("Name");
    this->nodeToKeepIndex = copyItem.searchAttributes<int>("NodeToKeepIndex");
    this->paletteIndex = copyItem.searchAttributes<int>("PaletteIndex");
    this->powerCost = copyItem.searchAttributes<int>("PowerCost");
    this->rechargeTime = copyItem.searchAttributes<float>("RechargeTime");
    this->recoilType = static_cast<RecoilType>(copyItem.searchAttributes<int>("RecoilType"));
    this->restrictToButton = static_cast<RestrictToButton>(copyItem.searchAttributes<int>("RestrictToButton"));
    this->segments = copyItem.searchAttributes<int>("Segments");
    this->sidekickCoolDownTime = copyItem.searchAttributes<float>("SidekickCoolDownTime");
    this->sidekickRechargeTime = copyItem.searchAttributes<float>("SidekickRechargeTime");
    this->sidekickSegments = copyItem.searchAttributes<int>("SidekickSegments");
    this->slot = static_cast<Slot>(copyItem.searchAttributes<int>("Slot"));
    this->team = static_cast<Team>(copyItem.searchAttributes<int>("Team"));
    this->toneLibrary = copyItem.searchAttributes<QString>("ToneLibrary");
}

exMinicon::exMinicon(dictItem copyItem){
    this->pickupID = copyItem.searchAttributes<int>("PickupID");
    this->miniconID = copyItem.searchAttributes<int>("MiniconID");
    this->pickupToSpawn = copyItem.searchAttributes<QString>("PickupToSpawn");
    this->hasVanillaPlacement = copyItem.searchAttributes<bool>("hasVanillaPlacement");
    this->dataID = 99; //this shouldn't exist?
    this->placed = false;
    this->isWeapon = copyItem.searchAttributes<bool>("Weapon");
    this->isExplosive = copyItem.searchAttributes<bool>("Explosive");
}

exPickupLocation::exPickupLocation(dictItem copyItem){
    level = static_cast<World>(copyItem.searchAttributes<int>("Level"));
    levelName = copyItem.searchAttributes<QString>("Level");
    inputDatabaseInstance = copyItem.searchAttributes<int>("InstanceIndex");
    //uniqueID = copyItem.searchAttributes<int>("ExodusID");
    uniqueID = copyItem.instanceIndex;

    slipstreamDifficulty = copyItem.searchAttributes<int>("SlipstreamDifficulty");
    highjumpDifficulty = copyItem.searchAttributes<int>("HighjumpDifficulty");
    locationName = copyItem.searchAttributes<QString>("LocationName");
    spawnEvent = copyItem.searchAttributes<QString>("SpawnEvent");
    bunkerID = copyItem.searchAttributes<int>("BunkerID");
    position = copyItem.searchAttributes<QVector3D>("Position");
    requiresExplosive = copyItem.searchAttributes<bool>("RequiresExplosive");
    maximumGenerationDifficulty = copyItem.searchAttributes<int>("MaxGenDifficulty");
    minimumGenerationDifficulty = copyItem.searchAttributes<int>("MinGenDifficulty");

    //linkedLocationIDs = copyItem.searchAttributes<std::vector<int>>("LinkedLocations");
    linkedLocationIDs = {}; //this needs some work on the taData side of things - the template function and variant system is a mess and I hate it
    alternativeRequirements = {}; //same as above

}

exEpisode::exEpisode(dictItem copyItem){
    logName = copyItem.searchAttributes<QString>("LogName");
    outputFileName = copyItem.searchAttributes<QString>("OutputFileName");

    assignedMinicons = 0;
    assignedDatacons = 0;
}

exEpisode::exEpisode(){
    logName = "UNLOADED LEVEL";
    outputFileName = "00_TESTLEVEL";
    assignedMinicons = 0;
    assignedDatacons = 0;
}

void exMinicon::setCreature(exPickup copyItem){
    this->pickupID = copyItem.pickupID;
    //this->instanceIndex = copyItem.instanceIndex;
    //this->isWeapon = copyItem.isWeapon;
    placed = false;
}

exMinicon::exMinicon(){
    //instanceIndex = 0;
    isWeapon = false;
    pickupToSpawn = "Default Shockpunch";
    pickupID = 27;
    miniconID = 0; //unknown for now, find shockpunch in the list
    //setAttribute("Position", "0, 0, 0");
}

exEpisode* DataHandler::getExodusEpisode(QString levelToGet){
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        if(exodusData.loadedLevels[i].outputFileName == levelToGet){
            return &exodusData.loadedLevels[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << levelToGet << "was not found in the Exodus data";
    return nullptr;
}

taEpisode* DataHandler::getGameEpisode(QString levelToGet){
    QStringList splitPath;
    for(int i = 0; i < gameData.levelList.size(); i++){
        splitPath = gameData.levelList[i].directoryName.split("/");
        if(splitPath.contains(levelToGet)){
            qDebug() << Q_FUNC_INFO << "Matched main directory for game data.";
            return &gameData.levelList[i];
        }
        splitPath = gameData.levelList[i].alternativeDirectoryName.split("/");
        if(splitPath.contains(levelToGet)){
            qDebug() << Q_FUNC_INFO << "Matched alternate directory for game data.";
            return &gameData.levelList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << levelToGet << "was not found in the Game data";
    return nullptr;
}

/*Currently only search by pickupID instead of miniconID*/
exMinicon* DataHandler::getExodusMinicon(int searchID){
    if(searchID == 3){
        qDebug() << Q_FUNC_INFO << "Searching for a datacon instead of minicon. This shouldn't happen.";
        return nullptr;
    }
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(exodusData.miniconList[i].pickupID == searchID){
            return &exodusData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

exMinicon* DataHandler::getExodusMinicon(QString searchName){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(exodusData.miniconList[i].pickupToSpawn == searchName){
            return &exodusData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

taMinicon* DataHandler::getGameMinicon(int searchID){
    /*There's some complication with this one - the METAGMAE minicon list does not match the CREATURE taPickupPlaced PickupToSpawn list. Another way needs to be figured out for linking these two types.*/
    if(searchID == 3){
        qDebug() << Q_FUNC_INFO << "Searching for a datacon instead of minicon. This shouldn't happen.";
        return nullptr;
    }
    for(int i = 0; i < gameData.miniconList.size(); i++){
        if(static_cast<int>(gameData.miniconList[i].minicon) == searchID){
            return &gameData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

taMinicon* DataHandler::getGameMinicon(QString searchName){
    for(int i = 0; i < gameData.miniconList.size(); i++){
        if(gameData.miniconList[i].name == searchName){
            return &gameData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

void DataHandler::resetMinicons(){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        exodusData.miniconList[i].placed = false;
    }
}

void DataHandler::resetDatacons(){
    for(int i = 0; i < exodusData.dataconList.size(); i++){
        exodusData.dataconList[i].placed = false;
    }
}

void DataHandler::resetLevels(){
    for(int i = 0; i < gameData.levelList.size(); i++){
        gameData.levelList[i].dataconCount = 0;
        gameData.levelList[i].miniconCount = 0;
    }
}

exWarpgate::exWarpgate(dictItem copyItem){
    this->instanceIndex = copyItem.instanceIndex;
    this->position = copyItem.position;
    this->x_position = copyItem.position.x();
    this->y_position = copyItem.position.y();
    this->z_position = copyItem.position.z();
    this->attributes = copyItem.attributes;
    for(int i = 0; i < copyItem.attributes.size(); i++){
        if(copyItem.attributes[i]->name == "WarpGateNumber"){
            this->name = copyItem.attributes[i]->display();
        }
    }
}

exWarpgate::exWarpgate(){
    this->x_position = 0;
    this->y_position = 0;
    this->z_position = 0;
}

/*taMinicon::taMinicon(){
    activationType = static_cast<enum ActivationType>(1); //I don't like this, but that would be how we get them from the databases
    activationType = ActivationType::AlwaysOn; //same as above
}*/

taPickupPlaced::taPickupPlaced(dictItem fromItem){

}

bool exEpisode::addLocation(exPickupLocation locationToAdd){
    if(locationToAdd.levelName == logName){
        spawnLocations.push_back(locationToAdd);
        return true;
    }
    return false;
}

void DataHandler::loadLevels(){
    exodusData.loadedLevels = convertInstances<exEpisode>(exodusData.dataFile->sendInstances("exEpisode"));


    std::vector<exPickupLocation> loadedLocations = convertInstances<exPickupLocation>(exodusData.dataFile->sendInstances("exPickupLocation"));

    for(int i = 0; i < exodusData.customLocationList.size(); i++){
        if(exodusData.customLocationList[i].enabled){
            for(int j = 0; j < exodusData.customLocationList[i].locationList.size(); j++){
                loadedLocations.push_back(exodusData.customLocationList[i].locationList[j]);
            }
        }
    }

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        for(int j = 0; j < loadedLocations.size(); j++){
            exodusData.loadedLevels[i].addLocation(loadedLocations[j]);
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded locations:" << loadedLocations.size();

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        for(int j = 0; j < exodusData.loadedLevels[i].spawnLocations.size(); j++){
            QVector3D debugPosition = exodusData.loadedLevels[i].spawnLocations[j].position;
            qDebug() << Q_FUNC_INFO << i << " " << exodusData.loadedLevels[i].spawnLocations[j].uniqueID << "  "
                     << exodusData.loadedLevels[i].spawnLocations[j].linkedLocationIDs << "    " << static_cast<int>(exodusData.loadedLevels[i].spawnLocations[j].level) << "    "
                     << exodusData.loadedLevels[i].spawnLocations[j].locationName << "    " << debugPosition.x() << "   " << debugPosition.y() << "  " << debugPosition.z();
        }
    }

}

dictItem DataHandler::createGamePickupPlaced(exPickupLocation location){
    exEpisode targetLevel = exodusData.loadedLevels[static_cast<int>(location.level)];
    dictItem convertedData;
    convertedData.name = "PickupPlaced";
    convertedData.attributes = targetLevel.levelFile->generateAttributes("PickupPlaced");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    convertedData.setAttribute("Position", QString::number(location.position.x()) + ", " + QString::number(location.position.y()) + ", " + QString::number(location.position.z()));

    convertedData.setAttribute("PickupToSpawn", QString::number(location.assignedPickup()->pickupID));

    if(!location.assignedPickup()->isMinicon()){
        qDebug() << Q_FUNC_INFO << "Datacon - setting production art value to" << location.assignedPickup()->dataID << "from" << location.assignedPickup()->pickupToSpawn;
        convertedData.setAttribute("ProductionArt", QString::number(location.assignedPickup()->dataID));
    }

    if(location.spawnEvent != ""){
        convertedData.setAttribute("SpawnEvent", location.spawnEvent);
    }

    for(int i = 0; i < convertedData.attributes.size(); i++){
        qDebug() << Q_FUNC_INFO << "attribute" << i << "name" << convertedData.attributes[i]->name << "value" << convertedData.attributes[i]->display();
    }

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(taMinicon minicon){

    dictItem convertedData;
    convertedData.name = "Minicon";
    convertedData.attributes = gameData.metagameFile->generateAttributes("Minicon");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    convertedData.setAttribute("ActivationType", QString::number(static_cast<int>(minicon.activationType)));
    convertedData.setAttribute("ChargeDrainMultiplier_Commander", QString::number(minicon.chargeDrainRate_Commander));
    convertedData.setAttribute("ChargeDrainMultiplier_Veteran", QString::number(minicon.chargeDrainRate_Veteran));
    convertedData.setAttribute("ChargeDrainTime", QString::number(minicon.chargeDrainTime));
    convertedData.setAttribute("CoolDownTime", QString::number(minicon.coolDownTime));
    convertedData.setAttribute("CoolDownTimeDepleted", QString::number(minicon.coolDownTimeDepleted));
    convertedData.setAttribute("CrosshairIndex", QString::number(static_cast<int>(minicon.crosshairIndex)));
    convertedData.setAttribute("EquipInHQ", QString::number(minicon.equipInHQ));
    convertedData.setAttribute("Icon", QString::number(static_cast<int>(minicon.icon)));
    convertedData.setAttribute("Minicon", QString::number(static_cast<int>(minicon.minicon)));
    convertedData.setAttribute("MinimumChargeToUse", QString::number(minicon.minimumChargeToUse));
    convertedData.setAttribute("MinimumChargeToUsePerShot", QString::number(minicon.minimumChargeToUsePerShot));
    convertedData.setAttribute("Name", minicon.name);
    convertedData.setAttribute("NodeToKeepIndex", QString::number(minicon.nodeToKeepIndex));
    convertedData.setAttribute("PaletteIndex", QString::number(minicon.paletteIndex));
    convertedData.setAttribute("PowerCost", QString::number(minicon.powerCost));
    convertedData.setAttribute("RechargeTime", QString::number(minicon.rechargeTime));
    convertedData.setAttribute("RecoilType", QString::number(static_cast<int>(minicon.recoilType)));
    convertedData.setAttribute("RestrictToButton", QString::number(static_cast<int>(minicon.restrictToButton)));
    convertedData.setAttribute("Segments", QString::number(minicon.segments));
    convertedData.setAttribute("SidekickCoolDownTime", QString::number(minicon.sidekickCoolDownTime));
    convertedData.setAttribute("SidekickRechargeTime", QString::number(minicon.sidekickRechargeTime));
    convertedData.setAttribute("SidekickSegments", QString::number(minicon.sidekickSegments));
    convertedData.setAttribute("Slot", QString::number(static_cast<int>(minicon.slot)));
    convertedData.setAttribute("Team", QString::number(static_cast<int>(minicon.team)));
    convertedData.setAttribute("ToneLibrary", minicon.toneLibrary);

    return convertedData;

}

void DataHandler::loadMinicons(){


    /*Move the metagame and exodusmain file searches to datahandler initialization*/
    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            gameData.metagameFile = parent->databaseList[i];
        }
    }

    if(gameData.metagameFile == nullptr){
        parent->log("Unable to find file METAGAME.TDB. Minicon metadata was not loaded.");
    }

    QStringList miniconTypes = {"Minicon", "MiniconDamageBonus", "MiniconArmor", "MiniconEmergencyWarpgate", "MiniconRangeBonus", "MiniconRegeneration"};

    std::vector<taMinicon> metagameMinicons;
    for(int type = 0; type < miniconTypes.size(); type++){
        metagameMinicons = convertInstances<taMinicon>(gameData.metagameFile->sendInstances(miniconTypes[type]));
        gameData.miniconList.insert(gameData.miniconList.end(), metagameMinicons.begin(), metagameMinicons.end());
    }

    qDebug() << Q_FUNC_INFO << "Total loaded minicons:" << exodusData.miniconList.size();
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "loaded minicon" << i << "is named" << exodusData.miniconList[i].pickupToSpawn;
    }

    //move this to DataHandler's initialization
    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "DATA-EXODUS_MAIN"){
            exodusData.dataFile = parent->databaseList[i];
        }
    }


    exodusData.miniconList = convertInstances<exMinicon>(exodusData.dataFile->sendInstances("exMinicon"));

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        std::vector<taPickupPlaced> filePickupsBase = convertInstances<taPickupPlaced>(exodusData.loadedLevels[i].levelFile->sendInstances("PickupPlaced"));
        gameData.pickupList.insert(gameData.pickupList.end(), filePickupsBase.begin(), filePickupsBase.end());
    }

    qDebug() << Q_FUNC_INFO << "Remaining pickups to process (should be 0):" << exodusData.pickupList.size();

    qDebug() << Q_FUNC_INFO << "Total loaded minicons:" << exodusData.miniconList.size();
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << exodusData.miniconList[i].pickupID << "  " << exodusData.miniconList[i].pickupToSpawn << "    "
                 << exodusData.miniconList[i].dataID << " is weapon:" << exodusData.miniconList[i].isWeapon;
    }



    //std::sort(miniconList.begin(), miniconList.end());
    //std::sort(dataconList.begin(), dataconList.end());
}

void DataHandler::loadAutobots(){
    std::shared_ptr<DatabaseFile> metagameFile;
    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            metagameFile = parent->databaseList[i];
        }
    }

    gameData.autobotList = metagameFile->sendInstances("Autobot");
}

void DataHandler::loadDatacons(){
    foreach(exPickup currentPickup, exodusData.pickupList){
        bool dataconIsLoaded = false;
        if(currentPickup.isMinicon()){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "pickup properties for:" << currentPickup.pickupToSpawn << "pickupID" << currentPickup.pickupID << "dataID" << currentPickup.dataID;
        dataconIsLoaded = dataconLoaded(currentPickup.dataID);
        qDebug() << Q_FUNC_INFO << "already loaded?" << dataconIsLoaded;
        if (!dataconIsLoaded && currentPickup.pickupID == 3){
            //we now know it's a datacon (but still check to be sure). if it hasn't been loaded, add it to the datacon list
            exodusData.dataconList.push_back(currentPickup);
        } else if (dataconIsLoaded && currentPickup.pickupID == 3){
            //if it has been loaded, just skip it
            continue;
        } else {
            //this should never happen, logically, but better safe than confused.
            //put some debugs here, just in case
            continue;
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded datacons:" << exodusData.dataconList.size();
    for(int i = 0; i < exodusData.dataconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << exodusData.dataconList[i].pickupID << "  " << exodusData.dataconList[i].pickupToSpawn << "    "
                 << exodusData.dataconList[i].dataID;
    }
}

void DataHandler::loadCustomLocations(){
    //this is really making me want to handle these as database files
    //actually, doing them this way with the header is nice for adding the
    //description as a tooltip later for location packs
    QString modPath = QCoreApplication::applicationDirPath() + "/Locations/";
    QDir modFolder(modPath);
    QDirIterator modIterator(modFolder.absolutePath());
    qDebug() << Q_FUNC_INFO << "next file info:" << modIterator.nextFileInfo().fileName() << "from path" << modFolder.absolutePath();
    bool headerFinished = false;
    TextProperty modProperty;
    QStringList propertyOptions = {"File Version", "Name", "Author", "Description", "Location Count", "Level", "Location Name", "Coordinates", "Location ID", "Linked Locations", "Highjump Difficulty", "Slipstream Difficulty"};
    int modVersion = 0;

    while (modIterator.hasNext()){
        QFile currentModFile = modIterator.next();
        qDebug() << Q_FUNC_INFO << "Current file" << currentModFile.fileName();
        if (currentModFile.open(QIODevice::ReadOnly)){
            qDebug() << Q_FUNC_INFO << "Reading file";
            FileData modBuffer;
            modBuffer.dataBytes = currentModFile.readAll();
            modBuffer.input = true;
            exCustomLocation currentLocations;
            headerFinished = false;
            int targetLevel = 8;
            int locationValue = 0;
            while(!headerFinished){
                modProperty = modBuffer.readProperty();
                qDebug() << Q_FUNC_INFO << "test property type:" << modProperty.name << "with value:" << modProperty.readValue;
                switch(propertyOptions.indexOf(modProperty.name)){
                case 0: //File Version
                    modVersion = modProperty.readValue.toInt();
                    break;
                case 1: //Name
                    currentLocations.name = modProperty.readValue;
                    break;
                case 2: //Author
                    currentLocations.author = modProperty.readValue;
                    break;
                case 3: //Description
                    currentLocations.description = modProperty.readValue;
                    //for human use only, for now. tooltips later.
                    break;
                case 4: //Location count
                    currentLocations.locationCount = modProperty.readValue.toInt();
                    headerFinished = true;
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "Unknown header property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                }
            }
            for(int i = 0; i <currentLocations.locationCount; i++){
                exPickupLocation customLocation = exPickupLocation();
                customLocation.level = static_cast<World>(targetLevel);
                //double-check that the below doesn't need to find the specific database for the target level
                //customLocation.attributes = levelList[0].levelFile->generateAttributes("PickupPlaced");
                bool readingLocation = true;
                while(readingLocation){
                    modProperty = modBuffer.readProperty();
                    switch(propertyOptions.indexOf(modProperty.name)){
                        case 5: //Level
                            for(int i = 0; i < gameData.levelList.size(); i++){
                                qDebug() << Q_FUNC_INFO << "comparing level" << gameData.levelList[i].name << "to" << modProperty.readValue;
                                if(gameData.levelList[i].name == modProperty.readValue){
                                    qDebug() << Q_FUNC_INFO << "comparing level: match";
                                    customLocation.level = static_cast<World>(i);
                                }
                            }
                            if(customLocation.level == World::Invalid){
                                qDebug() << Q_FUNC_INFO << "Invalid level:" << modProperty.readValue;
                            }
                            break;
                        case 6: //Location Name
                            customLocation.locationName = modProperty.readValue;
                            qDebug() << Q_FUNC_INFO << "name read as" << modProperty.readValue;
                            break;
                        case 7: //Coordinates
                            {//combine the x y and z into a QVector3D.
                            QStringList locationSplit = modProperty.readValue.split(", ");
                            if(locationSplit.size() < 3){
                                qDebug() << Q_FUNC_INFO << "Invalid location:" << modProperty.readValue;
                                customLocation.position = QVector3D();
                                //customLocation.setAttribute("Position", "0, 0, 0");
                            } else {
                                QString tempx = locationSplit[0];
                                float x = tempx.toFloat();
                                QString tempy = locationSplit[1];
                                float y = tempy.toFloat();
                                QString tempz = locationSplit[2];
                                float z = tempz.toFloat();
                                customLocation.position = QVector3D(x, y, z);
                                //customLocation.setAttribute("Position", QString::number(x) + ", " + QString::number(y) + ", " + QString::number(z));
                            }
                            break;
                        }
                        case 8: //LocationID
                            customLocation.uniqueID = modProperty.readValue.toInt();
                            break;
                        case 9: //Linked Location
                            locationValue = modProperty.readValue.toInt();
                            if(locationValue != 0){
                                customLocation.linkedLocationIDs.push_back(locationValue); //since these text files will be obsolete, I'm not worried about adding intvector support.
                            }
                            break;
                        case 10: //Highjump difficulty
                            customLocation.highjumpDifficulty = modProperty.readValue.toInt();
                            break;
                        case 11: //Slipstream difficulty
                            customLocation.slipstreamDifficulty = modProperty.readValue.toInt();
                            readingLocation = false;
                            break;
                        default:
                            qDebug() << Q_FUNC_INFO << "Unknown property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                    }
                }
                qDebug() << Q_FUNC_INFO << "Adding location" << customLocation.locationName << "for level" << customLocation.levelName << "at coordinates" << customLocation.position;
                currentLocations.locationList.push_back(customLocation);
            }
            exodusData.customLocationList.push_back(currentLocations);
        }
        qDebug() << Q_FUNC_INFO << "file" << currentModFile.fileName();
    }
}



bool DataHandler::duplicateLocation(exPickupLocation testLocation){
    QVector3D loadedPosition;
    QVector3D testPosition = testLocation.position;
    int locationCount = 0;
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        for(int j = 0; j < exodusData.loadedLevels[i].spawnLocations.size(); j++){
            loadedPosition = exodusData.loadedLevels[i].spawnLocations[j].position;
            if((testPosition.x() < loadedPosition.x()+5 && testPosition.x() > loadedPosition.x() - 5)
                    && (testPosition.y() < loadedPosition.y()+5 && testPosition.y() > loadedPosition.y() - 5)
                    && (testPosition.z() < loadedPosition.z()+5 && testPosition.z() > loadedPosition.z() - 5)
                    && (testLocation.level == exodusData.loadedLevels[i].spawnLocations[j].level)){
                locationCount++;
            }
            if(locationCount > 1){
               return true;
            }
        }
    }
    return false;
}

bool DataHandler::miniconLoaded(int checkID){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(checkID == exodusData.miniconList[i].pickupID){
            return true;
        }
    }
    return false;
}

bool DataHandler::dataconLoaded(int checkID){
    int pickupCount = 0;
    for(int i = 0; i < exodusData.dataconList.size(); i++){
        if(checkID == exodusData.dataconList[i].dataID){
            pickupCount++;
        }
        if(pickupCount > 0){
            return true;
        }
    }
    return false;
}

void exPickupLocation::assignPickup(exPickup* pickupToAssign){
    pickup = pickupToAssign;
}

exPickup* exPickupLocation::assignedPickup(){
    return pickup;
}

/*void exPickupLocation::assignMinicon(int miniconID){
    qDebug() << Q_FUNC_INFO << "Assigning minicon" << miniconID << "to position" << uniqueID;
    pickupID = miniconID;
    //setAttribute("PickupToSpawn", QString::number(miniconID));
    //qDebug() << Q_FUNC_INFO << "checking placement" << searchAttributes<int>("PickupToSpawn") << "vs" << miniconID;
    //qDebug() << Q_FUNC_INFO << "done assigning minicon";
}

void exPickupLocation::assignDatacon(int dataID){
    qDebug() << Q_FUNC_INFO << "Assigning datacon" << dataID << "to position" << uniqueID;
    pickupID = 3;
    dataconID = dataID;
    //setAttribute("PickupToSpawn", QString::number(3));
    //setAttribute("ProductionArt", QString::number(dataID));
    //qDebug() << Q_FUNC_INFO << "done assigning datacon";
}

int exPickupLocation::assignedMinicon(){
    //int miniconID = 0;
    //miniconID = searchAttributes<int>("PickupToSpawn");
    //qDebug() << Q_FUNC_INFO << "Position" << uniqueID << "currently has pickup ID" << miniconID;
    //return miniconID;
    return pickupID;
}*/

exPickupLocation::exPickupLocation(taPickupPlaced fromItem){
    //QVector3D location = fromItem.searchAttributes<QVector3D>("Position");
    //setAttribute("Position", QString::number(location.x()) + ", " + QString::number(location.y()) + ", " + QString::number(location.z()));
    //position = fromItem.searchAttributes<QVector3D>("Position");
    position = fromItem.position;
    inputDatabaseInstance = fromItem.instanceIndex;
    //attributes = fromItem.attributes;
    //assignMinicon(0);
    spoiled = false;
    bunkerID = 0;
    //instanceIndex = 0;
    linkedLocationIDs = std::vector<int>();
    //this->spawnEvent = fromItem.searchAttributes<QString>("SpawnEvent");
    spawnEvent = fromItem.spawnEvent;
    //setAttributeDefault("GenerationDifficulty");
    //setAttributeDefault("ProductionArt");

    //this will be defined from the database, along with some of the other values here
    //this->locationName = fromItem.name;
}

exPickupLocation::exPickupLocation(){
    //setAttribute("Position", "0, 0, 0");
    position = QVector3D();
    inputDatabaseInstance = 0;
    assignPickup(nullptr);
    bunkerID = 0;
    //instanceIndex = 0;
    linkedLocationIDs = std::vector<int>();
    spoiled = false;
}
