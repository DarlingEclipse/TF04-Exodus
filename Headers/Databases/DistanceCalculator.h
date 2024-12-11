#ifndef DISTANCECALCULATOR_H
#define DISTANCECALCULATOR_H

#include <QLineEdit>
#include <QString>

#include "Headers/Databases/Database.h"
#include "Headers/Databases/DataHandler.h"

class ProgWindow;

class WarpgateFile : public DatabaseFile{
    std::vector<exWarpgate> warpgateList;

    void isolateWarpgates();
};

class DistanceCalculator{
public:
    DistanceCalculator(ProgWindow *parentPass);
    std::vector<exWarpgate> warpgateList;
    ProgWindow *parent;

    QLineEdit *inputXValue;
    QLineEdit *inputYValue;
    QLineEdit *inputZValue;

    int currentLevel;

    void calculateWarpgateDistance();
    void userSelectLevel(int selectedLevel);

};

#endif // DISTANCECALCULATOR_H
