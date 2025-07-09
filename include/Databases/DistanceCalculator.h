#ifndef DISTANCECALCULATOR_H
#define DISTANCECALCULATOR_H

#include <QLineEdit>
#include <QString>
#include <QLabel>

#include "Databases/DataHandler.h"

class WarpgateFile : public DatabaseFile{
    std::vector<exWarpgate> warpgateList;

    void isolateWarpgates();
};

class DistanceCalculator{
public:
    DistanceCalculator(exWindowBase *passUI, zlManager *fileManager);
    std::vector<exWarpgate> warpgateList;
    exWindowBase *m_UI;
    exDebugger *m_Debug;
    zlManager *m_zlManager;

    /*None of these should be here*/
    QLineEdit *inputXValue;
    QLineEdit *inputYValue;
    QLineEdit *inputZValue;
    QLabel *m_closestWarpgate;

    int currentLevel;

    void calculateWarpgateDistance();
    void userSelectLevel(int selectedLevel);

};

#endif // DISTANCECALCULATOR_H
