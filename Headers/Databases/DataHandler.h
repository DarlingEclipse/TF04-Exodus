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
enum PickupToSpawn{Health_Small, Health_Large,  SidekickJuice, Data, AssaultBlaster, HeavyAssaultBlaster, SuperHeavyAssaultBlaster, SuperHeavyCannon
                           , HeavyElectricArcGun, SuperHeavyElectricArcGun, ImpactGun, HeavyRibbonBeam, SuperHeavyRibbonBeam, UltimatePrimary, ClusterRocket
                           , HeavyClusterRocket, DarkEnergonBlaster, DecoyLauncher, EMPBlast, FlakBurst, GrenadeLauncher, HomingMissile, LimpetMine, DropMine
                           , SniperRifle, VortexCannon, UltimateSecondary, ElectroField,Glide, UltimateGlide, AreaEffectScrambler, Armor, HeavyArmor
                           , SuperHeavyArmor, Dash, EmergencyWarpGate, EnergonMeleeWeapon, BallisticShield, EnergonVision, EnergonShield, EnhancedDash
                           , MeleeDamageEnhance, MiniconSensor, WeaponDistanceEnhance, ReflectionShield, Stealth, TractorBeam, UltimateShield, AutoRepair
                           , Revive, BoostJump, HeavyHomingMissile};
enum ProductArt{None,CharacterEnergon,RendersAutobots2,CardArtwork,DecepticloneSubmission,LeClezio1,RendersDecepticons2,ConceptArtDecepticlone,LeClezio2
                  , TVSeriesThemeMusic,LevelStoryboardsAmazon,ArtPostcards,InstructionSheetOptimus,ConceptArtAutobots,MovieStills6,MovieStills7,RendersDecepticon
                  , ThemeRegurgitator,MiniComic1,MiniComic2,MiniComic3,MiniComic4,ProductionArtAmazon,ProductionArtAntarctica,ProductionArtDeepAmazon
                  , ProductionArtMidAtlantic,ProductionArtAlaska,ProductionArtStarship,ProdcutionArtPacificIsland,ProductionArtAutobotHQ,RendersAutobots
                  , RendersDecepticlone,RendersMiniCons,ToyProductionHotShot,ToyProductionOptimus,ToyProductionRedAlert,ToyProductionMinicons
                  , ToyProductionCyclonus,ToyProductionStarscream,ToyProductionTidalWave,ToyProductionMegatron,RendersAutobots3,CGProductionSequence2
                  , CGProductionSequence,InstructionSheetMegatron,InstructionSheetHotShot,RendersDecepticlone2,RendersDecepticlone3,RendersDecepticlone4
                  , TVSpot1,TVSpot2,TVSpot3,TVSpot4,TVSpot5,InstructionSheetRedAlert,MovieStills1,MovieStills2,MovieStills3,MovieStills4,MovieStills5
                  , Dropbox_Wishbone,ThemeDropbox,LaunchPhotos,ThemeOrchestral};
class taPickup : public taDatabaseInstance{
public:
    int datacon_HealthPickupCount_Large; //0
    int datacon_HealthPickupCount_Small; //2
    PickupToSpawn pickupToSpawn;
    ProductArt productArt;

    bool placed = false;

    taPickup(dictItem fromItem);

    const bool isMinicon(){
        if(pickupToSpawn > 3){
            return true;
        }
        return false;
    };


    bool operator < (const taPickup& compPickup) const
    {
        return (pickupToSpawn < compPickup.pickupToSpawn);
    }

    bool operator == (const taPickup& compPickup) const
    {
        if(productArt != 0){
            return (productArt == compPickup.productArt);
        } else {
            return (pickupToSpawn == compPickup.pickupToSpawn);
        }
    }
};

class taLocation : public taDatabaseInstance{
public:
    GameModes gameModes;
    bool generate; //true
    GenerationDifficulty generationDifficulty;
    GenerationGameBuildType generationGameBuildType;
    GenerationSpawnType generationSpawnType;
    QQuaternion orientation; //1 0 0 0
    QVector3D position; //0 0 0
    QString spawnEvent; //""
    QString synchronizeEvent; //""

    taLocation(dictItem fromItem);

};

enum BossType{BossTypeInvalid = -1
                , BossTypeNone
                , BossTypeStarscream
                , BossTypeCyclonus
                , BossTypeMegatron
                , BossTypeTidalWave
                , BossTypeUnicron};
enum Episode{EpisodeInvalid = -1
               , EpisodeAmazon_1
               , EpisodeAntarctica
               , EpisodeAmazon_2
               , EpisodeAircraftCarrier
               , EpisodeGreenland
               , EpisodeSpaceship
               , EpisodeEasterIsland
               , EpisodeCybertron};
