#include <QPushButton>
#include "Headers/Databases/DistanceCalculator.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/FileManagement/Zebrafish.h"

void DistanceCalculator::userSelectLevel(int selectedLevel){
    //called when user makes a selection on the level list dropdown
    currentLevel = selectedLevel;
    std::vector<dictItem> warpgateListRaw = m_zlManager->m_databaseList[currentLevel]->sendInstances("Warpgate");
    //warpgateList = parent->databaseList[currentLevel]->sendWarpgates();
    //then currentlevel is used below to calculate the closest warpgate
}

DistanceCalculator::DistanceCalculator(exWindowBase *passUI, zlManager *fileManager){
    m_UI = passUI;
    m_Debug = &exDebugger::GetInstance();
    m_zlManager = fileManager;
    if(m_zlManager->LoadDatabases() != 0){
        m_Debug->Log("Could not load databases. Warpgate calculator was not loaded.");
        return;
    }
    m_UI->ClearWindow();
    QPushButton *ButtonCalculate = new QPushButton("Calculate", m_UI->m_centralContainer);
    ButtonCalculate->setGeometry(QRect(QPoint(50,320), QSize(150,30)));
    QAbstractButton::connect(ButtonCalculate, &QPushButton::released, m_UI, [this] {calculateWarpgateDistance();});
    m_UI->m_currentWidgets.push_back(ButtonCalculate);
    ButtonCalculate->show();

    inputXValue = new QLineEdit("X Value", m_UI->m_centralContainer);
    inputXValue->setGeometry(QRect(QPoint(200,320), QSize(150,30)));
    m_UI->m_currentWidgets.push_back(inputXValue);
    inputXValue->show();

    inputYValue = new QLineEdit("Y Value", m_UI->m_centralContainer);
    inputYValue->setGeometry(QRect(QPoint(350,320), QSize(150,30)));
    m_UI->m_currentWidgets.push_back(inputYValue);
    inputYValue->show();

    inputZValue = new QLineEdit("Z Value", m_UI->m_centralContainer);
    inputZValue->setGeometry(QRect(QPoint(500,320), QSize(150,30)));
    m_UI->m_currentWidgets.push_back(inputZValue);
    inputZValue->show();

    QComboBox* ListLevels = new QComboBox(m_UI->m_centralContainer);
    ListLevels -> setGeometry(QRect(QPoint(250,150), QSize(200,30)));

    for(int i=0; i<m_zlManager->m_databaseList.size(); ++i){
        ListLevels->insertItem(i, m_zlManager->m_databaseList[i]->fileName);
    }

    QComboBox::connect(ListLevels, &QComboBox::currentIndexChanged, m_UI, [ListLevels, this] {userSelectLevel(ListLevels->currentIndex());});
    ListLevels->show();
    m_UI->m_currentWidgets.push_back(ListLevels);
    ListLevels->setCurrentIndex(0);
}

void DistanceCalculator::calculateWarpgateDistance(){
    exWarpgate closestGate = exWarpgate();
    float totalDifference = 0;
    float lowestDistance = 99999;
    float xDistance = inputXValue->text().toFloat();
    float yDistance = inputYValue->text().toFloat();
    float zDistance = inputZValue->text().toFloat();

    qDebug() << Q_FUNC_INFO << "Finding closes warpgate to point: x" << xDistance << "y" << yDistance << "z" << zDistance;
    for (int i = 0; i<warpgateList.size(); i++) {
        totalDifference = std::pow(std::pow(xDistance - warpgateList[i].x_position, 2)
                + std::pow(yDistance - warpgateList[i].y_position, 2)
                + std::pow(zDistance - warpgateList[i].z_position, 2)
                    ,0.5);
        if (totalDifference < lowestDistance) {
            lowestDistance = totalDifference;
            closestGate = warpgateList[i];
        }
        qDebug() << Q_FUNC_INFO << "Distance to warpgate" << warpgateList[i].name << ":" << totalDifference;
    }
    closestGate.name += " x" + QString::number(closestGate.x_position) + " y" + QString::number(closestGate.y_position) + " z" + QString::number(closestGate.z_position);

    qDebug() << Q_FUNC_INFO << "Closest warpgate: " << closestGate.name;

    if(m_closestWarpgate == nullptr){
        m_closestWarpgate = new QLabel(closestGate.name, m_UI->m_centralContainer);
        m_closestWarpgate->setGeometry(QRect(QPoint(650,320), QSize(450,30)));
        m_closestWarpgate->setStyleSheet("QLabel { background-color: rgb(105,140,187) }");
        m_closestWarpgate->show();
    } else {
        m_closestWarpgate->setText(closestGate.name);
    }
}

std::vector<exWarpgate*> exWarpgate::createAmazonWarpgates(){
    std::vector<exWarpgate*> warpgates;

    exWarpgate* warpgate1 = new exWarpgate;
    warpgate1->name = "Amazon Basin";
    warpgate1->x_position = 962.6;
    warpgate1->y_position = -1749.8;
    warpgate1->z_position = -3.5;
    warpgates.push_back(warpgate1);

    exWarpgate* warpgate2 = new exWarpgate;
    warpgate2->name = "Stone Bridge";
    warpgate2->x_position = 149.6;
    warpgate2->y_position = -1624.4;
    warpgate2->z_position = -5.4;
    warpgates.push_back(warpgate2);

    exWarpgate* warpgate3 = new exWarpgate;
    warpgate3->name = "Ruined Temple";
    warpgate3->x_position = -529.1;
    warpgate3->y_position = -1593.4;
    warpgate3->z_position = -65.8;
    warpgates.push_back(warpgate3);

    exWarpgate* warpgate4 = new exWarpgate;
    warpgate4->name = "Waterfall";
    warpgate4->x_position = -95.6;
    warpgate4->y_position = -751.8;
    warpgate4->z_position = 104.9;
    warpgates.push_back(warpgate4);

    exWarpgate* warpgate5 = new exWarpgate;
    warpgate5->name = "Deep Ravine";
    warpgate5->x_position = 748.2;
    warpgate5->y_position = -112.7;
    warpgate5->z_position = 133.9;
    warpgates.push_back(warpgate5);

    exWarpgate* warpgate6 = new exWarpgate;
    warpgate6->name = "Mountain Ruins";
    warpgate6->x_position = 1034.2;
    warpgate6->y_position = 287.6;
    warpgate6->z_position = 433.1;
    warpgates.push_back(warpgate6);

    return warpgates;
}
