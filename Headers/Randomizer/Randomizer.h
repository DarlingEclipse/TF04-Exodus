#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <QRandomGenerator>
#include "Headers/Databases/Database.h"
#include "Headers/ISOManager/ModHandler.h"

class ProgWindow;
class DatabaseFile;

class Randomizer{
public:
    Randomizer(ProgWindow *parentPass);

    ProgWindow* parent;
    //std::vector<std::shared_ptr<DatabaseFile>> databaseLevels;
    //std::vector<Level> levelList;
    //std::vector<FileReplacement> replacementList;
    QRandomGenerator placemaster;
    QLineEdit* editSeed;
    QLineEdit* editSettings;
    QString settingsValue;
    quint32 seed;
    bool enableCustomLocations;
    struct randomizerSettings {
        bool generateDatacons = false;
        bool autoBuild = false;
        int overallDifficulty = 0;
        int slipstreamDifficulty = 0;
        int highjumpDifficulty = 0;
        bool randomizeTeams = false;
        bool balancedTeams = false;
        bool randomizePower = false;
        bool balancedPower = false;
        bool randomizeAutobotStats = false;
    } randSettings;

    //https://stackoverflow.com/questions/1604588/how-do-you-remove-elements-from-a-stdvector-while-iterating
    //use the above for turning pickuplist into the minicon and datacon lists

    //there are duplicate minicons on the miniconlist because of minicons with different placements in different difficulties.
    //see if we can clear those out.

    //also set a < operator for minicon based on their enumID
    //will probably want to subtract 3 when reading and add 3 when writing these

    std::vector<exPickupLocation> availableLocations;
    std::vector<exPickupLocation> placedLocations;

    void reset();

    void testAllPlacements();
    void randomizeTeamColors();
    void randomizePowers();
    void randomizeAutobotStats();

    void randomize();
    void randomFileReplacements();
    void removeLocation(exPickupLocation locationToRemove);
    void placeAll();
    void placeMinicon(int miniconToPlace, int placementID);
    void placeDatacon(int dataconToPlace, int placementID);
    void placeDatacon(exPickup* dataconToPlace, exPickupLocation location);
    void setOverallDifficulty(int difficulty);
    void setSlipstreamDifficulty(int difficulty);
    void setHighjumpDifficulty(int difficulty);
    void placeSlipstreamRequirement(int miniconID, int placementID);
    void placeRangefinder();
    void placeShepherd();
    void placeSlipstream();
    void placeHighjump();
    void placeStarterWeapon();
    void manualSettings();
    void exportSettings();
    void fixBunkerLinks(int level);
    void setSeed(QString value);

    float randomFloat(float minimum, float maximum);

    int writeSpoilers();
    void spoilMinicon(exPickupLocation placement, QTextStream& stream);
    void spoilMinicon(int miniconID, QTextStream& stream);

    int editDatabases();

};

#endif // RANDOMIZER_H
