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

// DataHandler::DataHandler(){
//     qDebug() << Q_FUNC_INFO << "Base DataHandler intialization called. This shouldn't happen";
// }

DataHandler::DataHandler(ProgWindow* passParent){
    parent = passParent;

    parent->openFile("TDB", QDir::currentPath() + "/DATA/EXODUS_MAIN.TDB");
    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "DATA-EXODUS_MAIN"){
            exodusData.dataFile = parent->databaseList[i];
        }
    }

    if(exodusData.dataFile == nullptr){
        qDebug() << Q_FUNC_INFO << "Main database was not found.";
    } else {
        qDebug() << Q_FUNC_INFO << "Main database file was found.";
    }

    return;
}

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

/*exPickup::exPickup(taPickupPlaced copyItem){
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
}*/

taMinicon::taMinicon(){
    this->name = "UNLOADED";
    this->minicon = MiniconEffects::None;
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
    this->minicon = static_cast<MiniconEffects>(copyItem.searchAttributes<int>("Minicon"));
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
    this->creatureID = copyItem.searchAttributes<int>("CreatureID");
    this->metagameID = copyItem.searchAttributes<int>("MetagameID");
    this->name = copyItem.searchAttributes<QString>("Name");
    this->rating = copyItem.searchAttributes<int>("Rating");
    this->hasVanillaPlacement = copyItem.searchAttributes<bool>("HasVanillaPlacement");
    this->isWeapon = copyItem.searchAttributes<bool>("IsWeapon");
    this->isExplosive = copyItem.searchAttributes<bool>("IsExplosive");
    this->placed = false;
}

taEpisode::taEpisode(dictItem copyItem){
    this->alternativeDirectoryName = copyItem.searchAttributes<QString>("AlternativeDirectoryName");
    this->bossType = static_cast<BossType>(copyItem.searchAttributes<int>("BossType"));
    this->dataconCount = copyItem.searchAttributes<int>("DataconCount");
    this->directoryName = copyItem.searchAttributes<QString>("DirectoryName");
    this->episodeOrder = copyItem.searchAttributes<int>("Episode");
    this->episodeID = static_cast<Episode>(episodeOrder);
    this->hasAlternativeDirectory = copyItem.searchAttributes<bool>("HasAlternativeDirectory");
    this->miniconCount = copyItem.searchAttributes<int>("MiniconCount");
    this->miniconUnlockCountExtreme = copyItem.searchAttributes<int>("MiniconUnlockCountExtreme");
    this->miniconUnlockCountHard = copyItem.searchAttributes<int>("MiniconUnlockCountHard");
    this->miniconUnlockCountNormal = copyItem.searchAttributes<int>("MiniconUnlockCountNormal");
    this->name = copyItem.searchAttributes<QString>("Name");
    this->WarpGateMusicTrack = {}; //copyItem.searchAttributes<bool>("MiniconCount");
    this->warpgates = copyItem.searchAttributes<int>("Warpgates");
}

void taEpisode::updateDirectories(){
    if(hasAlternativeDirectory){
        alternativeDirectoryName = "levels//episodes//0" + QString::number(static_cast<int>(episodeOrder)) + "_" + name + "Defeated/";
    }
    directoryName = "levels//episodes//0" + QString::number(static_cast<int>(episodeOrder)) + "_" + name + "/";
}

exPickupLocation::exPickupLocation(dictItem copyItem){
    originalEpisode = static_cast<Episode>(copyItem.searchAttributes<int>("Level"));
    usesAlternate = copyItem.searchAttributes<bool>("UsesAlternateDirectory");
    //levelName = copyItem.searchAttributes<QString>("LevelName");
    inputDatabaseInstance = copyItem.searchAttributes<int>("GameID");
    //uniqueID = copyItem.searchAttributes<int>("ExodusID");
    uniqueID = copyItem.instanceIndex;

    requiresSlipstream = copyItem.searchAttributes<bool>("RequiresSlipstream");
    requiresHighjump = copyItem.searchAttributes<bool>("RequiresHighjump");
    requiresExplosive = copyItem.searchAttributes<bool>("RequiresExplosive");
    //trick list excluded for now
    locationName = copyItem.searchAttributes<QString>("LocationName");
    spawnEvent = copyItem.searchAttributes<QString>("SpawnEvent");
    isBunker = copyItem.searchAttributes<bool>("IsBunker");
    position = copyItem.searchAttributes<QVector3D>("Position");
    maximumGenerationDifficulty = copyItem.searchAttributes<int>("MinimumGenerationDifficulty");
    minimumGenerationDifficulty = copyItem.searchAttributes<int>("MaximumGenerationDifficulty");

    //linkedLocationIDs = copyItem.searchAttributes<std::vector<int>>("LinkedLocations");
    linkedLocationIDs = {}; //this needs some work on the taData side of things - the template function and variant system is a mess and I hate it
    /*At the very least, manually define the linked locations for Mid-Atlantic. */
    availableTricks = {}; //same as above

    pickup = nullptr;
    spoiled = false;
}