class taEpisode : public taDatabaseInstance{
public:
    /*all members from metagame.tmd*/
    QString alternativeDirectoryName;
    int bossType;
    int dataconCount;
    QString directoryName;
    int episode;
    bool hasAlternativeDirectory;
    int miniconCount;
    int miniconUnlockCountExtreme;
    int miniconUnlockCountHard;
    int miniconUnlockCountNormal;
    QString name;
    std::vector<int> WarpGateMusicTrack;
    int warpgates;

    /*Populated at runtime*/
    int episodeOrder;

    taEpisode(dictItem copyItem);
    void updateDirectories();
};

/*class exPickup {
public:
    int pickupID; //connects to taPickupPlaced
    int dataID;
    QString pickupToSpawn; //for debugging and spoiler files

    //Set and used during randomizing
    bool placed;

    exPickup(taPickupPlaced copyItem);
    exPickup(dictItem copyItem);
    exPickup();


};*/

class exMinicon{
    /*Contains data useful for Exodus that is not defined in the METAGAME files. */
public:
    /*From Exodus database files*/
    QString name;
    int creatureID; //for CREATURE files
    int metagameID; //for METAGAME files
    int rating;
    bool isWeapon;
    bool isExplosive;

    /*Set and used at runtime*/
    bool hasVanillaPlacement; //should be redundant - if it doesn't have a creatureID, it doesn't have a placement
    bool isSlipstream;
    bool isHighjump;
    bool placed;


    void setCreature(taPickup copyItem);
    exMinicon(dictItem copyItem);
    exMinicon();
};

class exTrick{
public:
    QString name;
    QString description;
    int difficulty;
    bool enabled;
    QString guideLink;
    std::vector<int> requiredMinicons;
    bool needAllRequirements; //defines if the entire list of requirements is needed or just one

    exTrick(dictItem fromItem);

    bool operator < (const exTrick& compTrick) const
    {
        return (difficulty < compTrick.difficulty);
    }
};

class exEpisode;

class exPickupLocation{
public:
    int world;
    bool usesAlternate;
    QString levelName;
    int uniqueID; //randomizer ID
    int originalTeleportNode; //Used for bunkers

    /*The difficulties are determined by how hard it is to get to that location from the start of the level*/
    bool requiresSlipstream;
    bool requiresHighjump;
    bool requiresExplosive;

    std::vector<exTrick*> availableTricks; //currently handled as a single link in the file
    QString locationName;
    QString spawnEvent;
    int inputDatabaseInstance;
    int outputDatabaseInstance;
    bool spoiled;
    bool isBunker;
    QVector3D position;
    /*Generation difficulty: If the LocationChallenge is outside this range, then the location will not be added to the available
    location pool*/
    int minimumGenerationDifficulty;
    int maximumGenerationDifficulty;
    taPickup* pickup;

    std::vector<int> linkedLocationIDs; //if a minicon is placed at one of these, it's placed at both of them
    //so far only used for starship (vanilla Aftershock) and mid-atlantic, since it has separate database files

    exPickupLocation();
    exPickupLocation(dictItem copyItem);
    exPickupLocation(taLocation fromItem);
    taPickup* assignedPickup();
    void assignPickup(taPickup* pickupToPlace);
    void generateAttributes(); //for custom locations
    //implementing the < operator so we can simply sort the location vector
    //using std::sort instead of making a whole function for it
    bool operator < (const exPickupLocation& compLocation) const
    {
        return (world < compLocation.world);
    }
};

class exEpisode{
public:
    //from database file
    QString logName;
    int requirements;
    bool usesAlternate = false;
    /*whether the level needs highjump or slipstream to beat normally
     * Handling requirements this way because integerarrays won't work for a while
     * 0 = no requirements
     * 1 = slipstream
     * 2 = highjump
     * 3 = both*/
    std::vector<exTrick*> availableTricks; //tricks that can be used to complete the level without its typical requirements
    bool placeable; //only used for post-boss Mid-Atlantic
    int world; //for reconstructing the original CREATURE folder's location

    //populated at runtime
    int currentEpisode; //for randomizing the level order
    std::vector<exPickupLocation> spawnLocations;
    int assignedMinicons; //max 13 of any specific minicon without mods
    int assignedDatacons; //max 13 without mods
    std::shared_ptr<DatabaseFile> levelFile;

    exEpisode(dictItem copyItem);
    exEpisode();

    bool operator < (const exEpisode& compEpisode) const
    {
        return (currentEpisode < compEpisode.currentEpisode);
    }
};

