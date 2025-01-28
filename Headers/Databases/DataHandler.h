#ifndef DATAHANDLER_H
#define DATAHANDLER_H

#include "Headers/Databases/Database.h"

/*For handling data from database files in a more detailed manner*/

class taDatabaseInstance{
public:
    std::shared_ptr<DatabaseFile> fromFile;
    int instanceIndex;
};

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

class taCreatureWarpgate : public taDatabaseInstance{
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

enum class GameModes{None, BossMode, ExploreMode}; //None, probably not used
enum class GenerationDifficulty{Always, CadetAndVeteranOnly, CommanderOnly}; //Always
enum class GenerationGameBuildType{Any, FullGame, Demo}; //Any
enum class GenerationSpawnType{Narrative, Always, LevelDone}; //Always
enum class PickupToSpawn{LIST}; //not typing all that. default Health_Small
enum class ProductArt{LIST}; //really not typing all that. default None
class taPickupPlaced : public taDatabaseInstance{
public:
    int datacon_HealthPickupCount_Large; //0
    int datacon_HealthPickupCount_Small; //2
    GameModes gameModes;
    bool generate; //true
    GenerationDifficulty generationDifficulty;
    GenerationGameBuildType generationGameBuildType;
    GenerationSpawnType generationSpawnType;
    int linkedDropship; //null
    QQuaternion orientation; //1 0 0 0
    PickupToSpawn pickupToSpawn;
    QVector3D position; //0 0 0
    ProductArt productArt;
    QString spawnEvent; //""
    QString synchronizeEvent; //""

    taPickupPlaced(dictItem fromItem);
};

enum class BossType{None, Starscream, Cyclonus, Megatron, TidalWave, Unicron};
enum class Episode{Amazon_1, Antarctica, Amazon_2, AircraftCarrier, Greenland, Spaceship, EasterIsland, Cybertron};
class taEpisode : public taDatabaseInstance{
public:
    /*all members from metagame.tmd*/
    QString alternativeDirectoryName;
    BossType bossType;
    int dataconCount;
    QString directoryName;
    Episode episode;
    bool hasAlternativeDirectory;
    int miniconCount;
    int miniconUnlockCountExtreme;
    int miniconUnlockCountHard;
    int miniconUnlockCountNormal;
    QString name;
    std::vector<int> WarpGateMusicTrack;
    int warpgates;
};

class exPickup {
public:
    int pickupID; //connects to taPickupPlaced
    int dataID;
    QString pickupToSpawn; //for debugging and spoiler files

    //Set and used during randomizing
    bool placed;

    exPickup(taPickupPlaced copyItem);
    exPickup(dictItem copyItem);
    exPickup();

    const bool isMinicon(){
        if(pickupID > 3){
            return true;
        }
        return false;
    };


    bool operator < (const exPickup& compPickup) const
    {
        return (pickupID < compPickup.pickupID);
    }

    bool operator == (const exPickup& compPickup) const
    {
        if(dataID != 99){
            return (dataID == compPickup.dataID);
        } else {
            return (pickupID == compPickup.pickupID);
        }
    }
};

class exMinicon : public exPickup{
    /*Contains data useful for Exodus that is not defined in the METAGAME files. */
public:
    /*From Exodus database files*/
    int miniconID; //connects to taMinicon
    int rating;
    bool isWeapon;
    bool isExplosive;

    /*Set and used at runtime*/
    bool hasVanillaPlacement;
    bool isSlipstream;
    bool isHighjump;


    void setCreature(exPickup copyItem);
    exMinicon(dictItem copyItem);
    exMinicon();
};

enum class World{Amazon, Antarctica, DeepAmazon, MidAtlantic, MidAtlanticEmpty, Alaska, Spaceship, PacificIsland, Invalid};
class exPickupLocation{
public:
    World level;
    int gameID; //instance index
    QString levelName;
    int uniqueID; //randomizer ID

    /*The difficulties are determined by how hard it is to get to that location from the start of the level*/
    int slipstreamDifficulty;
    int highjumpDifficulty;
    bool requiresExplosive;
    /*Alternative requirements: list of minicons that must all be available if this location has a key minicon
    for example, revive, shield, and missiles for warship skip
    or highgear/fullspeed for the spire
    not sure about this one but it would solve the problem of complex requirements*/
    std::vector<int> alternativeRequirements;
    QString locationName;
    QString spawnEvent;
    int inputDatabaseInstance;
    int outputDatabaseInstance;
    bool spoiled;
    int bunkerID;
    QVector3D position;
    /*Generation difficulty: If the LocationChallenge is outside this range, then the location will not be added to the available
    location pool*/
    int minimumGenerationDifficulty;
    int maximumGenerationDifficulty;
    exPickup* pickup;

