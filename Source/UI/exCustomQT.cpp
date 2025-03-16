#include <QPushButton>
#include <QLineEdit>
#include <QBoxLayout>

#include "Headers/Main/CustomQT.h"

void CustomPopup::addBlankItem(){
    QListWidgetItem* blankValue = new QListWidgetItem("", listOption);
    blankValue->setFlags(blankValue->flags() | Qt::ItemIsEditable);
    listOption->addItem(blankValue);
}

SettingSlider::SettingSlider(QString labelText, QWidget* parent, int levels){
    this->setParent(parent);
    slider  = new QSlider(Qt::Horizontal, this);
    label = new QLabel(labelText, this);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(1);
    slider->setMaximum(levels);
    label->setGeometry(QRect(QPoint(0,0), QSize(150,30)));
    slider->setGeometry(QRect(QPoint(0,30), QSize(150,30)));
}

exLineButton::exLineButton(QString buttonLabel, QWidget* parent){
    m_button = new QPushButton(buttonLabel, this);
    m_lineedit = new QLineEdit(this);
    m_button->setGeometry(QRect(QPoint(0,0), QSize(150, 30)));
    m_lineedit->setGeometry(QRect(QPoint(150, 0), QSize(150, 30)));
}

exComboButton::exComboButton(QString buttonLabel, QStringList comboList, QWidget* parent){
    m_button = new QPushButton(buttonLabel, this);
    m_combobox = new QComboBox(this);
    for(int i = 0; i < comboList.size(); i++){
        m_combobox->addItem(comboList[i]);
    }
    m_button->setGeometry(QRect(QPoint(0,0), QSize(150, 30)));
    m_combobox->setGeometry(QRect(QPoint(150, 0), QSize(150, 30)));
}

exCheckList::exCheckList(QStringList labelList, QWidget* parent){
    setParent(parent);
    m_checklist = new QVBoxLayout(this);
    for(int i = 0; i < labelList.size(); i++){
        QCheckBox* currentCheck = new QCheckBox(labelList[i]);
        m_checklist->addWidget(currentCheck);
    }
}
