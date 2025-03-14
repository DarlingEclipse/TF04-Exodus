#ifndef CUSTOMQT_H
#define CUSTOMQT_H

#include <qcombobox>
#include <qlistwidget>
#include <qtextedit>
#include <QLabel>

#include <QDialog>
#include <QCheckBox>

#include <QSlider>
#include <QGroupBox>

class exWindowBase;

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

class exCustomGroup : public QGroupBox{
public:
    exWindowBase* m_UI; //this might not be needed, since we're inheriting QGroupBox
    //the parent UI can be set in the constructor, then the rest can be added after
    int m_groupType; //enum of the custom types
};

class exLineButton : public exCustomGroup{
public:
    QPushButton* m_button;
    QLineEdit* m_lineedit;
    exLineButton(QString buttonLabel, QWidget* parent = nullptr);
};

class exComboButton : public exCustomGroup{
public:
    QPushButton* m_button;
    QComboBox* m_combobox;
    exComboButton(QString buttonLabel, QStringList comboList, QWidget* parent = nullptr);
};

class exCheckList : public exCustomGroup{
public:
    std::vector<QCheckBox*> m_checklist;
    exCheckList(std::vector<QString> labelList, QWidget* parent = nullptr);
};

class SettingSlider : public exCustomGroup {
public:
    QSlider* slider;
    QLabel* label;
    SettingSlider(QString labelText, QWidget* parent = nullptr, int levels = 5);
};



#endif // CUSTOMQT_H