    std::vector<int> linkedLocationIDs; //if a minicon is placed at one of these, it's placed at both of them
    //so far only used for starship (vanilla Aftershock) and mid-atlantic, since it has separate database files

    exPickupLocation();
    exPickupLocation(dictItem copyItem);
    exPickupLocation(taPickupPlaced fromItem);
    exPickup* assignedPickup();
    void assignPickup(exPickup* pickupToPlace);
    void generateAttributes(); //for custom locations
    //implementing the < operator so we can simply sort the location vector
    //using std::sort instead of making a whole function for it
    bool operator < (const exPickupLocation& compLocation) const
    {
        return (level < compLocation.level);
    }
};

class exEpisode{
    /*Can link to taEpisode using outputFileName and comparing to directoryName and alternativeDirectoryName*/
public:
    //from database file
    QString logName;
    QString outputFileName;
    int requirements;
    /*whether the level needs highjump or slipstream to beat normally
     * Handling requirements this way because integerarrays won't work for a while
     * 0 = no requirements
     * 1 = slipstream
     * 2 = highjump
     * 3 = both*/
    std::vector<exPickupLocation> spawnLocations;

    //populated at runtime
    int assignedMinicons; //max 13 of any specific minicon without mods
    int assignedDatacons; //max 13 without mods
    std::shared_ptr<DatabaseFile> levelFile;

    bool addLocation(exPickupLocation locationToAdd);
    exEpisode(dictItem copyItem);
    exEpisode();
};

enum class ActivationType{HoldDown, AlwaysOn, PressInstant, ReleaseInstant, PressAndRelease, ChargeRelease, ChargeReleasePartial, Toggle, ToggleDrain, HoldDownDrain};
enum class CrosshairIndex{None, AssaultBlaster, Lockon, Slingshot, Claymore, Tractor, Firefight, Hailstorm, Landslide, Smackdown, Watchdog, Lookout, Sparkjump, Skirmish, Airburst, Sandstorm, Twister, Discord, Corona, Aftershock, Aurora, Failsafe, Jumpstart, Endgame, UltimatePrimary, UltimateSecondary};
enum class Icon{Armor, Beam, Blaster, Exotic, Lobbed, Melee, Missile, Movement, Repair, Shield, Stealth, Vision, None};
enum class MiniconNames{LIST}; //same comment as above
enum class RecoilType{None, Small, Large};
enum class RestrictToButton{Default, L2, R2, L1, R1};
enum class Slot{PrimaryWeapon, SecondaryWeapon, Movement, Ability};
enum class Team{Green, Blue, Red, Purple, Gold, None};
enum class UI_DamagePri{NA, UseFloat, UseFloatX2, UseFloatX4, Variable, AutobotMeleexFloat, plusFloat, FloatPerSec};
enum class UI_DamageSec{NA, UseFloat, UseFloatx4, Variable, Special, FloatPerSec};
enum class UI_Defence{NA, Float, AllBeamAndBlaster, AllBeamAndMelee, AllMissileAndMelee};
enum class UI_Duration{NA, Constant, FloatBoosts, FloatSec, Float, Instant};
enum class UI_Range{NA, UseRangeFloat, Melee, LineOfSight, PlusPercent, Drop};
enum class UI_Recharge{NA, FloatPtsPerSec, FloatSec, FloatSecOrHQ, HQ};
enum class UI_Rounds{NA, UseInt, Variable};
class taMinicon : public taDatabaseInstance{
    /*From METAGAME.TMD*/
public:
    ActivationType activationType;
    float chargeDrainRate_Commander;
    float chargeDrainRate_Veteran;
    float chargeDrainTime;
    float coolDownTime;
    float coolDownTimeDepleted;
    CrosshairIndex crosshairIndex;
    QString description;
    bool equipInHQ;
    Icon icon;
    MiniconNames minicon;
    float minimumChargeToUse;
    float minimumChargeToUsePerShot;
    QString name;
    int nodeToKeepIndex;
    int paletteIndex;
    int powerCost;
    float rechargeTime;
    RecoilType recoilType;
    RestrictToButton restrictToButton;
    int segments;
    float sidekickChargeDrainTime;
    float sidekickCoolDownTime;
    float sidekickCoolDownTimeDepleted;
    float sidekickMinimumChargeToUse;
    float sidekickRechargeTime;
    int sidekickSegments;
    Slot slot;
    Team team;
    QString toneLibrary;
    UI_DamagePri uiDamagePri;
    float UI_DamagePriFloat;
    UI_DamageSec uiDamageSec;
    float UI_DamageSecFloat;
    UI_Defence uiDefence;
    UI_Duration uiDuration;
    float UI_DurationFloat;
    UI_Range uiRange;
    float UI_RangeDist;
    UI_Recharge UI_Recharge;
    UI_Rounds uiRounds;
    int UI_RoundsInt;