exEpisode::exEpisode(dictItem copyItem){
    logName = copyItem.searchAttributes<QString>("EpisodeName");
    //outputFileName = copyItem.searchAttributes<QString>("EpisodeFolder");
    originalEpisode = copyItem.searchAttributes<int>("EpisodeID");
    currentEpisode = originalEpisode;
    episodeID = static_cast<Episode>(originalEpisode);
    requirements = copyItem.searchAttributes<int>("Requirements");
    placeable = copyItem.searchAttributes<bool>("Placeable");

    assignedMinicons = 0;
    assignedDatacons = 0;
}

exEpisode::exEpisode(){
    logName = "UNLOADED LEVEL";
    episodeID = Episode::Cybertron;
    assignedMinicons = 0;
    assignedDatacons = 0;
}

void exMinicon::setCreature(taPickup copyItem){
    if(!copyItem.isMinicon()){
        qDebug() << Q_FUNC_INFO << "Attempted to set a Minicon with a Datacon value. Not doing that.";
        return;
    }
    this->creatureID = copyItem.pickupToSpawn;
    //this->instanceIndex = copyItem.instanceIndex;
    //this->isWeapon = copyItem.isWeapon;
    placed = false;
}

exMinicon::exMinicon(){
    //instanceIndex = 0;
    isWeapon = false;
    name = "Shockpunch";
    creatureID = 27;
    metagameID = 0; //unknown for now, find shockpunch in the list
    //setAttribute("Position", "0, 0, 0");
}

