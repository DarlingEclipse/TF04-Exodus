#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "Headers/Databases/Database.h"

/*For handling data from database files in a more detailed manner*/

class exWarpgate : public dictItem{
public:
    double x_position;
    double y_position;
    double z_position;
    bool isStartingGate;
    bool hasScript;

    exWarpgate(dictItem copyItem);
    exWarpgate();
    static std::vector<exWarpgate*> createAmazonWarpgates();
};

class taCreatureWarpgate{
public:
    bool alternativeGate;
    bool destructibleGate;
    bool finalSpaceshipGate;
    bool generate;
    enum class generationDifficulty{Normal, Hard, Extreme};
    enum class generationSpawnType{Narrative, Always, LevelDone};
    QQuaternion orientation;
    QVector3D position;
    QString spawnedPlayer_event;
    enum class warpgateNumber{WarpGate_Start, WarpGate_Gate1};
};

class Pickup : public dictItem{
public:
    bool isWeapon;
    int spawnDifficulty;
    int enumID;
    int dataID;
    bool placed;
    QString pickupToSpawn;

    Pickup(dictItem copyItem);
    Pickup();

    const bool isMinicon(){
        if(enumID > 3){
            return true;
        }
        return false;
    };


    bool operator < (const Pickup& compPickup) const
    {
        return (enumID < compPickup.enumID);
    }

    bool operator == (const Pickup& compPickup) const
    {
        if(dataID != 99){
            return (dataID == compPickup.dataID);
        } else {
            return (enumID == compPickup.enumID);
        }
    }
};

class exPickupLocation{
public:
    std::array<int, 2> gameID; //level #, instance index
    int uniqueID; //randomizer ID

    /*The difficulties are determined by how hard it is to get to that location from the start of the level*/
    int slipstreamDifficulty;
    int highjumpDifficulty;
    QString locationName;
    QString spawnEvent;
    int originalDatabaseInstance;
    bool spoiled;
    int bunkerID;
    int instanceIndex;
    int linkedLocationID;
    QVector3D position;
    Pickup* pickup;

    std::vector<exPickupLocation*> linkedLocations; //if a minicon is placed at one of these, it's placed at both of them
    //only used for starship (vanilla Aftershock) and mid-atlantic, since it has separate database files

    exPickupLocation();
    exPickupLocation(dictItem fromItem);
    Pickup* assignedPickup();
    void assignPickup(Pickup* pickupToPlace);
    void generateAttributes(); //for custom locations
    //implementing the < operator so we can simply sort the location vector
    //using std::sort instead of making a whole function for it
    bool operator < (const exPickupLocation& compLocation) const
    {
        return (gameID[0] < compLocation.gameID[0]);
    }
};

class taEpisode{
public:
    /*Existing members before rework*/
    std::shared_ptr<DatabaseFile> levelFile;
    QString levelName;
    QString outputName;
    //int miniconCount;
    //int dataconCount;
    int maxInstances;
    int removedInstances;

    /*all members from metagame.tmd*/
    QString alternativeDirectoryName;
    enum class bossType{None, Starscream, Cyclonus, Megatron, TidalWave, Unicron};
    int dataconCount;
    QString directoryName;
    enum class episode{Amazon_1, Antarctica, Amazon_2, AircraftCarrier, Greenland, Spaceship, EasterIsland, Cybertron};
    bool hasAlternativeDirectory;
    int miniconCount;
    int miniconUnlockCountExtreme;
    int miniconUnlockCountHard;
    int miniconUnlockCountNormal;
    QString name;
    std::vector<int> WarpGateMusicTrack;
    int warpgates;
};

class taPickupPlaced{
public:
    int datacon_HealthPickupCount_Large; //0
    int datacon_HealthPickupCount_Small; //2
    enum class gameModes{None, BossMode, ExploreMode}; //None, probably not used
    bool generate; //true
    enum class generationDifficulty{Always, CadetAndVeteranOnly, CommanderOnly}; //Always
    enum class generationGameBuildType{Any, FullGame, Demo}; //Any
    enum class generationSpawnType{Narrative, Always, LevelDone}; //Always
    int linkedDropship; //null
    QQuaternion orientation; //1 0 0 0
    enum class pickupToSpawn{LIST}; //not typing all that. default Health_Small
    QVector3D position; //0 0 0
    enum class productArt{LIST}; //really not typing all that. default None
    QString spawnEvent; //""
    QString synchronizeEvent; //""
};

class exMinicon : public Pickup{
public:
    bool hasVanillaPlacement;
    bool isExplosive;
    bool isSlipstream;
    bool isHighjump;

    void setCreature(Pickup copyItem);
    exMinicon(Pickup copyItem);
    exMinicon();
};