enum class ActivationType{HoldDown, AlwaysOn, PressInstant, ReleaseInstant, PressAndRelease, ChargeRelease, ChargeReleasePartial, Toggle, ToggleDrain, HoldDownDrain};
enum class CrosshairIndex{None, AssaultBlaster, Lockon, Slingshot, Claymore, Tractor, Firefight, Hailstorm, Landslide, Smackdown, Watchdog, Lookout, Sparkjump, Skirmish, Airburst, Sandstorm, Twister, Discord, Corona, Aftershock, Aurora, Failsafe, Jumpstart, Endgame, UltimatePrimary, UltimateSecondary};
enum class Icon{Armor, Beam, Blaster, Exotic, Lobbed, Melee, Missile, Movement, Repair, Shield, Stealth, Vision, None};
enum class MiniconEffects{None, AssaultBlaster, HeavyAssaultBlaster, SuperHeavyAssaultBlaster, SuperHeavyCannon, HeavyElectricArcGun, SuperHeavyElectricArcGun
                          , ImpactGun, HeavyRibbonBeam, SuperHeavyRibbonBeam, UltimatePrimary, ClusterRocket, HeavyClusterRocket, DarkEnergonBlaster
                          , DecoyLauncher, EMPBlast, FlakBurst, GrenadeLauncher, HomingMissile, LimpetMine, DropMine, SniperRifle, VortexCannon, UltimateSecondary
                          , ElectroField, Glide, UltimateGlide, AreaEffectScrambler, Armor, HeavyArmor, SuperHeavyArmor, Dash, EmergencyWarpGate, EnergonMeleeWeapon
                          , EnergonScreen, EnergonVision, EnergonShield, EnhancedDash, MeleeDamageEnhance, MiniconSensor, WeaponDistanceEnhance, ReflectionShield
                          , Stealth, TractorBeam, UltimateShield, AutoRepair, Revive, BoostJump, HeavyHomingMissile};
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
    MiniconEffects minicon;
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

    std::vector<exMinicon> miniconList;
    std::vector<exCustomLocation> customLocationList;
    std::vector<exEpisode> loadedLevels;
    std::vector<exTrick> trickList;
};

class GameData{
  /*Container for all information used in game database files*/
public:
    std::shared_ptr<DatabaseFile> metagameFile;

    std::vector<taPickup> pickupList;
    std::vector<taPickup> dataconList;
    std::vector<dictItem> autobotList;
    std::vector<taEpisode> levelList;
    std::vector<taMinicon> miniconList;
};

class DataHandler{
public:
    inline const static QList<int> weaponList {4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 18, 19, 20, 21, 22, 23, 24, 25, 27, 30, 36, 41, 46, 51};
    inline const static QList<int> bunkerList {137,140,141,142,143,144,145};

    exWindowBase* m_UI;
    zlManager* m_zlManager;
    exDebugger* m_Debug;

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

    DataHandler(exWindowBase *passUI, zlManager *fileManager);

    /*Used for converting processed location data into a database file. Should rarely need to be used.*/
    void createExodusLocationDatabase();


    dictItem createGamePickupPlaced(const exPickupLocation* location);
    dictItem createGameEpisode(const taEpisode* episode);
    dictItem createMetagameMinicon(taMinicon minicon);
    void LoadAll();
    void loadMinicons();
    void loadDatacons();
    void loadLevels();
    void loadAutobots();
    void loadTricks();
    bool miniconLoaded(int checkID);
    bool dataconLoaded(int checkID);
    void loadCustomLocations();
    void addCustomLocations();
    void debugLocations();
    //void addCustomLocation(int locationID, int level, QVector3D location);

    //compare taMinicon MiniconList to exMinicon enumID
    exMinicon* getExodusMinicon(int searchID);
    taMinicon* getGameMinicon(int searchID);

    taPickup* getPickup(int searchID);

    //compare taMinicon name to exMinicon pickupToSpawn
    exMinicon* getExodusMinicon(QString searchName);
    taMinicon* getGameMinicon(QString searchName);

    exEpisode* getExodusEpisode(int episodeToGet);
    taEpisode* getGameEpisode(int episodeToGet);

    int highestAvailableLevel(int checkRequirements);

    bool duplicateLocation(exPickupLocation testLocation);
    bool duplicatePickup(taPickup testPickup);

    void resetMinicons();
    void resetDatacons();
    void resetLevels();

    /*std::vector<Pickup> convertPickups(std::vector<dictItem> itemList);
    std::vector<Minicon> convertMinicons(std::vector<dictItem> itemList);
    std::vector<Warpgate> convertWarpgates(std::vector<dictItem> itemList);*/
};

#endif // DATAHANDLER_H