    taMinicon();
    taMinicon(dictItem copyItem);
};

class taMiniconArmor : public taMinicon {
public:
    float AbsorbPercentage;
    float MaxAbsorbPerHit;
    float MaxDamageToAbsorb;
};

class ExodusOption{
public:
    /*From */
    QString name;
    QString author;
    QString description;
    bool enabled;
};

class exCustomLocation : public ExodusOption {
public:
    /*Inheriting ExodusOption for now - I don't know if that should be the case long-term*/
    int locationCount;
    std::vector<exPickupLocation> locationList;
};

class ExodusData{
  /*Container for all Exodus-specific classes from databases*/
 public:
    std::shared_ptr<DatabaseFile> dataFile;

    std::vector<exPickup> pickupList;
    std::vector<exMinicon> miniconList;
    std::vector<exPickup> dataconList;
    std::vector<exCustomLocation> customLocationList;
    std::vector<exEpisode> loadedLevels;
};

class GameData{
  /*Container for all information used in game database files*/
public:
    std::shared_ptr<DatabaseFile> metagameFile;

    std::vector<taPickupPlaced> pickupList;
    std::vector<dictItem> autobotList;
    std::vector<taEpisode> levelList;
    std::vector<taMinicon> miniconList;
};

class DataHandler{
public:
    inline const static QList<int> weaponList {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 27, 30, 36, 41, 46, 51};
    inline const static QList<int> bunkerList {137,140,141,142,143,144,145};

    ProgWindow* parent;

    //std::vector<dictItem> itemList;

    ExodusData exodusData;
    GameData gameData;

    template <class instanceType>
    std::vector<instanceType> convertInstances(const std::vector<dictItem> itemList){
        std::vector<instanceType> convertedList;
        for(int i = 0; i < itemList.size(); i++){
            instanceType nextInstance = instanceType(itemList[i]);
            convertedList.push_back(nextInstance);
        }
        return convertedList;
    };

    /*Used for converting processed location data into a database file. Should rarely need to be used.*/
    void createExodusLocationDatabase();


    dictItem createGamePickupPlaced(exPickupLocation location);
    dictItem createMetagameMinicon(taMinicon minicon);
    void loadMinicons();
    void loadDatacons();
    void loadLevels();
    void loadAutobots();
    bool miniconLoaded(int checkID);
    bool dataconLoaded(int checkID);
    void loadCustomLocations();
    //void addCustomLocation(int locationID, int level, QVector3D location);

    //compare taMinicon MiniconList to exMinicon enumID
    exMinicon* getExodusMinicon(int searchID);
    taMinicon* getGameMinicon(int searchID);

    //compare taMinicon name to exMinicon pickupToSpawn
    exMinicon* getExodusMinicon(QString searchName);
    taMinicon* getGameMinicon(QString searchName);

    exEpisode* getExodusEpisode(QString levelToGet);
    taEpisode* getGameEpisode(QString levelToGet);

    bool duplicateLocation(exPickupLocation testLocation);

    void resetMinicons();
    void resetDatacons();
    void resetLevels();

    /*std::vector<Pickup> convertPickups(std::vector<dictItem> itemList);
    std::vector<Minicon> convertMinicons(std::vector<dictItem> itemList);
    std::vector<Warpgate> convertWarpgates(std::vector<dictItem> itemList);*/
};

#endif // DATAHANDLER_H
