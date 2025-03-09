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