class taMinicon{
    /*From METAGAME.TMD*/
public:
    enum class activationType{HoldDown, AlwaysOn, PressInstant, ReleaseInstant, PressAndRelease, ChargeRelease, ChargeReleasePartial, Toggle, ToggleDrain, HoldDownDrain};
    float chargeDrainRate_Commander;
    float chargeDrainRate_Veteran;
    float chargeDrainTime;
    float coolDownTime;
    float coolDownTimeDepleted;
    enum class crosshairIndex{None, AssaultBlaster, Lockon, Slingshot, Claymore, Tractor, Firefight, Hailstorm, Landslide, Smackdown, Watchdog, Lookout, Sparkjump, Skirmish, Airburst, Sandstorm, Twister, Discord, Corona, Aftershock, Aurora, Failsafe, Jumpstart, Endgame, UltimatePrimary, UltimateSecondary};
    QString description;
    bool equipInHQ;
    enum class icon{Armor, Beam, Blaster, Exotic, Lobbed, Melee, Missile, Movement, Repair, Shield, Stealth, Vision, None};
    enum class minicon{LIST}; //same as above
    float minimumChargeToUse;
    float minimumChargeToUsePerShot;
    QString name;
    int nodeToKeepIndex;
    int paletteIndex;
    int powerCost;
    float rechargeTime;
    enum class recoilType{None, Small, Large};
    enum class restrictToButton{Default, L2, R2, L1, R1};
    int segments;
    float sidekickChargeDrainTime;
    float sidekickCoolDownTime;
    float sidekickCoolDownTimeDepleted;
    float sidekickMinimumChargeToUse;
    float sidekickRechargeTime;
    int sidekickSegments;
    enum class slot{PrimaryWeapon, SecondaryWeapon, Movement, Ability};
    enum class team{Green, Blue, Red, Purple, Gold, None};
    QString toneLibrary;
    enum class UI_DamagePri{NA, UseFloat, UseFloatX2, UseFloatX4, Variable, AutobotMeleexFloat, plusFloat, FloatPerSec};
    float UI_DamagePriFloat;
    enum class UI_DamageSec{NA, UseFloat, UseFloatx4, Variable, Special, FloatPerSec};
    float UI_DamageSecFloat;
    enum class UI_Defence{NA, Float, AllBeamAndBlaster, AllBeamAndMelee, AllMissileAndMelee};
    enum class UI_Duration{NA, Constant, FloatBoosts, FloatSec, Float, Instant};
    float UI_DurationFloat;
    enum class UI_Range{NA, UseRangeFloat, Melee, LineOfSight, PlusPercent, Drop};
    float UI_RangeDist;
    enum class UI_Recharge{NA, FloatPtsPerSec, FloatSec, FloatSecOrHQ, HQ};
    enum class UI_Rounds{NA, UseInt, Variable};
    int UI_RoundsInt;
};

class taMiniconArmor : public taMinicon {
public:
    float AbsorbPercentage;
    float MaxAbsorbPerHit;
    float MaxDamageToAbsorb;
};

class ExodusOption{
public:
    QString name;
    QString author;
    QString description;
    bool enabled;
};

class CustomLocations : public ExodusOption{
public:
    int locationCount;
    std::vector<exPickupLocation> locationList;
};

class DataHandler{
public:
    inline const static QList<int> weaponList {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 27, 30, 36, 41, 46, 51};
    inline const static QList<int> bunkerList {137,140,141,142,143,144,145};

    ProgWindow* parent;

/*each of these lists should be moved to their respective users - datahandler should just populate the lists
Maybe something can hold game object data from the databases - or have one for game data, one for exodus data?
then the datahandler is just some functions to convert that data from generic dictItems to their specific types
if we do separate them in this way, maybe the game classes can take on the "ta" prefix and exodus data can have "ex"*/
    //std::vector<dictItem> itemList;
    std::vector<Pickup> pickupList;
    std::vector<exMinicon> miniconList;
    std::vector<Pickup> dataconList;
    std::vector<dictItem> autobotList;
    std::vector<exPickupLocation> loadedLocations;
    std::vector<CustomLocations> customLocationList;
    std::vector<taEpisode> levelList;

    template <class instanceType>
    std::vector<instanceType> convertInstances(std::vector<dictItem> itemList){
        std::vector<instanceType> convertedList;
        for(int i = 0; i < itemList.size(); i++){
            instanceType nextInstance = instanceType(itemList[i]);
            convertedList.push_back(nextInstance);
        }
        return convertedList;
    };

    /*Used for converting processed location data into a database file. Should rarely need to be used.*/
    void createExodusLocationDatabase();

    dictItem createExodusPickupLocation(exPickupLocation location);
    void loadMinicons();
    void loadDatacons();
    void loadLevels();
    void loadAutobots();
    bool miniconLoaded(int checkID);
    bool dataconLoaded(int checkID);
    void loadCustomLocations();
    //void addCustomLocation(int locationID, int level, QVector3D location);
    exMinicon* getMinicon(int searchID);
    exMinicon* getMinicon(QString searchName);
    bool duplicateLocation(exPickupLocation testLocation);

    void resetMinicons();
    void resetDatacons();
    void resetLevels();

    /*std::vector<Pickup> convertPickups(std::vector<dictItem> itemList);
    std::vector<Minicon> convertMinicons(std::vector<dictItem> itemList);
    std::vector<Warpgate> convertWarpgates(std::vector<dictItem> itemList);*/
};

#endif // DATAHANDLER_H
