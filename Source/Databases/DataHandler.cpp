#include <QDir>
#include <QDirIterator>
#include <QCoreApplication>

#include "Databases/DataHandler.h"
#include "Databases/taMinicon.h"
#include "UI/exWindow.h"
#include "Utility/exDebugger.h"
#include "FileManagement/Zebrafish.h"

DataHandler::DataHandler(exWindowBase *passUI, zlManager *fileManager){
    m_UI = passUI;
    m_Debug = &exDebugger::GetInstance();
    m_zlManager = fileManager;

    m_zlManager->OpenFile("TDB", QDir::currentPath() + "/DATA/EXODUS_MAIN.TDB");
    for(int i = 0; i < m_zlManager->m_databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << m_zlManager->m_databaseList[i]->fileName;
        if(m_zlManager->m_databaseList[i]->fileName == "DATA-EXODUS_MAIN"){
            exodusData.dataFile = m_zlManager->m_databaseList[i];
        }
    }

    if(exodusData.dataFile == nullptr){
        qDebug() << Q_FUNC_INFO << "Main database was not found.";
    } else {
        qDebug() << Q_FUNC_INFO << "Main database file was found.";
    }

    return;
}

void DataHandler::LoadAll(){
    loadLevels();
    loadMinicons();
    loadDatacons();
    loadAutobots();
    loadCustomLocations();
    loadTricks();

    qDebug() << Q_FUNC_INFO << "Exodus data:";
    qDebug() << Q_FUNC_INFO << "Loaded levels:" << exodusData.loadedLevels.size();
    qDebug() << Q_FUNC_INFO << "Minicon list:" << exodusData.miniconList.size();
    qDebug() << Q_FUNC_INFO << "Custom locations:" << exodusData.customLocationList.size();
    qDebug() << Q_FUNC_INFO << "Tricks:" << exodusData.trickList.size();

    qDebug() << Q_FUNC_INFO << "Game data:";
    qDebug() << Q_FUNC_INFO << "Level list:" << gameData.levelList.size();
    qDebug() << Q_FUNC_INFO << "Autobot list:" << gameData.autobotList.size();
    qDebug() << Q_FUNC_INFO << "Minicon list:" << gameData.miniconList.size();
    qDebug() << Q_FUNC_INFO << "Pickup list:" << gameData.pickupList.size();
    qDebug() << Q_FUNC_INFO << "Datacon list:" << gameData.dataconList.size();
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
    this->bossType = copyItem.searchAttributes<int>("BossType");
    this->dataconCount = copyItem.searchAttributes<int>("DataconCount");
    this->directoryName = copyItem.searchAttributes<QString>("DirectoryName");
    this->episodeOrder = copyItem.searchAttributes<int>("Episode");
    this->episode = episodeOrder;
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
        alternativeDirectoryName = "levels//episodes//0" + QString::number(episodeOrder) + "_" + name + "Defeated/";
    }
    directoryName = "levels//episodes//0" + QString::number(episodeOrder) + "_" + name + "/";
}

exPickupLocation::exPickupLocation(dictItem copyItem){
    int levelID = copyItem.searchAttributes<int>("Level");
    world = levelID;
    usesAlternate = copyItem.searchAttributes<bool>("UsesAlternateDirectory");
    //levelName = copyItem.searchAttributes<QString>("LevelName");
    inputDatabaseInstance = copyItem.searchAttributes<int>("GameID");
    //uniqueID = copyItem.searchAttributes<int>("ExodusID");
    uniqueID = (usesAlternate * 10000) + (levelID * 1000) + copyItem.instanceIndex;

    requiresSlipstream = copyItem.searchAttributes<bool>("RequiresSlipstream");
    requiresHighjump = copyItem.searchAttributes<bool>("RequiresHighjump");
    requiresExplosive = copyItem.searchAttributes<bool>("RequiresExplosive");
    //trick list excluded for now
    locationName = copyItem.searchAttributes<QString>("LocationName");
    spawnEvent = copyItem.searchAttributes<QString>("SpawnEvent");
    isBunker = copyItem.searchAttributes<bool>("IsBunker");
    originalTeleportNode = copyItem.searchAttributes<int>("OriginalTeleportNode");
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
    if(logName == "Post-Boss Mid-Atlantic"){
        /*Being lazy and hard-coding this instead of adding it to the DB. TODO: do this properly*/
        usesAlternate = true;
    }
    //outputFileName = copyItem.searchAttributes<QString>("EpisodeFolder");
    world = copyItem.searchAttributes<int>("EpisodeID");
    currentEpisode = world;
    requirements = copyItem.searchAttributes<int>("Requirements");
    placeable = copyItem.searchAttributes<bool>("Placeable");

    assignedMinicons = 0;
    assignedDatacons = 0;
}