exEpisode* DataHandler::getExodusEpisode(Episode episodeToGet){
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        if(exodusData.loadedLevels[i].episodeID == episodeToGet){
            return &exodusData.loadedLevels[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << static_cast<int>(episodeToGet) << "was not found in the Exodus data";
    return nullptr;
}

taEpisode* DataHandler::getGameEpisode(Episode episodeToGet){
    QStringList splitPath;
    for(int i = 0; i < gameData.levelList.size(); i++){
        if(gameData.levelList[i].episodeID == episodeToGet){
            return &gameData.levelList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << static_cast<int>(episodeToGet) << "was not found in the Game data";
    return nullptr;
}

taPickup* DataHandler::getPickup(int searchID){
    for(int i = 0; i < gameData.pickupList.size(); i++){
        if(gameData.pickupList[i].pickupToSpawn == searchID){
            return &gameData.pickupList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "Pickup" << searchID << "was not found.";
    return nullptr;
}

/*Currently only search by creatureID instead of metagameID*/
exMinicon* DataHandler::getExodusMinicon(int searchID){
    if(searchID == 3){
        qDebug() << Q_FUNC_INFO << "Searching for a datacon instead of minicon. This shouldn't happen.";
        return nullptr;
    }
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(exodusData.miniconList[i].creatureID == searchID){
            return &exodusData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

exMinicon* DataHandler::getExodusMinicon(QString searchName){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(exodusData.miniconList[i].name == searchName){
            return &exodusData.miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

taMinicon* DataHandler::getGameMinicon(int searchID){
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
    /*for(int i = 0; i < exodusData.dataconList.size(); i++){
        exodusData.dataconList[i].placed = false;
    }*/
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

taPickup::taPickup(dictItem fromItem){
    datacon_HealthPickupCount_Large = 0;
    datacon_HealthPickupCount_Small = 2;
    pickupToSpawn = static_cast<PickupToSpawn>(fromItem.searchAttributes<int>("PickupToSpawn"));
    productArt = static_cast<ProductArt>(fromItem.searchAttributes<int>("ProductionArt"));
    instanceIndex = fromItem.instanceIndex;
}

exTrick::exTrick(dictItem fromItem){
    this->name = fromItem.searchAttributes<QString>("Name");
    this->description = fromItem.searchAttributes<QString>("Description");
    this->difficulty = fromItem.searchAttributes<int>("Difficulty");
    this->guideLink = fromItem.searchAttributes<QString>("Guide");
    /*Minicon requirements have to be hard-coded for now*/
    QStringList hardcodedTricks = {"Damage Boost", "Ramp Boost", "Death Boost","Airdash","Starship Skip", "Infinite Glide Up (IGU)"};
    switch (hardcodedTricks.indexOf(name)){
    case 0: //Damage Boost - hailstorm
        this->requiredMinicons = {11, 12};
        this->needAllRequirements = false;
        break;
    case 1: //Ramp boost
        this->requiredMinicons = {31, 37};
        this->needAllRequirements = false;
        break;
    case 2: //Death boost
        this->requiredMinicons = {12, 41, 46};
        this->needAllRequirements = true;
        break;
    case 3: //Airdash
        this->requiredMinicons = {31, 37};
        this->needAllRequirements = false;
        break;
    case 4: //Starship skip
        this->requiredMinicons = {25};
        this->needAllRequirements = true;
        break;
    case 5: //IGU
        this->requiredMinicons = {25};
        this->needAllRequirements = true;
        break;
    default:
        this->requiredMinicons = {};
        this->needAllRequirements = false;
        break;
    }
}

void DataHandler::loadTricks(){
    exodusData.trickList = convertInstances<exTrick>(exodusData.dataFile->sendInstances("exTrick"));
    std::sort(exodusData.trickList.begin(), exodusData.trickList.end());
}

void DataHandler::loadLevels(){

    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            gameData.metagameFile = parent->databaseList[i];
        }
    }

    exodusData.loadedLevels = convertInstances<exEpisode>(exodusData.dataFile->sendInstances("exEpisode"));
    gameData.levelList = convertInstances<taEpisode>(gameData.metagameFile->sendInstances("Episode"));

    qDebug() << Q_FUNC_INFO << "Loaded levels:" << exodusData.loadedLevels.size();

    for(int i = 0; i< exodusData.loadedLevels.size(); i++){
        taEpisode* currentEpisode = getGameEpisode(exodusData.loadedLevels[i].episodeID);
        QString nameCheck = "0" + QString::number(exodusData.loadedLevels[i].originalEpisode+1) + "_" + currentEpisode->name.toUpper() + "-CREATURE";
        for(int j = 0; j < parent->databaseList.size(); j++){
            qDebug() << Q_FUNC_INFO << "Checking for file name:" << nameCheck << "vs" << parent->databaseList[j]->fileName;
            if(parent->databaseList[j]->fileName == nameCheck){
                qDebug() << Q_FUNC_INFO << "Setting level file.";
                exodusData.loadedLevels[i].levelFile = parent->databaseList[j];
            }
        }
    }


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
            if(exodusData.loadedLevels[i].episodeID != loadedLocations[j].originalEpisode){
                continue;
            }
            loadedLocations[j].episode = &exodusData.loadedLevels[i];
            exodusData.loadedLevels[i].spawnLocations.push_back(loadedLocations[j]);
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded locations:" << loadedLocations.size();

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        qDebug() << Q_FUNC_INFO << "Level file name:" << exodusData.loadedLevels[i].levelFile->fileName;
        for(int j = 0; j < exodusData.loadedLevels[i].spawnLocations.size(); j++){
            QVector3D debugPosition = exodusData.loadedLevels[i].spawnLocations[j].position;
            qDebug() << Q_FUNC_INFO << i << " " << exodusData.loadedLevels[i].spawnLocations[j].uniqueID << "  "
                     << exodusData.loadedLevels[i].spawnLocations[j].linkedLocationIDs << "    " << exodusData.loadedLevels[i].spawnLocations[j].episode->originalEpisode << "    "
                     << exodusData.loadedLevels[i].spawnLocations[j].locationName << "    " << debugPosition.x() << "   " << debugPosition.y() << "  " << debugPosition.z();
            qDebug() << Q_FUNC_INFO << "is pickup nullptr? (it should be)" << (exodusData.loadedLevels[i].spawnLocations[j].pickup == nullptr);
        }
    }

}

dictItem DataHandler::createGamePickupPlaced(const exPickupLocation* location){
    //exEpisode targetLevel = exodusData.loadedLevels[static_cast<int>(location.episode)];
    dictItem convertedData;
    convertedData.name = "PickupPlaced";
    convertedData.attributes = location->episode->levelFile->generateAttributes("PickupPlaced");
    //qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    convertedData.setAttribute("Position", QString::number(location->position.x()) + ", " + QString::number(location->position.y()) + ", " + QString::number(location->position.z()));

    convertedData.setAttribute("PickupToSpawn", QString::number(location->pickup->pickupToSpawn));

    if(!location->pickup->isMinicon()){
        //qDebug() << Q_FUNC_INFO << "Datacon - setting production art value to" << location->pickup->productArt << "from" << location->pickup->pickupToSpawn;
        convertedData.setAttribute("ProductionArt", QString::number(location->pickup->productArt));
    }

    if(location->spawnEvent != ""){
        convertedData.setAttribute("SpawnEvent", location->spawnEvent);
    }

    /*for(int i = 0; i < convertedData.attributes.size(); i++){
        qDebug() << Q_FUNC_INFO << "attribute" << i << "name" << convertedData.attributes[i]->name << "value" << convertedData.attributes[i]->display();
    }*/

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
        qDebug() << Q_FUNC_INFO << "Calling for minicon type" << miniconTypes[type];
        metagameMinicons = convertInstances<taMinicon>(gameData.metagameFile->sendInstances(miniconTypes[type]));
        for(int i = 0; i < metagameMinicons.size(); i++){
            if(metagameMinicons[i].minicon != MiniconEffects::None){
                gameData.miniconList.push_back(metagameMinicons[i]);
            }
        }
        //gameData.miniconList.insert(gameData.miniconList.end(), metagameMinicons.begin(), metagameMinicons.end());
        qDebug() << Q_FUNC_INFO << "Minicon list now has:" << gameData.miniconList.size() << "loaded minicons";
    }

    qDebug() << Q_FUNC_INFO << "gameData loaded minicons:" << gameData.miniconList.size();
    for(int i = 0; i < gameData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "loaded minicon" << i << "is named" << gameData.miniconList[i].name << static_cast<int>(gameData.miniconList[i].minicon);
    }

    exodusData.miniconList = convertInstances<exMinicon>(exodusData.dataFile->sendInstances("exMinicon"));

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        std::vector<taPickup> filePickupsBase = convertInstances<taPickup>(exodusData.loadedLevels[i].levelFile->sendInstances("PickupPlaced"));
        for(int j = 0; j < filePickupsBase.size(); j++){
            if(!duplicatePickup(filePickupsBase[j])){
                gameData.pickupList.push_back(filePickupsBase[j]);
            }
        }
    }

    qDebug() << Q_FUNC_INFO << "Total gameData pickuplist items:" << gameData.pickupList.size();
    for(int i = 0; i < gameData.pickupList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << gameData.pickupList[i].isMinicon() << gameData.pickupList[i].pickupToSpawn << gameData.pickupList[i].productArt;
    }

    qDebug() << Q_FUNC_INFO << "exodusData loaded minicons:" << exodusData.miniconList.size();
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << exodusData.miniconList[i].creatureID << "  " << exodusData.miniconList[i].name << "    "
                 << " is weapon:" << exodusData.miniconList[i].isWeapon;
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
    foreach(taPickup currentPickup, gameData.pickupList){
        bool dataconIsLoaded = false;
        if(currentPickup.isMinicon()){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "pickup properties for:" << currentPickup.pickupToSpawn << "dataID" << currentPickup.productArt;
        dataconIsLoaded = dataconLoaded(currentPickup.productArt);
        qDebug() << Q_FUNC_INFO << "already loaded?" << dataconIsLoaded;
        if (!dataconIsLoaded && currentPickup.pickupToSpawn == 3){
            //we now know it's a datacon (but still check to be sure). if it hasn't been loaded, add it to the datacon list
            gameData.dataconList.push_back(currentPickup);
        } else if (dataconIsLoaded && currentPickup.pickupToSpawn == 3){
            //if it has been loaded, just skip it
            continue;
        } else {
            //this should never happen, logically, but better safe than confused.
            //put some debugs here, just in case
            continue;
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded datacons:" << gameData.dataconList.size();
    for(int i = 0; i < gameData.dataconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << gameData.dataconList[i].pickupToSpawn << "  " << gameData.dataconList[i].productArt;
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
    QStringList propertyOptions = {"File Version", "Name", "Author", "Description", "Location Count", "Level", "Location Name", "Coordinates", "Location ID", "Linked Locations", "Requires Highjump", "Requires Slipstream"};
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
                customLocation.originalEpisode = Episode::Cybertron;
                //double-check that the below doesn't need to find the specific database for the target level
                //customLocation.attributes = levelList[0].levelFile->generateAttributes("PickupPlaced");
                bool readingLocation = true;
                while(readingLocation){
                    modProperty = modBuffer.readProperty();
                    switch(propertyOptions.indexOf(modProperty.name)){
                        case 5: //Level
                            for(int i = 0; i < gameData.levelList.size(); i++){
                                customLocation.levelName = modProperty.readValue;
                                qDebug() << Q_FUNC_INFO << "comparing level" << gameData.levelList[i].name << "to" << customLocation.levelName;
                                if(gameData.levelList[i].name == customLocation.levelName){
                                    qDebug() << Q_FUNC_INFO << "comparing level: match";
                                    customLocation.originalEpisode = static_cast<Episode>(i);
                                }
                            }
                            if(customLocation.originalEpisode == Episode::Cybertron){
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
                            if(modProperty.readValue.toUpper() == "FALSE"){
                                customLocation.requiresHighjump = false;
                            } else {
                                customLocation.requiresHighjump = true;
                            }
                            break;
                        case 11: //Slipstream difficulty
                            if(modProperty.readValue.toUpper() == "FALSE"){
                                customLocation.requiresSlipstream = false;
                            } else {
                                customLocation.requiresSlipstream = true;
                            }
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

bool DataHandler::duplicatePickup(taPickup testPickup){
    for(int i = 0; i < gameData.pickupList.size(); i++){
        if(testPickup.pickupToSpawn == gameData.pickupList[i].pickupToSpawn
            && testPickup.productArt == gameData.pickupList[i].productArt){
            return true;
        }
    }
    return false;
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
                    && (testLocation.episode == exodusData.loadedLevels[i].spawnLocations[j].episode)){
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
        if(checkID == exodusData.miniconList[i].creatureID){
            return true;
        }
    }
    return false;
}

bool DataHandler::dataconLoaded(int checkID){
    int pickupCount = 0;
    for(int i = 0; i < gameData.dataconList.size(); i++){
        if(checkID == gameData.dataconList[i].productArt){
            pickupCount++;
        }
        if(pickupCount > 0){
            return true;
        }
    }
    return false;
}

void exPickupLocation::assignPickup(taPickup* pickupToAssign){
    pickup = pickupToAssign;
}

taPickup* exPickupLocation::assignedPickup(){
    return pickup;
}

int DataHandler::highestAvailableLevel(int checkRequirements){
    /*Useful when the level order has been randomized*/
    int availableWorld = 0;
    int checkWorld = 0;
    qDebug() << Q_FUNC_INFO << "Checking for highest level available without requirement" << checkRequirements;
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        qDebug() << Q_FUNC_INFO << "original episode" << exodusData.loadedLevels[i].originalEpisode << "has current episode" << exodusData.loadedLevels[i].currentEpisode;
        /*set up a case statement to handle alternate requirements based on available tricks*/
        checkWorld = exodusData.loadedLevels[i].currentEpisode;
        if(availableWorld > checkWorld){
            /*With the level list sorting, this should never happen*/
            continue;
        }
        availableWorld = checkWorld;
        if(exodusData.loadedLevels[i].requirements & checkRequirements){
            qDebug() << Q_FUNC_INFO << "Found a level with this requirement. Returning" << availableWorld;
            return availableWorld;
        }
    }
    return availableWorld;
}

exPickupLocation::exPickupLocation(taLocation fromItem){
    //QVector3D location = fromItem.searchAttributes<QVector3D>("Position");
    //setAttribute("Position", QString::number(location.x()) + ", " + QString::number(location.y()) + ", " + QString::number(location.z()));
    //position = fromItem.searchAttributes<QVector3D>("Position");
    position = fromItem.position;
    inputDatabaseInstance = fromItem.instanceIndex;
    //attributes = fromItem.attributes;
    //assignMinicon(0);
    spoiled = false;
    inputDatabaseInstance = 0;
    //instanceIndex = 0;
    linkedLocationIDs = std::vector<int>();
    //this->spawnEvent = fromItem.searchAttributes<QString>("SpawnEvent");
    spawnEvent = fromItem.spawnEvent;
    pickup = nullptr;
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
    inputDatabaseInstance = 0;
    //instanceIndex = 0;
    linkedLocationIDs = std::vector<int>();
    spoiled = false;
}
