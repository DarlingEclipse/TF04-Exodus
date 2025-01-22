#ifndef CUSTOMQT_H
#define CUSTOMQT_H

#include <qcombobox>
#include <qlistwidget>
#include <qtextedit>
#include <QLabel>
#include <QDialog>
#include <QCheckBox>

class CustomLabel : public QLabel {
public:
    static QLabel* addLabel(QRect relativeObject, QString text, QWidget *parent, int direction = 0){
        QLabel *objectLabel = new QLabel(text, parent);
        switch (direction){
            case 0: //above
                objectLabel->setGeometry(QRect(QPoint(relativeObject.left(), relativeObject.top()-30)
                                        ,QSize(150, 30)));
                break;
            case 1: //right
                objectLabel->setGeometry(QRect(QPoint(relativeObject.right(), relativeObject.top())
                                        ,QSize(150, 30)));
                break;
            case 2: //below
                objectLabel->setGeometry(QRect(QPoint(relativeObject.left(), relativeObject.bottom()+30)
                                        ,QSize(150, 30)));
                break;
            case 3: //left
            //this one should probably be modified to account for text length
                objectLabel->setGeometry(QRect(QPoint(relativeObject.left()-150, relativeObject.top())
                                        ,QSize(150, 30)));
                break;
            default:
                qDebug() << Q_FUNC_INFO << "Invalid direction provided for custom label.";
        }

        objectLabel->setStyleSheet("QLabel { background-color: rgb(105,140,187) }");
        objectLabel->show();
        return objectLabel;
    };
};

class CustomPopup : public QDialog{
public:
    QCheckBox* checkOption;
    QComboBox* comboOption;
    QLineEdit* lineOption;
    QTextEdit* multiOption;
    QListWidget* listOption;
    std::vector<QLineEdit*> boxList;
    QPushButton* buttonOption;

    void addBlankItem();
};

#endif // CUSTOMQT_H