exEpisode::exEpisode(){
    logName = "UNLOADED LEVEL";
    world = EpisodeInvalid;
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

exEpisode* DataHandler::getExodusEpisode(int episodeToGet){
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        if(exodusData.loadedLevels[i]->world == episodeToGet){
            return exodusData.loadedLevels[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << episodeToGet << "was not found in the Exodus data";
    return nullptr;
}

taEpisode* DataHandler::getGameEpisode(int episodeToGet){
    QStringList splitPath;
    for(int i = 0; i < gameData.levelList.size(); i++){
        if(gameData.levelList[i]->episode == episodeToGet){
            return gameData.levelList[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "Episode" << episodeToGet << "was not found in the Game data";
    return nullptr;
}

taPickup* DataHandler::getPickup(int searchID){
    for(int i = 0; i < gameData.pickupList.size(); i++){
        if(gameData.pickupList[i]->pickupToSpawn == searchID){
            return gameData.pickupList[i].get();
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
        if(exodusData.miniconList[i]->creatureID == searchID){
            return exodusData.miniconList[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

exMinicon* DataHandler::getExodusMinicon(QString searchName){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        if(exodusData.miniconList[i]->name == searchName){
            return exodusData.miniconList[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

taMinicon* DataHandler::getGameMinicon(int searchID){
    for(int i = 0; i < gameData.miniconList.size(); i++){
        if(gameData.miniconList[i]->minicon == searchID){
            return gameData.miniconList[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

taMinicon* DataHandler::getGameMinicon(QString searchName){
    for(int i = 0; i < gameData.miniconList.size(); i++){
        if(gameData.miniconList[i]->name == searchName){
            return gameData.miniconList[i].get();
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

void DataHandler::resetMinicons(){
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        exodusData.miniconList[i]->placed = false;
    }
}

void DataHandler::resetDatacons(){
    /*for(int i = 0; i < exodusData.dataconList.size(); i++){
        exodusData.dataconList[i].placed = false;
    }*/
}

void DataHandler::resetLevels(){
    for(int i = 0; i < gameData.levelList.size(); i++){
        gameData.levelList[i]->dataconCount = 0;
        gameData.levelList[i]->miniconCount = 0;
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
    convertInstances<exTrick>(exodusData.dataFile->sendInstances("exTrick"), &exodusData.trickList);
    std::sort(exodusData.trickList.begin(), exodusData.trickList.end());
    for(int i = 0; i < exodusData.trickList.size(); i++){
        exodusData.trickList[i]->enabled = false;
    }
}

void DataHandler::addCustomLocations(){
    std::vector<exPickupLocation> loadedLocations;
    qDebug() << Q_FUNC_INFO << "Adding custom locations to list." << exodusData.customLocationList.size() << "custom locations to add.";
    for(int i = 0; i < exodusData.customLocationList.size(); i++){
        qDebug() << Q_FUNC_INFO << "enabled?" << exodusData.customLocationList[i]->enabled;
        if(exodusData.customLocationList[i]->enabled){
            qDebug() << Q_FUNC_INFO << "Locationlist" << i << "has" << exodusData.customLocationList[i]->locationList.size() << "locations";
            for(int j = 0; j < exodusData.customLocationList[i]->locationList.size(); j++){
                loadedLocations.push_back(exodusData.customLocationList[i]->locationList[j]);
            }
        }
    }
    for(int i = 0; i < loadedLocations.size(); i++){
        qDebug() << Q_FUNC_INFO << "custom location has original episode" << loadedLocations[i].world << "uses alternate?" << loadedLocations[i].usesAlternate;
    }

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        for(int j = 0; j < loadedLocations.size(); j++){
            if(exodusData.loadedLevels[i]->world != loadedLocations[j].world){
                continue;
            }
            if(exodusData.loadedLevels[i]->usesAlternate != loadedLocations[j].usesAlternate){
                continue;
            }
            exodusData.loadedLevels[i]->spawnLocations.push_back(loadedLocations[j]);
        }
    }
}

void DataHandler::loadMinicons(){

    /*Move the metagame and exodusmain file searches to datahandler initialization*/
    for(int i = 0; i < m_zlManager->m_databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << m_zlManager->m_databaseList[i]->fileName;
        if(m_zlManager->m_databaseList[i]->fileName == "TFA-METAGAME"){
            gameData.metagameFile = m_zlManager->m_databaseList[i];
        }
    }

    if(gameData.metagameFile == nullptr){
        m_Debug->Log("Unable to find file METAGAME.TDB. Minicon metadata was not loaded.");
    }

    for(int i = 0; i < m_miniconTypes.size(); i++){
        LoadMiniconType(m_miniconTypes[i]);
    }

    qDebug() << Q_FUNC_INFO << "gameData loaded minicons:" << gameData.miniconList.size();
    for(int i = 0; i < gameData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "loaded minicon" << i << "is named" << gameData.miniconList[i]->name << gameData.miniconList[i]->minicon;
    }

    convertInstances<exMinicon>(exodusData.dataFile->sendInstances("exMinicon"), &exodusData.miniconList);

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        std::vector<std::shared_ptr<taPickup>> filePickupsBase;
        convertInstances<taPickup>(exodusData.loadedLevels[i]->levelFile->sendInstances("PickupPlaced"), &filePickupsBase);
        for(int j = 0; j < filePickupsBase.size(); j++){
            if(!duplicatePickup(*filePickupsBase[j].get())){
                gameData.pickupList.emplace_back(filePickupsBase[j]);
            }
        }
    }

    qDebug() << Q_FUNC_INFO << "Total gameData pickuplist items:" << gameData.pickupList.size();
    for(int i = 0; i < gameData.pickupList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << gameData.pickupList[i]->isMinicon() << gameData.pickupList[i]->pickupToSpawn << gameData.pickupList[i]->productArt;
    }

    qDebug() << Q_FUNC_INFO << "exodusData loaded minicons:" << exodusData.miniconList.size();
    for(int i = 0; i < exodusData.miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << exodusData.miniconList[i]->creatureID << "  " << exodusData.miniconList[i]->name << "    "
                 << " is weapon:" << exodusData.miniconList[i]->isWeapon;
    }

}

void DataHandler::loadLevels(){

    for(int i = 0; i < m_zlManager->m_databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << m_zlManager->m_databaseList[i]->fileName;
        if(m_zlManager->m_databaseList[i]->fileName == "TFA-METAGAME"){
            gameData.metagameFile = m_zlManager->m_databaseList[i];
        }
    }

    convertInstances<exEpisode>(exodusData.dataFile->sendInstances("exEpisode"), &exodusData.loadedLevels);
    convertInstances<taEpisode>(gameData.metagameFile->sendInstances("Episode"), &gameData.levelList);

    qDebug() << Q_FUNC_INFO << "Loaded levels:" << exodusData.loadedLevels.size();

    for(int i = 0; i< exodusData.loadedLevels.size(); i++){
        taEpisode* currentEpisode = getGameEpisode(exodusData.loadedLevels[i]->world);
        QString alternateCheck = "";
        if(exodusData.loadedLevels[i]->usesAlternate && exodusData.loadedLevels[i]->logName != "Starship"){
            alternateCheck = "DEFEATED";
        }
        QString nameCheck = "0" + QString::number(exodusData.loadedLevels[i]->world+1) + "_" + currentEpisode->name.toUpper() + alternateCheck + "-CREATURE";
        for(int j = 0; j < m_zlManager->m_databaseList.size(); j++){
            qDebug() << Q_FUNC_INFO << "Checking for file name:" << nameCheck << "vs" << m_zlManager->m_databaseList[j]->fileName;
            if(m_zlManager->m_databaseList[j]->fileName == nameCheck){
                qDebug() << Q_FUNC_INFO << "Setting level file.";
                exodusData.loadedLevels[i]->levelFile = m_zlManager->m_databaseList[j];
            }
        }
    }


    std::vector<std::shared_ptr<exPickupLocation>> loadedLocations;
    convertInstances<exPickupLocation>(exodusData.dataFile->sendInstances("exPickupLocation"), &loadedLocations);

    /*Manually setting linked locations, since they don't work until the Database system is fixed*/
    for(int i = 0; i < loadedLocations.size(); i++){
        switch(loadedLocations[i]->uniqueID){
            case 3061:
            loadedLocations[i]->linkedLocationIDs.push_back(13065);
            break;
            case 3062:
            loadedLocations[i]->linkedLocationIDs.push_back(13066);
            break;
            case 3063:
            loadedLocations[i]->linkedLocationIDs.push_back(13068);
            break;
            case 3064:
            loadedLocations[i]->linkedLocationIDs.push_back(13067);
            break;
            case 13065:
            loadedLocations[i]->linkedLocationIDs.push_back(3061);
            break;
            case 13066:
            loadedLocations[i]->linkedLocationIDs.push_back(3062);
            break;
            case 13067:
            loadedLocations[i]->linkedLocationIDs.push_back(3064);
            break;
            case 13068:
            loadedLocations[i]->linkedLocationIDs.push_back(3063);
            break;
            case 5094:
            loadedLocations[i]->linkedLocationIDs.push_back(5095);
            break;
            case 5095:
            loadedLocations[i]->linkedLocationIDs.push_back(5094);
            break;
        default:
            break;
        }
    }

    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        for(int j = 0; j < loadedLocations.size(); j++){
            if(exodusData.loadedLevels[i]->world != loadedLocations[j]->world){
                continue;
            }
            if(exodusData.loadedLevels[i]->usesAlternate != loadedLocations[j]->usesAlternate){
                continue;
            }
            exodusData.loadedLevels[i]->spawnLocations.push_back(*loadedLocations[j].get());
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded locations:" << loadedLocations.size();
}

void DataHandler::debugLocations(){
    exPickupLocation* currentLocation = nullptr;
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        qDebug() << Q_FUNC_INFO << "Level file name:" << exodusData.loadedLevels[i]->levelFile->fileName;
        for(int j = 0; j < exodusData.loadedLevels[i]->spawnLocations.size(); j++){
            currentLocation = &exodusData.loadedLevels[i]->spawnLocations[j];
            QVector3D debugPosition = currentLocation->position;
            qDebug() << Q_FUNC_INFO << i << j << " " << currentLocation->uniqueID << "  "
                     << currentLocation->linkedLocationIDs << "    " << currentLocation->world << "    "
                     << currentLocation->locationName << "    " << debugPosition.x() << "   " << debugPosition.y() << "  " << debugPosition.z()
                     << "  " << currentLocation->originalTeleportNode;
            qDebug() << Q_FUNC_INFO << "is pickup nullptr? (it should be)" << (exodusData.loadedLevels[i]->spawnLocations[j].pickup == nullptr);
        }
    }
}

dictItem DataHandler::createGamePickupPlaced(const exPickupLocation* location){
    //exEpisode targetLevel = exodusData.loadedLevels[static_cast<int>(location.episode)];
    dictItem convertedData;
    convertedData.name = "PickupPlaced";
    convertedData.attributes = getExodusEpisode(location->world)->levelFile->generateAttributes("PickupPlaced");
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

dictItem DataHandler::createGameEpisode(const taEpisode* episode){
    //exEpisode targetLevel = exodusData.loadedLevels[static_cast<int>(location.episode)];
    dictItem convertedData;
    convertedData.name = "Episode";
    convertedData.attributes = gameData.metagameFile->generateAttributes("Episode");
    //qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    convertedData.setAttribute("BossType", QString::number(episode->bossType));
    convertedData.setAttribute("DataconCount", QString::number(episode->dataconCount));
    convertedData.setAttribute("DirectoryName", episode->directoryName);
    convertedData.setAttribute("Episode", QString::number(getExodusEpisode(episode->episode)->currentEpisode));

    if(episode->hasAlternativeDirectory){
        convertedData.setAttribute("HasAlternativeDirectory", "True");
    }
    convertedData.setAttribute("AlternativeDirectoryName", episode->alternativeDirectoryName);

    convertedData.setAttribute("MiniconCount", QString::number(episode->miniconCount));
    convertedData.setAttribute("Name", episode->name);
    convertedData.setAttribute("Warpgates", QString::number(episode->warpgates));

    return convertedData;
}

void DataHandler::loadAutobots(){
    std::shared_ptr<DatabaseFile> metagameFile;
    for(int i = 0; i < m_zlManager->m_databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << m_zlManager->m_databaseList[i]->fileName;
        if(m_zlManager->m_databaseList[i]->fileName == "TFA-METAGAME"){
            metagameFile = m_zlManager->m_databaseList[i];
        }
    }

    gameData.autobotList = metagameFile->sendInstances("Autobot");
}

void DataHandler::loadDatacons(){
    for(int i = 0; i < gameData.pickupList.size(); i++){
        bool dataconIsLoaded = false;
        if(gameData.pickupList[i]->isMinicon()){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "pickup properties for:" << gameData.pickupList[i]->pickupToSpawn << "dataID" << gameData.pickupList[i]->productArt;
        dataconIsLoaded = dataconLoaded(gameData.pickupList[i]->productArt);
        qDebug() << Q_FUNC_INFO << "already loaded?" << dataconIsLoaded;
        if (!dataconIsLoaded && gameData.pickupList[i]->pickupToSpawn == 3){
            //we now know it's a datacon (but still check to be sure). if it hasn't been loaded, add it to the datacon list
            gameData.dataconList.push_back(gameData.pickupList[i]);
        } else if (dataconIsLoaded && gameData.pickupList[i]->pickupToSpawn == 3){
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
        qDebug() << Q_FUNC_INFO << i << " " << gameData.dataconList[i]->pickupToSpawn << "  " << gameData.dataconList[i]->productArt;
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
    QStringList propertyOptions = {"File Version", "Name", "Author", "Description", "Location Count", "Level", "Location Name", "Coordinates", "Location ID", "Linked Locations", "Requires Highjump", "Requires Slipstream", "Alternate Directory"};
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
            currentLocations.enabled = false;
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
                customLocation.world = EpisodeInvalid;
                //double-check that the below doesn't need to find the specific database for the target level
                //customLocation.attributes = levelList[0].levelFile->generateAttributes("PickupPlaced");
                bool readingLocation = true;
                while(readingLocation){
                    qDebug() << Q_FUNC_INFO << "read property type:" << modProperty.name << "with value:" << modProperty.readValue << "switch case:" << propertyOptions.indexOf(modProperty.name);
                    modProperty = modBuffer.readProperty();
                    switch(propertyOptions.indexOf(modProperty.name)){
                        case 5: //Level
                            for(int i = 0; i < gameData.levelList.size(); i++){
                                customLocation.levelName = modProperty.readValue;
                                qDebug() << Q_FUNC_INFO << "comparing level" << gameData.levelList[i]->name << "to" << customLocation.levelName;
                                if(gameData.levelList[i]->name == customLocation.levelName){
                                    qDebug() << Q_FUNC_INFO << "comparing level: match";
                                    customLocation.world = i;
                                }
                            }
                            if(customLocation.world == EpisodeInvalid){
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
                            qDebug() << Q_FUNC_INFO << "Location ID:" << modProperty.readValue;
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
                        case 12:
                            if(modProperty.readValue.toUpper() == "TRUE"){
                                customLocation.usesAlternate = true;
                            } else {
                                customLocation.usesAlternate = false;
                            }
                            break;
                        default:
                            qDebug() << Q_FUNC_INFO << "Unknown property" << modProperty.name << "with value" << modProperty.readValue << "found at" << modBuffer.currentPosition;
                    }
                }
                qDebug() << Q_FUNC_INFO << "Adding location" << customLocation.locationName << "for level" << customLocation.levelName << "at coordinates" << customLocation.position;
                currentLocations.locationList.push_back(customLocation);
            }
            qDebug() << Q_FUNC_INFO << "Adding currentlocations" << currentLocations.locationList.size();
            exodusData.customLocationList.push_back(std::make_shared<exCustomLocation>(currentLocations));
        }
        qDebug() << Q_FUNC_INFO << "file" << currentModFile.fileName();
    }

}

void DataHandler::sortEpisodes(){
    std::sort(exodusData.loadedLevels.begin(), exodusData.loadedLevels.end(), EpisodeCompare());
}

bool DataHandler::duplicatePickup(taPickup testPickup){
    for(int i = 0; i < gameData.pickupList.size(); i++){
        if(testPickup.pickupToSpawn == gameData.pickupList[i]->pickupToSpawn
            && testPickup.productArt == gameData.pickupList[i]->productArt){
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
        for(int j = 0; j < exodusData.loadedLevels[i]->spawnLocations.size(); j++){
            loadedPosition = exodusData.loadedLevels[i]->spawnLocations[j].position;
            if((testPosition.x() < loadedPosition.x()+5 && testPosition.x() > loadedPosition.x() - 5)
                    && (testPosition.y() < loadedPosition.y()+5 && testPosition.y() > loadedPosition.y() - 5)
                    && (testPosition.z() < loadedPosition.z()+5 && testPosition.z() > loadedPosition.z() - 5)
                    && (testLocation.world == exodusData.loadedLevels[i]->spawnLocations[j].world)){
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
        if(checkID == exodusData.miniconList[i]->creatureID){
            return true;
        }
    }
    return false;
}

bool DataHandler::dataconLoaded(int checkID){
    int pickupCount = 0;
    for(int i = 0; i < gameData.dataconList.size(); i++){
        if(checkID == gameData.dataconList[i]->productArt){
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
        qDebug() << Q_FUNC_INFO << "original episode" << exodusData.loadedLevels[i]->world << "has current episode" << exodusData.loadedLevels[i]->currentEpisode;
        /*set up a case statement to handle alternate requirements based on available tricks*/
        checkWorld = exodusData.loadedLevels[i]->currentEpisode;
        if(availableWorld > checkWorld){
            /*With the level list sorting, this should never happen*/
            continue;
        }
        availableWorld = checkWorld;
        if(exodusData.loadedLevels[i]->requirements & checkRequirements){
            qDebug() << Q_FUNC_INFO << "Found a level with this requirement. Returning" << availableWorld;
            return availableWorld;
        }
    }
    return availableWorld;
}

void DataHandler::updateMetagameEpisodes(){
    for(int i = 0; i < exodusData.loadedLevels.size(); i++){
        int miniconCount = exodusData.loadedLevels[i]->assignedMinicons;
        int dataconCount = exodusData.loadedLevels[i]->assignedDatacons;
        getGameEpisode(exodusData.loadedLevels[i]->world)->miniconCount = miniconCount;
        getGameEpisode(exodusData.loadedLevels[i]->world)->dataconCount = dataconCount;
    }

    gameData.metagameFile->removeAll("Episode");
    for(int i = 0; i < gameData.levelList.size(); i++){
        dictItem itemToAdd = createGameEpisode(gameData.levelList[i].get());
        gameData.metagameFile->addInstance(itemToAdd);
    }
}

void DataHandler::updateMetagameMinicons(){
    QStringList miniconTypes = {"Minicon", "MiniconDamageBonus", "MiniconArmor", "MiniconEmergencyWarpgate", "MiniconRangeBonus", "MiniconRegeneration"};
    //QStringList miniconTypes = {"Minicon"};
    //this could be an issue - some minicons are their own classes and inherit from minicon.
    for(int i = 0; i < miniconTypes.size(); i++){
        gameData.metagameFile->removeAll(miniconTypes[i]);
    }
    for(int i = 0; i < gameData.miniconList.size(); i++){
        taMinicon* tempMinicon = gameData.miniconList[i].get();
        qDebug() << Q_FUNC_INFO << "Adding minicon" << tempMinicon->name;
        dictItem itemToAdd = createMetagameMinicon(tempMinicon);
        gameData.metagameFile->addInstance(itemToAdd);
    }
}

void DataHandler::updateMetagemaAutobots(){
    gameData.metagameFile->removeAll("Autobot");
    for(int i = 0; i < gameData.autobotList.size(); i++){
        gameData.metagameFile->addInstance(gameData.autobotList[i]);
    }
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
    //instanceIndex = 0;
    linkedLocationIDs = std::vector<int>();
    spoiled = false;
}
