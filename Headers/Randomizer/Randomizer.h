#ifndef RANDOMIZER_H
#define RANDOMIZER_H

#include <QRandomGenerator>
#include "Headers/Databases/Database.h"
#include "Headers/ISOManager/ModHandler.h"

class ProgWindow;
class DatabaseFile;

/*Randomizer will need, at minimum, the file manager and data handler*/
class Randomizer{
public:
    Randomizer(zlManager *fileManager, DataHandler *dataSystem);

    exWindow *m_UI;
    zlManager *m_zlManager;
    DataHandler *m_DataHandler;
    exDebugger *m_Debug;
    //std::vector<std::shared_ptr<DatabaseFile>> databaseLevels;
    //std::vector<Level> levelList;
    //std::vector<FileReplacement> replacementList;
    QRandomGenerator placemaster;
    QLineEdit* editSeed;
    QLineEdit* editSettings;
    quint32 seed;
    bool enableCustomLocations;
    struct randomizerSettings {
        bool generateDatacons = false;
        bool autoBuild = false;
        /*Change overall to PorgressionBalancing and LocationChallenge
        ProgressionBalancing will be used for the location difficulty calculation
        LocationChallenge will be used to determine which locations are available in the pool*/
        //int overallDifficulty = 0;
        int progressionBalancing = 0;
        int locationChallenge = 0;
        bool randomizeTeams = false;
        bool balancedTeams = false;
        bool randomizePower = false;
        bool balancedPower = false;
        bool randomizeAutobotStats = false;
        bool randomizeLevelOrder = false;
    } randSettings;

    long enabledTricks = 0; //not sure if we'll actually use this

    //https://stackoverflow.com/questions/1604588/how-do-you-remove-elements-from-a-stdvector-while-iterating
    //use the above for turning pickuplist into the minicon and datacon lists

    //there are duplicate minicons on the miniconlist because of minicons with different placements in different difficulties.
    //see if we can clear those out.

    //also set a < operator for minicon based on their enumID
    //will probably want to subtract 3 when reading and add 3 when writing these

    std::vector<exPickupLocation*> availableLocations;
    std::vector<exPickupLocation*> placedLocations;

    void reset();

    void testAllPlacements();
    void randomizeTeamColors();
    void randomizePowers();
    void randomizeAutobotStats();
    void randomizeLevels();

    void randomize();
    void randomFileReplacements();
    void removeLocation(const exPickupLocation* locationToRemove);
    void placeAll();
    void placeMinicon(int miniconToPlace, int placementID);
    void placeDatacon(int dataconToPlace, int placementID);
    void placeDatacon(taPickup* dataconToPlace, exPickupLocation* location);
    void placeSlipstreamRequirement(int miniconID, int placementID);
    void placeRangefinder();
    void placeShepherd();
    void placeSlipstream();
    void placeHighjump();
    void placeStarterWeapon();
    void manualSettings();
    void exportSettings();
    void fixBunkerLinks();
    void setSeed(QString value);

    std::vector<int> createRandomList(int listSize);

    float randomFloat(float minimum, float maximum);

    int writeSpoilers();
    void spoilMinicon(const exPickupLocation* placement, QTextStream& stream);
    void spoilMinicon(int miniconID, QTextStream& stream);

    int editDatabases();

};

#endif // RANDOMIZER_H
