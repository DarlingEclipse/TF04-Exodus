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
    qDebug() << Q_FUNC_INFO << "adding pickup" << copyItem.instanceIndex << "name" << copyItem.name << "|" << pickupToSpawn << "has enumID" << enumID << "is minicon?" << isMinicon();
    placed = false;
}

Pickup::Pickup(){
    dataID = 99;
    isWeapon = false;
    setAttribute("Position", "0, 0, 0");
}


exMinicon::exMinicon(exPickup copyItem){
    //this->name = copyItem.name;
    this->pickupToSpawn = copyItem.pickupToSpawn;
    //this->attributes = copyItem.attributes;
    hasVanillaPlacement = false;
}

void exMinicon::setCreature(exPickup copyItem){
    this->enumID = copyItem.enumID;
    //this->instanceIndex = copyItem.instanceIndex;
    //this->isWeapon = copyItem.isWeapon;
    placed = false;
}

exMinicon::exMinicon(){
    //instanceIndex = 0;
    isWeapon = false;
    pickupToSpawn = "Default Shockpunch";
    enumID = 27;
    //setAttribute("Position", "0, 0, 0");
}

exMinicon* DataHandler::getMinicon(int searchID){
    if(searchID == 3){
        qDebug() << Q_FUNC_INFO << "Searching for a datacon instead of minicon. This shouldn't happen.";
        return nullptr;
    }
    for(int i = 0; i < miniconList.size(); i++){
        if(miniconList[i].enumID == searchID){
            return &miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchID << "was not found.";
    return nullptr;
}

exMinicon* DataHandler::getMinicon(QString searchName){
    for(int i = 0; i < miniconList.size(); i++){
        if(miniconList[i].pickupToSpawn == searchName){
            return &miniconList[i];
        }
    }
    qDebug() << Q_FUNC_INFO << "minicon" << searchName << "was not found.";
    return nullptr;
}

void DataHandler::resetMinicons(){
    for(int i = 0; i < miniconList.size(); i++){
        miniconList[i].placed = false;
    }
}

void DataHandler::resetDatacons(){
    for(int i = 0; i < dataconList.size(); i++){
        dataconList[i].placed = false;
    }
}

void DataHandler::resetLevels(){
    for(int i = 0; i < levelList.size(); i++){
        levelList[i].dataconCount = 0;
        levelList[i].miniconCount = 0;
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

void DataHandler::loadLevels(){
    for(int i = 0; i < 9; i++){
        taEpisode nextLevel;
        nextLevel.miniconCount = 0;
        nextLevel.dataconCount = 0;
        switch(i){
        case 0: //Amazon 1
            nextLevel.levelName = "Amazon";
            nextLevel.outputName = "01_AMAZON_1";
            break;
        case 1: //Antarctica
            nextLevel.levelName = "Antarctica";
            nextLevel.outputName = "02_ANTARCTICA";
            break;
        case 2: //Amazon 2
            nextLevel.levelName = "Deep Amazon";
            nextLevel.outputName = "03_AMAZON_2";
            break;
        case 3: //Mid Atlantic Carrier
            nextLevel.levelName = "Mid-Atlantic with Carrier";
            nextLevel.outputName = "04_AIRCRAFTCARRIER";
            break;
        case 4: //Mid Atlantic Defeated
            nextLevel.levelName = "Mid-Atlantic Empty";
            nextLevel.outputName = "04_AIRCRAFTCARRIERDEFEATED";
            break;
        case 5: //Alaska
            nextLevel.levelName = "Alaska";
            nextLevel.outputName = "05_GREENLAND";
            break;
        case 6: //Starship
            nextLevel.levelName = "Starship";
            nextLevel.outputName = "06_SPACESHIP";
            break;
        case 7: //Pacific Island
            nextLevel.levelName = "Pacific Island";
            nextLevel.outputName = "07_EASTERISLAND";
            break;
        case 8: //Cybertron
            nextLevel.levelName = "Cybertron";
            nextLevel.outputName = "08_CYBERTRON";
            break;
        }
        for(int j = 0; j < parent->databaseList.size(); j++){
            if(parent->databaseList[j]->fileName.contains(nextLevel.outputName)){
                nextLevel.fromFile = parent->databaseList[j];
            }
        }
        levelList.push_back(nextLevel);
    }

    int totalRemoved = 0;
    std::vector<exPickupLocation> tempLocations;
    for(int i = 0; i < levelList.size(); i++){
        std::vector<taPickupPlaced> filePickupsBase;
        /*conversion function for taPickupPlaced still needs to be defined. Also, this could probably go directly to exPickupLocation and simplify this whole loop*/
        filePickupsBase = convertInstances<taPickupPlaced>(levelList[i].fromFile->sendInstances("PickupPlaced"));
        //std::vector<PickupLocation> filePickups = convertInstances<PickupLocation>(filePickupsBase);
        levelList[i].maxInstances = levelList[i].fromFile->maxInstances;
        levelList[i].removedInstances = filePickupsBase.size()-2; //to account for glide and highjump placements
        totalRemoved += levelList[i].removedInstances;
        for(int pickup = 0; pickup < filePickupsBase.size(); pickup++){
            tempLocations.push_back(filePickupsBase[pickup]);
            tempLocations[tempLocations.size()-1].gameID[0] = i;
            tempLocations[tempLocations.size()-1].gameID[1] = filePickupsBase[pickup].instanceIndex;
            //tempLocations[tempLocations.size()-1].position = filePickupsBase[pickup].searchAttributes<QVector3D>("Position");
            tempLocations[tempLocations.size()-1].position = filePickupsBase[pickup].position;
            pickupList.push_back(filePickupsBase[pickup]);
        }
    }

    int id = 0;

    foreach(exPickupLocation currentLocation, tempLocations){
        currentLocation.uniqueID = id;
        currentLocation.originalDatabaseInstance = tempLocations[id].gameID[1];
        //addedLocation.setAttribute("PickupToSpawn", "0");

        //to be placed inside the switch:
        currentLocation.locationName = "Unnamed Location";
        switch(currentLocation.gameID[0]){
        case 0: //Amazon 1
            currentLocation.slipstreamDifficulty = 0;
            currentLocation.highjumpDifficulty = 0;
            switch(currentLocation.uniqueID){
                case 0:
                    currentLocation.locationName = "Claymore Cave";
                    break;
                case 18: //1
                    currentLocation.locationName = "Ruined Temple Courtyard";
                    break;
                case 2:
                    currentLocation.slipstreamDifficulty = 3;
                    currentLocation.locationName = "Spire";
                    break;
                case 17: //3
                    currentLocation.locationName = "Tutorial Minicon";
                    break;
                case 4:
                    currentLocation.locationName = "Neighboring Mountain";
                    break;
                case 5:
                    currentLocation.locationName = "Ravine Cliff Cave";
                    break;
                case 6:
                    currentLocation.locationName = "Pressurepoint's Corner";
                    break;
                case 7:
                    currentLocation.locationName = "Foot of the Mountain";
                    break;
                case 8:
                    currentLocation.locationName = "Mountain Ruins";
                    break;
                case 9:
                    currentLocation.locationName = "Before the Waterfall Bridge";
                    break;
                case 20: //10
                    currentLocation.locationName = "Hidden Cave";
                    break;
                case 11:
                    currentLocation.locationName = "Spidertank Triplets";
                    break;
                case 12:
                    currentLocation.locationName = "Ruined Temple Alcove";
                    break;
                case 13:
                    currentLocation.locationName = "Forest before the Bridge";
                    break;
                case 19: //14
                    currentLocation.locationName = "Light Unit Party";
                    break;
                case 15:
                    currentLocation.locationName = "Ruined Temple Ledge";
                    break;
                case 16:
                    currentLocation.locationName = "Forest before Firefight";
                    break;
                case 21:
                    currentLocation.locationName = "Riverside Ledge";
                    break;
                case 22:
                    currentLocation.locationName = "Waterfall Base";
                    break;
                case 23:
                    currentLocation.locationName = "Forest after the Bridge";
                    break;
                case 24:
                    currentLocation.locationName = "Surprise";
                    break;
                case 25:
                    currentLocation.locationName = "Forgettable Location";
                    break;
                case 26:
                    currentLocation.locationName = "Medium Unit Party";
                    break;
                case 27:
                    currentLocation.locationName = "Forest Across from the Basin 1";
                    break;
                case 28:
                    currentLocation.locationName = "Forest Across from the Basin 2";
                    break;
                case 42069:
                    currentLocation.locationName = "Beginner Weapon";
                    break;
                }
            break;
        case 1: //Antarctica
            currentLocation.slipstreamDifficulty = 0;
            currentLocation.highjumpDifficulty = 0;
            switch(currentLocation.uniqueID){
                case 49: //29
                    currentLocation.locationName = "Crevasse Field Overlook";
                    break;
                case 30:
                    currentLocation.locationName = "Rock of Power";
                    currentLocation.highjumpDifficulty = 1;
                    break;
                case 45: //31
                    currentLocation.locationName = "Research Center Hangar";
                    break;
                case 44: //32
                    currentLocation.locationName = "Crashed Icebreaker";
                    break;
                case 47: //33
                    currentLocation.locationName = "Crashed Plane";
                    break;
                case 34:
                    currentLocation.locationName = "Research Base Office 1";
                    break;
                case 46: //35
                    currentLocation.locationName = "Beachfront Property";
                    break;
                case 51: //36
                    currentLocation.locationName = "Research Base Office 2";
                    break;
                case 37:
                    currentLocation.locationName = "Midfield Beacon";
                    break;
                case 38:
                    currentLocation.locationName = "Hide-and-Seek Champion";
                    break;
                case 39:
                    currentLocation.locationName = "Research Base Containers 1";
                    break;
                case 40:
                    currentLocation.locationName = "Research Base Containers 2";
                    break;
                case 41:
                    currentLocation.locationName = "Research Base Office 3";
                    break;
                case 48: //42
                    currentLocation.locationName = "Research Base Office 4";
                    break;
                case 50: //43
                    currentLocation.locationName = "Lonely Island";
                    break;
                case 52:
                    currentLocation.locationName = "Across the Field";
                    break;
            }
            break;
        case 2: //Amazon 2
            currentLocation.slipstreamDifficulty = 0;
            currentLocation.highjumpDifficulty = 0;
            switch(currentLocation.uniqueID){
                case 53:
                    currentLocation.locationName = "Island Altar";
                    break;
                case 67: //54
                    currentLocation.locationName = "Jungle Village Warpgate";
                    break;
                case 55:
                    currentLocation.locationName = "Top of the Temple";
                    break;
                case 66: //56
                    currentLocation.locationName = "Village at the Foot of the Hill";
                    break;
                case 57:
                    currentLocation.locationName = "Spawn Ledge";
                    break;
                case 58:
                    currentLocation.locationName = "Waterfall Rock";
                    break;
                case 59:
                    currentLocation.locationName = "Jungle Ruins";
                    break;
                case 60:
                    currentLocation.slipstreamDifficulty = 4;
                    currentLocation.locationName = "Patrolling Dropship";
                    break;
                case 61:
                    currentLocation.locationName = "Jungle Village Outskirts";
                    break;
                case 62:
                    currentLocation.locationName = "Field before the Bridge";
                    break;
                case 69: //63
                    currentLocation.locationName = "Temple Pool";
                    break;
                case 64:
                    currentLocation.locationName = "Temple Climb";
                    break;
                case 65:
                    currentLocation.locationName = "Mid-Jungle Ditch";
                    break;
                case 68:
                    currentLocation.locationName = "Temple Dead End";
                    break;
                case 70:
                    currentLocation.locationName = "Temple Side-Path";
                    break;
                case 71:
                    currentLocation.locationName = "Forest before the Bridge";
                    break;
                case 72:
                    currentLocation.locationName = "Antechamber";
                    break;
                case 73:
                    currentLocation.locationName = "Antechamber Alcove";
                    currentLocation.highjumpDifficulty = 2;
                    break;
                case 74:
                    currentLocation.locationName = "Behind the Temple";
                    break;
            }
            break;
        case 3: //Mid atlantic 1
            currentLocation.slipstreamDifficulty = 0;
            currentLocation.highjumpDifficulty = 0;
            switch(currentLocation.uniqueID){
                case 75:
                    currentLocation.linkedLocationID = 79;
                    currentLocation.locationName = "Vanilla Progression";
                    break;
                case 76:
                    currentLocation.linkedLocationID = 80;
                    currentLocation.slipstreamDifficulty = 5;
                    //this is the only location that is completely impossible without slipstream
                    currentLocation.locationName = "Distant Island";
                    break;
                case 77:
                    currentLocation.linkedLocationID = 82;
                    currentLocation.locationName = "Pinnacle Rock";
                    break;
                case 78:
                    currentLocation.linkedLocationID = 81;
                    currentLocation.locationName = "Atoll";
                    break;
            }
            break;
        case 4: //Mid atlantic 2
            currentLocation.slipstreamDifficulty = 3;
            currentLocation.highjumpDifficulty = 0;
            switch(currentLocation.uniqueID){
                case 79:
                    currentLocation.linkedLocationID = 75;
                    currentLocation.locationName = "Vanilla Progression";
                    break;
                case 80:
                    currentLocation.linkedLocationID = 76;
                    currentLocation.slipstreamDifficulty = 5;
                    //this is the only location that is completely impossible without slipstream
                    currentLocation.locationName = "Distant Island";
                    break;
                case 81:
                    currentLocation.linkedLocationID = 78;
                    currentLocation.locationName = "Atoll";
                    break;
                case 82:
                    currentLocation.linkedLocationID = 77;
                    currentLocation.locationName = "Pinnacle Rock";
                    break;
            }
            break;
        case 5: //Alaska
            currentLocation.slipstreamDifficulty = 4;
            currentLocation.highjumpDifficulty = 3; //can be lower for early placements
            switch(currentLocation.uniqueID){
                case 83:
                    currentLocation.locationName = "Mountaintop";
                    currentLocation.slipstreamDifficulty = 5;
                    currentLocation.highjumpDifficulty = 4;
                    break;
                case 84:
                    currentLocation.locationName = "Small Forest";
                    break;
                case 98: //85
                    currentLocation.locationName = "Peninsula";
                    break;
                case 86:
                    currentLocation.locationName = "Cave's Far Ledge";
                    currentLocation.slipstreamDifficulty = 5;
                    currentLocation.highjumpDifficulty = 4;
                    break;
                case 87:
                    currentLocation.locationName = "Cave's Right Fork";
                    currentLocation.slipstreamDifficulty = 5;
                    currentLocation.highjumpDifficulty = 4;
                    break;
                case 88:
                    currentLocation.locationName = "Cave's Left Fork";
                    currentLocation.slipstreamDifficulty = 5;
                    currentLocation.highjumpDifficulty = 4;
                    break;
                case 89:
                    currentLocation.locationName = "Cave's Pool";
                    currentLocation.slipstreamDifficulty = 5;
                    currentLocation.highjumpDifficulty = 4;
                    break;
                case 90:
                    currentLocation.locationName = "Along the Canyon";
                    break;
                case 91:
                    currentLocation.locationName = "Ocean Overlook";
                    break;
                case 97: //92
                    currentLocation.locationName = "Dropship Island";
                    break;
                case 93:
                    currentLocation.locationName = "Hidden Hill";
                    break;
                case 94:
                    currentLocation.locationName = "Canyon Clearing Ledge 1";
                    break;
                case 95:
                    currentLocation.locationName = "Battlefield";
                    break;
                case 96:
                    currentLocation.locationName = "Canyon Clearing Ledge 2";
                    break;
                case 99:
                    currentLocation.locationName = "Escape";
                    currentLocation.slipstreamDifficulty = 5;
                    break;
                case 100:
                    currentLocation.locationName = "Hillside";
                    break;

            }
            break;
        case 6: //Starship
            //must have either Slipstream or Highjump before starship, even for cheaters
            currentLocation.slipstreamDifficulty = 5;
            currentLocation.highjumpDifficulty = 6;
            switch(currentLocation.uniqueID){
                case 101:
                    currentLocation.locationName = "Start of the Climb";
                    break;
                case 102:
                    currentLocation.locationName = "First Steps";
                    break;
                case 103:
                    currentLocation.locationName = "Main Elevator";
                    break;
                case 104:
                    currentLocation.locationName = "Sideways Elevator";
                    break;
                case 105:
                    currentLocation.locationName = "Lower Dropship";
                    break;
                case 106:
                    currentLocation.locationName = "Bait";
                    break;
                case 107:
                    currentLocation.locationName = "Free Space";
                    break;
                case 108:
                    currentLocation.locationName = "Across the Gap";
                    break;
                case 109:
                    currentLocation.locationName = "Early Gift";
                    break;
                case 110:
                    currentLocation.slipstreamDifficulty = 4;
                    //slipstream can technically be on the Bridge as long as highjump is acquired before then
                    currentLocation.linkedLocationID = 111;
                    currentLocation.locationName = "Pristine Bridge";
                    break;
                case 111:
                    currentLocation.linkedLocationID = 110;
                    currentLocation.locationName = "Crashed Bridge";
                    break;
                case 112:
                    currentLocation.locationName = "Top of the Crashed Ship";
                    break;
                case 113:
                    currentLocation.locationName = "Dropdown 1";
                    break;
                case 114:
                    currentLocation.locationName = "Risky Jump";
                    break;
                case 115:
                    currentLocation.locationName = "Rock 1";
                    break;
                case 116:
                    currentLocation.locationName = "Middle Dropship";
                    break;
                case 117:
                    currentLocation.locationName = "Dropdown 2";
                    break;
                case 118:
                    currentLocation.locationName = "Rock 2";
                    break;
                case 119:
                    currentLocation.locationName = "Rock 3";
                    break;
                case 120:
                    currentLocation.locationName = "Rock 4";
                    break;
            }
            break;
        case 7: //Pacific Island
            currentLocation.slipstreamDifficulty = 5;
            currentLocation.highjumpDifficulty = 6;
            switch(currentLocation.uniqueID){
                case 132: //121
                    currentLocation.locationName = "Forest Basin";
                    break;
                case 122:
                    currentLocation.locationName = "Small Silo";
                    break;
                case 123:
                    currentLocation.locationName = "Moai";
                    break;
                case 124:
                    currentLocation.locationName = "Back of the Volcano";
                    break;
                case 125:
                    currentLocation.locationName = "Volcano Path";
                    break;
                case 133: //126
                    currentLocation.locationName = "Hidden Ledge";
                    break;
                case 127:
                    currentLocation.locationName = "Lighthouse";
                    break;
                case 128:
                    currentLocation.locationName = "Waterfall Climb";
                    break;
                case 129:
                    currentLocation.locationName = "Stronghold Ledge";
                    break;
                case 136: //130
                    currentLocation.locationName = "Small Island";
                    break;
                case 131:
                    currentLocation.locationName = "River Basin Peak (I think?)";
                    break;
                case 134:
                    currentLocation.locationName = "Above the Waterfall";
                    break;
                case 135:
                    currentLocation.locationName = "Large Silo";
                    break;
                case 137:
                    currentLocation.locationName = "Bunker 1";
                    currentLocation.bunkerID = 526;
                    break;
                case 138:
                    currentLocation.locationName = "Bunker 2";
                    currentLocation.bunkerID = 525;
                    break;
                case 139:
                    currentLocation.locationName = "Bunker 3";
                    currentLocation.bunkerID = 522;
                    break;
                case 140:
                    currentLocation.locationName = "Bunker 4";
                    currentLocation.bunkerID = 521;
                    break;
                case 141:
                    currentLocation.locationName = "Bunker 5";
                    currentLocation.bunkerID = 527;
                    break;
                case 142:
                    currentLocation.locationName = "Bunker 6";
                    currentLocation.bunkerID = 523;
                    break;
                case 143:
                    currentLocation.locationName = "Bunker 7";
                    currentLocation.bunkerID = 520;
                    break;
                case 144:
                    currentLocation.locationName = "Bunker 8";
                    currentLocation.bunkerID = 519;
                    break;
                case 145:
                    currentLocation.locationName = "Bunker 9";
                    currentLocation.bunkerID = 524;
                    break;
            }
            break;
        default:
            currentLocation.slipstreamDifficulty = 6;
            currentLocation.highjumpDifficulty = 6;
            qDebug() << Q_FUNC_INFO << "no defined positions for level" << currentLocation.gameID[0];
        }
        loadedLocations.push_back(currentLocation);
        id++;

    }

    id = 0;
    bool positionIsDuplicate = false;
    std::vector<exPickupLocation>::iterator currentLocation;
    for(currentLocation = loadedLocations.begin(); currentLocation != loadedLocations.end(); id++){
        positionIsDuplicate = duplicateLocation(*currentLocation);
        if (positionIsDuplicate){
            currentLocation = loadedLocations.erase(currentLocation);
        } else {
            currentLocation++;
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded locations:" << loadedLocations.size();
    for(int i = 0; i < loadedLocations.size(); i++){
        QVector3D debugPosition = loadedLocations[i].position;
        qDebug() << Q_FUNC_INFO << i << "   " << loadedLocations[i].originalDatabaseInstance << "  " << loadedLocations[i].uniqueID << "   " << loadedLocations[i].linkedLocationID << "    "
                 << loadedLocations[i].gameID[0] << "    " << loadedLocations[i].locationName << "    " << debugPosition.x()
                 << "   " << debugPosition.y() << "  " << debugPosition.z();
    }
}

dictItem DataHandler::createExodusPickupLocation(exPickupLocation location){
    taEpisode targetLevel = levelList[location.gameID[0]];
    dictItem convertedData;
    convertedData.name = "PickupPlaced";
    convertedData.attributes = targetLevel.fromFile->generateAttributes("PickupPlaced");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    convertedData.setAttribute("Position", QString::number(location.position.x()) + ", " + QString::number(location.position.y()) + ", " + QString::number(location.position.z()));

    convertedData.setAttribute("PickupToSpawn", QString::number(location.assignedPickup()->enumID));

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

void DataHandler::loadMinicons(){

    std::shared_ptr<DatabaseFile> metagameFile;
    std::vector<dictItem> metagameMinicons;
    for(int i = 0; i < parent->databaseList.size(); i++){
        qDebug() << Q_FUNC_INFO << "checking file name" << parent->databaseList[i]->fileName;
        if(parent->databaseList[i]->fileName == "TFA-METAGAME"){
            metagameFile = parent->databaseList[i];
        }
    }
    QStringList miniconTypes = {"Minicon", "MiniconDamageBonus", "MiniconArmor", "MiniconEmergencyWarpgate", "MiniconRangeBonus", "MiniconRegeneration"};

    for(int type = 0; type < miniconTypes.size(); type++){
        metagameMinicons = metagameFile->sendInstances(miniconTypes[type]);
        for(int i = 0; i < metagameMinicons.size(); i++){
            miniconList.push_back(exPickup(metagameMinicons[i]));
            /*QString currentName = metagameMinicons[i].searchAttributes<QString>("Name");
            Minicon *currentMinicon = getMinicon(currentName);
            if(currentMinicon != nullptr){
                //qDebug() << Q_FUNC_INFO << "successfully found minicon" << currentName << "with enumID" << currentMinicon->enumID;
                currentMinicon->name = metagameMinicons[i].name;
                currentMinicon->attributes = metagameMinicons[i].attributes;
            }*/
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded minicons:" << miniconList.size();
    for(int i = 0; i < miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << "loaded minicon" << i << "is named" << miniconList[i].pickupToSpawn;
    }

    for(int i = 0; i < levelList.size(); i++){
        std::vector<taPickupPlaced> filePickupsBase = convertInstances<taPickupPlaced>(levelList[i].fromFile->sendInstances("PickupPlaced"));
        for(int pickup = 0; pickup < filePickupsBase.size(); pickup++){
            //qDebug() << Q_FUNC_INFO << "pickup" << pickup << "has instance ID" << filePickupsBase[pickup].instanceIndex << "and name" << filePickupsBase[pickup].name;
            pickupList.push_back(filePickupsBase[pickup]);
        }
    }

    for(int i = 0; i < miniconList.size(); i++){
        foreach(exPickup currentPickup, pickupList){
            bool miniconIsLoaded = false;
            //qDebug() << Q_FUNC_INFO << "comparing minicon" << currentMinicon.pickupToSpawn << "to pickup" << currentPickup.pickupToSpawn;
            if(miniconList[i].pickupToSpawn != currentPickup.pickupToSpawn){
                //Only process minicons that have a placement.
                continue;
            }
            miniconIsLoaded = miniconLoaded(currentPickup.enumID);
            //dataconIsLoaded = dataconLoaded(currentPickup.dataID);

            qDebug() << Q_FUNC_INFO << "minicon" << miniconList[i].pickupToSpawn << "is loaded?" << miniconIsLoaded << "should get enumID" << currentPickup.enumID;
            if(!miniconIsLoaded && currentPickup.enumID != 3){
                //if it's a minicon we don't already have, add it to the minicon list
                qDebug() << Q_FUNC_INFO << "Doing initial minicon load for" << currentPickup.enumID;
                qDebug() << Q_FUNC_INFO << "Does weapon list contain?" << weaponList.contains(currentPickup.enumID) ;
                if(weaponList.contains(currentPickup.enumID)){
                    qDebug() << Q_FUNC_INFO << "setting to weapon";
                    miniconList[i].isWeapon = true;
                }
                miniconList[i].hasVanillaPlacement = true;
                miniconList[i].setCreature(currentPickup);
                qDebug() << Q_FUNC_INFO << "minicon enum ID set to" << miniconList[i].enumID;
            } else if (miniconIsLoaded && currentPickup.enumID != 3){
                //if it's a minicon we already have, don't add it again
                continue;
            } else {
                //this should never happen, logically, but better safe than confused.
                //put some debugs here, just in case
                continue;
            }
        }
    }

    /*
    }*/

    qDebug() << Q_FUNC_INFO << "Remaining pickups to process (should be 0):" << pickupList.size();

    qDebug() << Q_FUNC_INFO << "Total loaded minicons:" << miniconList.size();
    for(int i = 0; i < miniconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << miniconList[i].enumID << "  " << miniconList[i].pickupToSpawn << "    "
                 << miniconList[i].dataID << " is weapon:" << miniconList[i].isWeapon;
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

    autobotList = metagameFile->sendInstances("Autobot");
}

void DataHandler::loadDatacons(){
    foreach(exPickup currentPickup, pickupList){
        bool dataconIsLoaded = false;
        if(currentPickup.isMinicon()){
            continue;
        }
        qDebug() << Q_FUNC_INFO << "pickup properties for:" << currentPickup.pickupToSpawn << "enumID" << currentPickup.enumID << "dataID" << currentPickup.dataID;
        dataconIsLoaded = dataconLoaded(currentPickup.dataID);
        qDebug() << Q_FUNC_INFO << "already loaded?" << dataconIsLoaded;
        if (!dataconIsLoaded && currentPickup.enumID == 3){
            //we now know it's a datacon (but still check to be sure). if it hasn't been loaded, add it to the datacon list
            dataconList.push_back(currentPickup);
        } else if (dataconIsLoaded && currentPickup.enumID == 3){
            //if it has been loaded, just skip it
            continue;
        } else {
            //this should never happen, logically, but better safe than confused.
            //put some debugs here, just in case
            continue;
        }
    }

    qDebug() << Q_FUNC_INFO << "Total loaded datacons:" << dataconList.size();
    for(int i = 0; i < dataconList.size(); i++){
        qDebug() << Q_FUNC_INFO << i << " " << dataconList[i].enumID << "  " << dataconList[i].pickupToSpawn << "    "
                 << dataconList[i].dataID;
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
            int targetLevel = -1;
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
                customLocation.gameID[0] = targetLevel;
                //double-check that the below doesn't need to find the specific database for the target level
                //customLocation.attributes = levelList[0].levelFile->generateAttributes("PickupPlaced");
                bool readingLocation = true;
                while(readingLocation){
                    modProperty = modBuffer.readProperty();
                    switch(propertyOptions.indexOf(modProperty.name)){
                        case 5: //Level
                            for(int i = 0; i < levelList.size(); i++){
                                qDebug() << Q_FUNC_INFO << "comparing level" << levelList[i].levelName << "to" << modProperty.readValue;
                                if(levelList[i].levelName == modProperty.readValue){
                                    qDebug() << Q_FUNC_INFO << "comparing level: match";
                                    customLocation.gameID[0] = i;
                                }
                            }
                            if(customLocation.gameID[0] == -1){
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
                                customLocation.linkedLocationID = locationValue;
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
                qDebug() << Q_FUNC_INFO << "Adding location" << customLocation.locationName << "for level" << customLocation.gameID[0] << "at coordinates" << customLocation.position;
                currentLocations.locationList.push_back(customLocation);
            }
            customLocationList.push_back(currentLocations);
        }
        qDebug() << Q_FUNC_INFO << "file" << currentModFile.fileName();
    }
}



bool DataHandler::duplicateLocation(exPickupLocation testLocation){
    QVector3D loadedPosition;
    QVector3D testPosition = testLocation.position;
    int locationCount = 0;
    for(int i = 0; i < loadedLocations.size(); i++){
        loadedPosition = loadedLocations[i].position;
        if((testPosition.x() < loadedPosition.x()+5 && testPosition.x() > loadedPosition.x() - 5)
                && (testPosition.y() < loadedPosition.y()+5 && testPosition.y() > loadedPosition.y() - 5)
                && (testPosition.z() < loadedPosition.z()+5 && testPosition.z() > loadedPosition.z() - 5)
                && (testLocation.gameID[0] == loadedLocations[i].gameID[0])){
            locationCount++;
        }
        if(locationCount > 1){
           return true;
        }
    }
    return false;
}

bool DataHandler::miniconLoaded(int checkID){
    for(int i = 0; i < miniconList.size(); i++){
        if(checkID == miniconList[i].enumID){
            return true;
        }
    }
    return false;
}

bool DataHandler::dataconLoaded(int checkID){
    int pickupCount = 0;
    for(int i = 0; i < dataconList.size(); i++){
        if(checkID == dataconList[i].dataID){
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
    originalDatabaseInstance = fromItem.instanceIndex;
    //attributes = fromItem.attributes;
    //assignMinicon(0);
    spoiled = false;
    bunkerID = 0;
    instanceIndex = 0;
    linkedLocationID = 999;
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
    originalDatabaseInstance = 0;
    assignPickup(nullptr);
    bunkerID = 0;
    instanceIndex = 0;
    linkedLocationID = 999;
    spoiled = false;
}
