
#include "Headers/Main/mainwindow.h"

/*Reads through a dictionary file to populate data. This data can be edited
and re-exported or simply obeserved.

Binary files: Using the section lenghts provided with each header, data is
read in chunks.

Text files: Data is read line-by-line, using headers to change into different
reading "modes" that change how lines are handled.

*/

void DictionaryFile::writeText(){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
}

void DictionaryFile::writeBinary(){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
}

int DictionaryFile::readDictionary(){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

int DictionaryFile::readFileDictionary(){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

int DictionaryFile::readInstances(){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

int DictionaryFile::readDictionary(SectionHeader headerData){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

int DictionaryFile::readFileDictionary(SectionHeader headerData){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

int DictionaryFile::readInstances(SectionHeader headerData){
    qDebug() << Q_FUNC_INFO << "base function called. this shouldn't happen.";
    return 1;
}

void DictionaryFile::save(QString toType){
    if(toType == "TMD" || toType == "TDB"){
        writeText();
    } else if (toType == "BMD" || toType == "BDB"){
        writeBinary();
    } else if (toType == "DAE"){
        writeDAE();
    }
}

void DictionaryFile::load(QString fromType){
    int failedRead = 0;
    if(fromType == "BMD" || fromType == "BDB"){
        binary = true;
        failedRead = readBinary();
    } else if (fromType == "TMD" || fromType == "TDB") {
        binary = false;
        failedRead = readText();
    } else {
        qDebug() << Q_FUNC_INFO << "Unexpected file extension received. file extension:" << fromType;
        failedRead = 1;
    }
    if(failedRead){
        parent->messageError("There was an error reading " + fileName);
        return;
    }
    if(fromType == "BDB" || fromType == "TDB"){
        //this will probably cause issues elsewhere, but is needed for automatically loading warpgates.
        QStringList fileDirectories = QFileInfo(this->inputPath).absolutePath().split("/");
        fileName = fileDirectories[fileDirectories.size()-1] + '-' + fileName;
        qDebug() << Q_FUNC_INFO << "file name changed to" << fileName;
    }
}

void DefinitionFile::updateCenter(){
    qDebug() << Q_FUNC_INFO << "updating center view for file" << fileName << "." << fileExtension;
    createDBTree();
}

void DatabaseFile::updateCenter(){
    qDebug() << Q_FUNC_INFO << "updating center view for file" << fileName << "." << fileExtension;
    createDBTree();

    QPushButton* ButtonFilterTree = new QPushButton("Filter Instances", parent->centralContainer);
    ButtonFilterTree->setGeometry(QRect(QPoint(50,370), QSize(150,30)));
    QAbstractButton::connect(ButtonFilterTree, &QPushButton::released, parent, [this]{filterInstances();});
    ButtonFilterTree->show();
    parent->currentModeWidgets.push_back(ButtonFilterTree);

}

void DefinitionFile::createDBTree(){

    dataTree = new QTreeView(parent->centralContainer);
    dataModel = new QStandardItemModel;

    dataTree->setGeometry(QRect(QPoint(250,50), QSize(1000,750)));
    QStandardItem *item = dataModel->invisibleRootItem();
    QList<QStandardItem *> dictRow;
    QStandardItem *classRow;
    QStandardItem *headerItem;
    QList<QStandardItem *> details;
    QStandardItemModel model2;

    //dictRow = {new QStandardItem("Included Files")};
    //item->appendRow(dictRow);

    //append items to matching dictrow
    /*These need to be split up if we want them to be uneditable*/
    QStringList columnHeaders = {"Type","Value","Value List","Comment"};
    dataTree->setModel(dataModel);
    headerItem = new QStandardItem(fileName);
    headerItem->setEditable(false);
    dictRow.push_back(headerItem);
    for(int i = 0; i < columnHeaders.size(); i++){
        headerItem = new QStandardItem(columnHeaders[i]);
        headerItem->setEditable(false);
        dictRow.push_back(headerItem);
    }
    item->appendRow(dictRow);

    //add columns "name", "type", "value", "allowed values"

    for (int i = 0; i < dictionary.size();i++) {
        //qDebug() << Q_FUNC_INFO << "creating class row from" << dictionary[i].name;
        classRow = new QStandardItem(dictionary[i].name);
        classRow->setEditable(false);
        dictRow.first()->appendRow(classRow);
        if(dictionary[i].expanded){
            dataTree->expand(classRow->index());
        }
        for(int j = 0; j<dictionary[i].attributes.size();j++){
            //qDebug() << Q_FUNC_INFO << "creating detail row from" << dictionary[i].attributes[j]->name;
            /*These need to be split up if we want them to be uneditable*/
            details = {new QStandardItem(dictionary[i].attributes[j]->name),new QStandardItem(dictionary[i].attributes[j]->type),
                       new QStandardItem(dictionary[i].attributes[j]->display()), new QStandardItem(dictionary[i].attributes[j]->options().join(", ")), new QStandardItem(dictionary[i].attributes[j]->comment)};
            for(int k = 0; k < details.size(); k++){
                details[k]->setEditable(false);
            }
            classRow->appendRow(details);
        }
    }


    //dataTree->expandAll();
    dataTree->expand(dataTree->model()->index(0, 0));
    QTreeView::connect(dataTree, &QTreeView::expanded, parent, [this](QModelIndex index){setItemExpansion(index, true);});
    QTreeView::connect(dataTree, &QTreeView::collapsed, parent, [this](QModelIndex index){setItemExpansion(index, false);});
    dataTree->show();
    dataTree->resizeColumnToContents(0);
    QAbstractButton::connect(dataTree, &QAbstractItemView::doubleClicked, parent, [this](QModelIndex selected){editRow(selected);});
    parent->currentModeWidgets.push_back(dataTree);
}

int DatabaseFile::addInstance(dictItem itemToAdd){
    /*This was originally written to only work with pickups - that will need to be
    changed since this now handles dictItems of any time.*/
    //modify itemToAdd to put enumID and location where they belong in the attributes
    dictItem tempItem = dictItem();
    int dictIndex = 0;
    int attributeIndex = 0;
    qDebug() << Q_FUNC_INFO << "itemToAdd has type name:" << itemToAdd.name;
    tempItem.name = itemToAdd.name;


    //this needs to limit itself to the attributes present in the target file
    //that means some minicons will need to pull the default value from the definition file
    //qDebug() << Q_FUNC_INFO << "adding item" << itemToAdd.name << "to file" << fileName << "with" << itemToAdd.attributes.size() << "attributes";
    for(int i = 0; i < dictionary.size(); i++){
        if(dictionary[i].name == tempItem.name){
            dictIndex = i;
        }
    }

    //need to find the first available instance ID and apply it to the item being added instead of this
    qDebug() << Q_FUNC_INFO << "count of instances:" << instances.size();
    tempItem.instanceIndex = instances[instances.size()-1].instanceIndex + 1;
//    int previousIndex = 0;
//    for(int i = 0; i < instances.size(); i++){
//        if(instances[i].instanceIndex - previousIndex > 1){
//            tempItem.instanceIndex = previousIndex + 1;
//            break;
//        }
//        previousIndex = instances[i].instanceIndex;
//    }

    qDebug() << Q_FUNC_INFO << "original dictionary item has" << dictionary[dictIndex].attributes.size() << "attributes, new item has" << itemToAdd.attributes.size();
    //dictionary does not have all attributes for some reason.
    for(int i = 0; i < dictionary[dictIndex].attributes.size(); i++){
        attributeIndex = 9999;
        //qDebug() << Q_FUNC_INFO << "checking attribute" << i << dictionary[dictIndex].attributes[i]->name << "in itemtoadd's attribute list of" << itemToAdd.attributes.size() << "items";
        for(int j = 0; j < itemToAdd.attributes.size(); j++){
            if(itemToAdd.attributes[j]->name == dictionary[dictIndex].attributes[i]->name){
                qDebug() << Q_FUNC_INFO << "found a match";
                attributeIndex = j;
            }
        }
        if(attributeIndex == 9999){
            //add attribute with default value
            qDebug() << Q_FUNC_INFO << "creating default value";
            tempItem.attributes.push_back(dictionary[dictIndex].attributes[i]->clone());
        } else {
            tempItem.attributes.push_back(itemToAdd.attributes[attributeIndex]->clone());
        }
    }
    //qDebug() << Q_FUNC_INFO << "adding modified item" << tempItem.name << "to file" << fileName << "with" << tempItem.attributes.size() << "attributes";
    instances.push_back(tempItem);
    //std::sort(instances.begin(), instances.end());
    return tempItem.instanceIndex;
}

void DatabaseFile::filterInstances(){
    QStringList filterOptions;

    for(int i = 0; i < dictionary.size(); i++){
        filterOptions.push_back(dictionary[i].name);
    }

    QString filterChoice = QInputDialog::getItem(parent, parent->tr("Filter Instances"), parent->tr("Select instance type:")
                                                           ,filterOptions, 0, false);
    //clear dataTree, or at least the instances section
    QStandardItem *item = dataModel->invisibleRootItem();
    item->removeRow(item->rowCount()-1);
    QList<QStandardItem *> instanceHeader = {new QStandardItem("Instances"), new QStandardItem("Type"), new QStandardItem("Value"), new QStandardItem("Value List"), new QStandardItem("Default")};
    item->appendRow(instanceHeader);
    QList<QStandardItem *> instanceRow;
    QList<QStandardItem *> details;

    //add columns "name", "type", "value", "allowed values"

    for (int i = 0; i < instances.size();i++) {
        if(instances[i].name == filterChoice){
            instanceRow = {new QStandardItem(instances[i].name), new QStandardItem(QString::number(instances[i].instanceIndex))};
            instanceHeader.first()->appendRow(instanceRow);
            for(int j = 0; j<instances[i].attributes.size();j++){
                details = {new QStandardItem(instances[i].attributes[j]->name),new QStandardItem(instances[i].attributes[j]->type),
                           new QStandardItem(instances[i].attributes[j]->display()), new QStandardItem(instances[i].attributes[j]->options().join(", "))
                           , new QStandardItem(instances[i].attributes[j]->comment)};
                instanceRow.first()->appendRow(details);
            }
        }
    }
    dataTree->expand(dataTree->model()->index(1, 0));
    dataTree->resizeColumnToContents(0);
}

QList<QStandardItem *> DatabaseFile::createFileDictionaryRow(std::shared_ptr<taData> dataRow){
    QList<QStandardItem *> details;
    details = {new QStandardItem(dataRow->name),new QStandardItem(dataRow->type),
               new QStandardItem(dataRow->display()), new QStandardItem(dataRow->options().join(", "))
               , new QStandardItem(dataRow->comment)};
    for(int k = 0; k < details.size(); k++){
        details[k]->setEditable(false);
    }
    return details;
}

QList<QStandardItem *> DatabaseFile::createInstanceRow(std::shared_ptr<taData> dataRow){
    QString lastColumnValue;
    QString defaultString;
    QList<QStandardItem *> details;
    if(dataRow->isDefault){
        defaultString = "Default";
    } else {
        defaultString = "Not Default";
    }
    if(dataRow->comment == ""){
        lastColumnValue = defaultString;
    } else {
        defaultString + ", " + dataRow->comment;
    }
    //qDebug() << Q_FUNC_INFO << "generating detail line for instance:" <<instances[i].instanceIndex << "attribute" << j << instances[i].attributes[j]->name;
    details = {new QStandardItem(dataRow->name),new QStandardItem(dataRow->type),
               new QStandardItem(dataRow->display()), new QStandardItem(dataRow->options().join(", "))
               , new QStandardItem(lastColumnValue)};
    for(int k = 0; k < details.size(); k++){
        details[k]->setEditable(false);
    }
    return details;
}

void DatabaseFile::createDBTree(){

    dataTree = new QTreeView(parent->centralContainer);
    dataModel = new QStandardItemModel;

    QHeaderView *headers = dataTree->header();
    headers->setSectionsClickable(true);

    dataTree->setGeometry(QRect(QPoint(250,50), QSize(1000,750)));
    QStandardItem *item = dataModel->invisibleRootItem();
    QList<QStandardItem *> dictHeader;
    QStandardItem *headerItem;
    QList<QStandardItem *> instanceHeader;
    QStandardItem *dictRow;
    QStandardItem *instanceRowBase;
    QList<QStandardItem *> instanceRow;
    QList<QStandardItem *> details;
    QStandardItemModel model2;

    //dictRow = {new QStandardItem("Included Files")};
    //item->appendRow(dictRow);

    //append items to matching dictrow

    dataTree->setModel(dataModel);

    QStringList dictionaryHeaders = {"File Dictionary","Type","Value","Value List","Default"};
    for(int i = 0; i < dictionaryHeaders.size(); i++){
        headerItem = new QStandardItem(dictionaryHeaders[i]);
        headerItem->setEditable(false);
        dictHeader.push_back(headerItem);
    }
    item->appendRow(dictHeader);

    for (int i = 0; i < dictionary.size();i++) {
        dictRow = new QStandardItem(dictionary[i].name);
        dictHeader.first()->appendRow(dictRow);
        if(dictionary[i].expanded){
            dataTree->expand(dictRow->index());
        }
        for(int j = 0; j<dictionary[i].attributes.size();j++){
            dictRow->appendRow(createFileDictionaryRow(dictionary[i].attributes[j]));
        }
    }

    QStringList instanceHeaders = {"Instances","Type","Value","Value List","Default"};
    for(int i = 0; i < instanceHeaders.size(); i++){
        headerItem = new QStandardItem(instanceHeaders[i]);
        headerItem->setEditable(false);
        instanceHeader.push_back(headerItem);
    }
    item->appendRow(instanceHeader);

    for (int i = 0; i < instances.size();i++) {
        instanceRowBase = new QStandardItem(instances[i].name);
        instanceRow = {instanceRowBase, new QStandardItem(QString::number(instances[i].instanceIndex))};
        instanceHeader.first()->appendRow(instanceRow);
        if(instances[i].expanded){
            dataTree->expand(instanceRowBase->index());
        }
        for(int j = 0; j<instances[i].attributes.size();j++){
            instanceRowBase->appendRow(createInstanceRow(instances[i].attributes[j]));
        }
    }

    //dataTree->setSortingEnabled(true);
    //dataTree->expandAll();
    QTreeView::connect(dataTree, &QTreeView::expanded, parent, [this](QModelIndex index){setItemExpansion(index, true);});
    QTreeView::connect(dataTree, &QTreeView::collapsed, parent, [this](QModelIndex index){setItemExpansion(index, false);});
    dataTree->expand(dataTree->model()->index(1, 0));
    dataTree->expand(dataTree->model()->index(0, 0));
    dataTree->show();
    dataTree->resizeColumnToContents(0);
    QAbstractButton::connect(dataTree, &QAbstractItemView::doubleClicked, parent, [this](QModelIndex selected){editRow(selected);});
    parent->currentModeWidgets.push_back(dataTree);
}

void DatabaseFile::removeTreeInstance(QModelIndex item){
    removeInstance(item.row());
    dataModel->removeRows(item.row(), 1, item.parent());
}

void DatabaseFile::removeInstance(int instanceIndex){
    instances.erase(instances.begin() + instanceIndex);
}

void DatabaseFile::removeAll(QString itemType){
    std::vector<dictItem>::iterator currentItem;
    for(currentItem = instances.begin(); currentItem != instances.end();){
        if(currentItem->name == itemType){
            currentItem = instances.erase(currentItem);
        } else {
            currentItem++;
        }
    }
}

dictItem* DatabaseFile::getLink(int linkID){
    //change the link value to be an int
    //then just use this function to find the linked item when needed.

    for(int i=0; i <instances.size(); i++){
        if(instances[i].instanceIndex == linkID){
            return &instances[i];
        }
    }
    return nullptr;
}

int DefinitionFile::indexIn(QString searchName){
    //get index of the given dict in the TMD type list
    for (int i = 0; i < dictionary.size(); i++) {
        if (dictionary[i].name == searchName){
            return i;
        }
    }
    return -1;
}

int DefinitionFile::dictItemIndex(int dictIndex, QString searchName){
    //get index of the given dictItem in the given TMD dict's details
    for (int i = 0; i < dictionary[dictIndex].attributes.size(); i++) {
        //qDebug() << Q_FUNC_INFO << "searching for name" << searchName << "comparing to" << dictionary[dictIndex].name << "'s" << dictionary[dictIndex].attributes[i].name;
        if (dictionary[dictIndex].attributes[i]->name == searchName) {
            return i;
        }
    }
    parent->log("Item " + searchName + " not found in " + dictionary[dictIndex].name + "| " + QString(Q_FUNC_INFO));
    return -1;
}

int DatabaseFile::instanceIndexIn(int searchIndex){
    //get index of the given instanceIndex in the given TDB's instances
    for (int i = 0; i < instances.size(); i++) {
        if (instances[i].instanceIndex == searchIndex) {
            return i;
        }
    }
    return -1;
}

int DictionaryFile::readData(){
    int passed = 0;
    if(binary){
        passed = readBinary();
    } else {
        passed = readText();
    }
    return passed;
}

int DefinitionFile::readDictionary(SectionHeader signature){
    //for BMD files

    //itemDetails->file = this;
    QString dataType;
    int sectionIndex = 0;


    long endDictionary = signature.sectionLocation + signature.sectionLength;
    int endSection = 0;

    while(fileData->currentPosition < endDictionary){
        fileData->signature(&signature);
        dictionary.resize(sectionIndex+1);
        dictionary[sectionIndex].name = signature.type;
        endSection = signature.sectionLocation + signature.sectionLength;
        while(fileData->currentPosition < endSection){
            //qDebug() << Q_FUNC_INFO << "current position:" << fileData->currentPosition << "vs end of section" << endSection;
            int typeLength = fileData->readUInt();
            dataType = fileData->readHex(typeLength);
            std::shared_ptr<taData> itemDetails = createItem(dataType);
            if (itemDetails == nullptr){
                return 1;
            }
            itemDetails->type = dataType;
            //qDebug() << Q_FUNC_INFO << "type:" << dataType << "type created" << itemDetails->type;
            int activeLength = fileData->readUInt();
            QString activeType = fileData->readHex(activeLength);
            if(activeType == "False"){
                itemDetails->active = false;
            } else {
                itemDetails->active = true;
            }
            //qDebug() << Q_FUNC_INFO << "activation:" << itemDetails.active << "active length" << activeLength;
            int nameLength = fileData->readUInt();
            itemDetails->name = fileData->readHex(nameLength);
            itemDetails->read();

            dictionary[sectionIndex].attributes.push_back(itemDetails);
        }

        sectionIndex++;

    }

    /*for(int i = 0; i < dictionary.size(); i++){
        for(int j = 0; j < dictionary[i].attributes.size(); j++){
            qDebug() << Q_FUNC_INFO << "class" << i << dictionary[i].name << "item" << j << "is" << dictionary[i].attributes[j]->index << dictionary[i].attributes[j]->type << dictionary[i].attributes[j]->name;
        }
    }*/
    return 0;
}

int DatabaseFile::readFileDictionary(SectionHeader signature){
    //for bdb files
    QString itemName;
    int sectionIndex = 0;
    int classIndexTMD = 0;

    long endDictionary = signature.sectionLocation + signature.sectionLength;
    int endSection = 0;

    while(fileData->currentPosition < endDictionary){
        fileData->signature(&signature);
        dictionary.resize(sectionIndex+1);
        dictionary[sectionIndex].name = signature.type;
        endSection = signature.sectionLocation + signature.sectionLength;
        for(int i = 0; i < inheritedFile->dictionary.size(); i++){
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "comparing class" << i << inheritedFile->dictionary[i].name << "to inherited class" << signature.type;
            if(inheritedFile->dictionary[i].name == signature.type){
                //qDebug() << Q_FUNC_INFO << "They match.";
                classIndexTMD = i;
                break;
            }
        }
        while(fileData->currentPosition < endSection){
            int nameLength = fileData->readUInt();
            itemName = fileData->readHex(nameLength);
            for(int i =0; i < inheritedFile->dictionary[classIndexTMD].attributes.size(); i++){
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == itemName){
                    dictionary[sectionIndex].attributes.push_back(inheritedFile->dictionary[classIndexTMD].attributes[i]->clone());
                }
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "PrototypeName"){
                    dictionary[sectionIndex].prototype = inheritedFile->dictionary[classIndexTMD].attributes[i]->stringValue();
                }
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "Position"){
                    dictionary[sectionIndex].position = inheritedFile->dictionary[classIndexTMD].attributes[i]->vectorValue();
                }
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "Orientation"){
                    dictionary[sectionIndex].orientation = inheritedFile->dictionary[classIndexTMD].attributes[i]->quatValue();
                }
                if(itemName == "PickupPlaced" && inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "PickupToSpawn"){
                    if(inheritedFile->dictionary[classIndexTMD].attributes[i]->intValue() == 3){
                        dictionary[sectionIndex].prototype = "DATADISK_DISK";
                    } else if (inheritedFile->dictionary[classIndexTMD].attributes[i]->intValue() > 3){
                        dictionary[sectionIndex].prototype = "PANEL";
                    }
                }
            }
            if(itemName == "CreatureWarpGate"){
                dictionary[sectionIndex].prototype = "WARP_ANIM";
            }
        }
        sectionIndex++;
    }
    //qDebug() << Q_FUNC_INFO << "Finished FileDictionary at" << fileData->currentPosition << "based on expected end of section" << endSection;
    for(int i = 0; i < dictionary.size(); i++){
        //qDebug() << Q_FUNC_INFO << "dictionary item" << i << "has" << dictionary[i].attributes.size() << "attributes";
    }
    return 0;
}

int DatabaseFile::readInstances(SectionHeader signature){
    int sectionIndex = 0;
    long endInstances = signature.sectionLength + signature.sectionLocation;
    //bool isPickup = false;
    //qDebug() << Q_FUNC_INFO << "just double checking. binary?" << binary;
    while(fileData->currentPosition < endInstances){
        fileData->signature(&signature);
        instances.resize(sectionIndex+1);

        //qDebug() << Q_FUNC_INFO << "Signature read as" << signature.type << "at" << fileData->currentPosition;
        for(int i = 0; i < dictionary.size(); i++){
            //qDebug() << Q_FUNC_INFO << "comparing class" << i << dictionary[i].name << "to inherited class" << signature.type;
            if(dictionary[i].name == signature.type){
                //qDebug() << Q_FUNC_INFO << "They match.";
                instances[sectionIndex].name = dictionary[i].name;
                instances[sectionIndex].copiedClass = dictionary[i].copiedClass;
                instances[sectionIndex].prototype = dictionary[i].prototype;
                instances[sectionIndex].position = dictionary[i].position;
                instances[sectionIndex].orientation = dictionary[i].orientation;
                for(int j = 0; j < dictionary[i].attributes.size(); j++){
                    std::shared_ptr<taData> dictionaryCopy = dictionary[i].attributes[j]->clone();
                    dictionaryCopy->file = this; //have to replace the file attribute when copying from dictionary to filedictionary
                    if(dictionaryCopy == nullptr){
                        dictionaryCopy = dictionary[i].attributes[j];
                    }
                    //qDebug() << Q_FUNC_INFO << "dictionary" << i << "instance" << j << "adding item" << dictionaryCopy->name;
                    instances[sectionIndex].attributes.push_back(dictionaryCopy);
                }
                //instances[sectionIndex] = dictionary[i];
                break;
            }
        }
        instances[sectionIndex].instanceIndex = fileData->readInt(2);
        //qDebug() << Q_FUNC_INFO << "instance index read as" << instances[sectionIndex].instanceIndex << "at" << fileData->currentPosition;

        for(int i = 0; i < instances[sectionIndex].attributes.size(); i++){
            instances[sectionIndex].attributes[i]->isDefault = fileData->readBool();
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "Item" << instances[sectionIndex].attributes[i]->name << "is type" << instances[sectionIndex].attributes[i]->type
            //         << "is default?" << instances[sectionIndex].attributes[i]->isDefault << "is binary file?" << instances[sectionIndex].attributes[i]->file->binary;
            if(!instances[sectionIndex].attributes[i]->isDefault){
                instances[sectionIndex].attributes[i]->read();
                if(instances[sectionIndex].attributes[i]->name == "PrototypeName"){
                    instances[sectionIndex].prototype = instances[sectionIndex].attributes[i]->stringValue();
                }
                if(instances[sectionIndex].attributes[i]->name == "Position"){
                    instances[sectionIndex].position = instances[sectionIndex].attributes[i]->vectorValue();
                }
                if(instances[sectionIndex].attributes[i]->name == "Orientation"){
                    instances[sectionIndex].orientation = instances[sectionIndex].attributes[i]->quatValue();
                }
                if(signature.type == "PickupPlaced" && instances[sectionIndex].attributes[i]->name == "PickupToSpawn"){
                    //isPickup = true;
                    if(instances[sectionIndex].attributes[i]->intValue() == 3){
                        instances[sectionIndex].prototype = "DATADISK_DISK";
                    } else if (instances[sectionIndex].attributes[i]->intValue() > 3){
                        instances[sectionIndex].prototype = "PANEL";
                    }
                }
            }
        }
        if(instances[sectionIndex].name == "CreatureWarpGate"){
            instances[sectionIndex].prototype = "WARP_ANIM";
            //warpgates.push_back(instances[sectionIndex]);
        }
        /*if(isPickup){
            //pickups.push_back(instances[sectionIndex]);
            isPickup = false;
        }*/
        sectionIndex++;
    }
    /*for(int i = 0; i < warpgates.size(); i++){
        qDebug() << Q_FUNC_INFO << "Warpgate" << i << "is instance index" << warpgates[i].instanceIndex << "with name" << warpgates[i].name << "and position" << warpgates[i].position;
    }*/
    maxInstances = instances.size();
    qDebug() << Q_FUNC_INFO << "file" << fileName << "has instance count" << instances.size();
    return 0;
}

int DictionaryFile::readBinary(){
    long endSection = 0;
    int failed = 0;
    /*Loops through file to find major sections, then handle the information in that section based on what type of section it is
     IncludedFiles needs to be outside the inner loop since it doesn't have subsections of its own

     Ideally I'd like to get this out of a while loop but this works well enough for now.
    */
    SectionHeader majorSignature;
    qDebug() << Q_FUNC_INFO << "THE FUNCTION RUNS. file length" << parent->fileData.dataBytes.size() << "binary?" << binary;

    parent->fileData.currentPosition = 4;
    versionNumber = parent->fileData.readUInt();
    fileData->signature(&majorSignature);
    //qDebug() << Q_FUNC_INFO << "section type: " << majorSignature.type << "current position" << parent->fileData.currentPosition << "section end:" << endSection;
    while (fileData->currentPosition != -1 and fileData->currentPosition < fileData->dataBytes.size()){
        //majorSections.push_back(majorSignature.type);
        endSection = majorSignature.sectionLocation + majorSignature.sectionLength;

        if (majorSignature.type == "IncludedFiles" and fileData->currentPosition < endSection) {
            int pathLength = fileData->readUInt();
            QString realPath = fileData->readHex(pathLength);
            failed = readIncludedFiles(realPath);
            //qDebug() << Q_FUNC_INFO << "name length" << pathLength << "test path" << realPath << "path failed?" << failed;
        }

        if(majorSignature.type == "Dictionary"){
            failed = readDictionary(majorSignature);
        }

        if (majorSignature.type == "FileDictionary"){
            failed = readFileDictionary(majorSignature);
            for(int i = 0; i < dictionary.size(); i++){
                //qDebug() << Q_FUNC_INFO << "dictionary item" << i << "has" << dictionary[i].attributes.size() << "attributes";
            }
        }

        if (majorSignature.type == "Instances") {
            //qDebug() << Q_FUNC_INFO << "reading instances. cut name:" << sectionName;
            failed = readInstances(majorSignature);
        }

        if (knownSections.indexOf(majorSignature.type) == -1) {
            parent->log("unrecognized section name: " + majorSignature.type + "| " + QString(Q_FUNC_INFO));
            failed = 1;
        }

        if(failed){
            parent->messageError("There was an error while reading the Dictionary section.");
            return 1;
        }

        //qDebug() << Q_FUNC_INFO << "temp read position" << tempRead.currentPosition << "databytes length" << tempRead.dataBytes.length();
        fileData->signature(&majorSignature);
        //qDebug() << Q_FUNC_INFO << "section type: " << majorSignature.type << "current position" << parent->fileData.currentPosition << "section end:" << endSection;
    }

    for(int i = 0; i < dictionary.size();i++){
        //qDebug() << Q_FUNC_INFO << "Section" << i << "name: " << dictionary[i].name;
        for (int j = 0; j<dictionary[i].attributes.size();j++){
            //qDebug() << Q_FUNC_INFO << "index" << j << "Item name: " << dictionary[i].attributes[j].name << "item type: " << dictionary[i].attributes[j].type;
        }
    }

    return 0;
}

int DictionaryFile::readIncludedFiles(QString fullRead){
    //read Definition file name, verify that we're using the right Definitons
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    int failed = 1;

    fullRead = fullRead.remove(quoteRemover);
    //qDebug() << Q_FUNC_INFO << fullRead << "included file" << inheritedFileName;
    if(fullRead == ""){
        //qDebug() << Q_FUNC_INFO << "No files included. We can continue.";
        failed = 0;
        return failed;
    }

    if(fullRead.contains(":")){
        failed = 1;
        parent->log("Included file references a different drive - we don't have the required file.");
        return failed;
    }

    QString extension = fullRead.right(3).toUpper();
    parent->loadRequiredFile(inputPath, fullRead, extension);

    static QRegularExpression pathRemover = QRegularExpression("../");
    inheritedFileName = fullRead.remove(pathRemover);
    inheritedFile = std::static_pointer_cast<DefinitionFile>(parent->matchFile(inheritedFileName));

    if(inheritedFile != nullptr){
        //qDebug() << Q_FUNC_INFO << "The Database file includes the loaded Definition file. We can continue.";
        failed = 0;
    }

    if(failed){
        parent->messageError("The file does not include a loaded TMD/BMD file. Please verify that the correct files are loaded."
                             "TDB/BDB file includes:" + fullRead);
    }
    return failed;

}

QString DictionaryFile::includedFileRelativePath(){
    //TFA and TFA2 are treated as being at the same level of the directory
    static QRegularExpression tfaRemover = QRegularExpression("TFA/");
    static QRegularExpression tfa2Remover = QRegularExpression("TFA2/");
    if(inheritedFile == nullptr){
        return "";
    }
    QString path;

    QDir dir(inputPath.remove(tfaRemover).remove(tfa2Remover));

    //qDebug() << Q_FUNC_INFO << "current file path" << inputPath.remove(tfaRemover).remove(tfa2Remover) << "inherited path" << inheritedFile->inputPath.remove(tfaRemover).remove(tfa2Remover);

    path = dir.relativeFilePath(inheritedFile->inputPath.remove(tfaRemover).remove(tfa2Remover));
    path = path.right(path.length()-3); //QT interprets the path as having one ../ even if the files are in the same folder
    //qDebug() << Q_FUNC_INFO << "Relative path found as:" << path;

    return path.toLower();
}

int DefinitionFile::readDictionary(){
    //for TMD files
    QString tempRead;
    QString dataType;
    int typeIndexTMD = 0;
    int sectionIndex = 0;
    //dictItem itemDetails;
    bool dictionaryEnd = false;
    bool sectionEnd = false;
    SectionHeader signature;
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    //itemDetails.file = this;


    //dictionary[sectionIndex].name = getName();

    while(!dictionaryEnd){
        //qDebug() << Q_FUNC_INFO << "reading a dictionary line at" << fileData->currentPosition;
        if(!fileData->skipLine(true)){
            qDebug() << Q_FUNC_INFO << "End of dictionary reached.";
            dictionaryEnd = true;
            continue;
        }
        dictionary.resize(sectionIndex+1);
        sectionEnd = false;
        fileData->textSignature(&signature);
        dictionary[sectionIndex].name = signature.type;
        //qDebug() << Q_FUNC_INFO << "reading class" << dictionary[sectionIndex].name;
        fileData->nextLine();
        fileData->skipLine();

        bool isOverwrittenInheritence = false;
        int overwrittenAttribute = 0;
        while(!sectionEnd){
            //check if the current line is empty - if it is, we end the section and continue to the next.
            //qDebug() << Q_FUNC_INFO << "reading a section line at" << fileData->currentPosition;
            if(!fileData->skipLine(true)){
                sectionEnd = true;
                fileData->skipLine(); //skip the // SectionName line
                continue;
            }
            dataType = fileData->textWord();
            if(dataType.contains(":")){
                //qDebug() << Q_FUNC_INFO << "inheriting data from:" << dataType;
                dataType = dataType.right(dataType.length()-1);
                dictionary[sectionIndex].copiedClass = dataType;
                for(int i = 0; i < dictionary.size(); i++){
                    //qDebug() << Q_FUNC_INFO << "comparing class" << i << dictionary[i].name << "to inherited class" << dataType;
                    if(dictionary[i].name == dataType){
                        //qDebug() << Q_FUNC_INFO << "They match.";
                        for(int j = 0; j < dictionary[i].attributes.size(); j++){
                            dictionary[sectionIndex].attributes.push_back(dictionary[i].attributes[j]->clone());
                            dictionary[sectionIndex].attributes[j]->inherited = true;
                        }
                    }
                }
                //inherit data from a previously read item
                fileData->nextLine();
                continue;
            }

            std::shared_ptr<taData> itemDetails = createItem(dataType);
            if(itemDetails == nullptr){
                parent->messageError("Unknown item type encountered.");
                return 1;
            }
            itemDetails->file = this;
            itemDetails->inherited = false;
            itemDetails->type = dataType;
            itemDetails->index = typeIndexTMD;
            tempRead = fileData->textWord().remove(quoteRemover);
            if(tempRead == "False"){
                itemDetails->active = false;
            } else {
                itemDetails->active = true;
            }
            tempRead = fileData->textWord();
            itemDetails->name = tempRead.remove(quoteRemover);
            for(int i = 0; i < dictionary[sectionIndex].attributes.size(); i++){
                if(dictionary[sectionIndex].attributes[i]->name == itemDetails->name){
                    //if this happens, then there was an inherited type that is overwritten by the new values
                    isOverwrittenInheritence = true;
                    overwrittenAttribute = i;
                }
            }
            itemDetails->read();
            if(itemDetails->type != "Enum"){
                itemDetails->comment = fileData->textWord().remove(quoteRemover);
            } else {
                itemDetails->comment = "";
            }
            fileData->nextLine();
            //qDebug() << Q_FUNC_INFO << "Data read as:" << itemDetails->type << itemDetails->name << itemDetails->active << itemDetails->display() << itemDetails->comment;

            if(isOverwrittenInheritence){
                dictionary[sectionIndex].attributes[overwrittenAttribute] = itemDetails;
                isOverwrittenInheritence = false;
            } else {
                dictionary[sectionIndex].attributes.push_back(itemDetails);
            }
            //dictionary[sectionIndex].attributes.push_back(itemDetails);
            typeIndexTMD++;
        }

        sectionIndex++;
    }
    return 0;
}

int DatabaseFile::readFileDictionary(){
    //for TDB files
    QString itemName;
    int classIndexTMD = 0;
    int typeIndexTDB = 0;
    int sectionIndex = 0;
    //dictItem itemDetails;
    bool dictionaryEnd = false;
    bool sectionEnd = false;
    SectionHeader signature;
    static QRegularExpression quoteRemover = QRegularExpression("[\[\"\\]]");
    //itemDetails.file = this;


    //fileDictionary[sectionIndex].name = getName();

    while(!dictionaryEnd){
        //qDebug() << Q_FUNC_INFO << "reading a dictionary line at" << fileData->currentPosition;
        if(!fileData->skipLine(true)){
            //qDebug() << Q_FUNC_INFO << "End of dictionary reached.";
            dictionaryEnd = true;
            continue;
        }
        dictionary.resize(sectionIndex+1);
        sectionEnd = false;
        fileData->textSignature(&signature);
        dictionary[sectionIndex].name = signature.type;
        //qDebug() << Q_FUNC_INFO << "reading class" << fileDictionary[sectionIndex].name;
        fileData->nextLine();
        fileData->skipLine();

        for(int i = 0; i < inheritedFile->dictionary.size(); i++){
            //qDebug() << Q_FUNC_INFO << "comparing class" << i << inheritedFile->fileDictionary[i].name << "to inherited class" << signature.type;
            if(inheritedFile->dictionary[i].name == signature.type){
                //qDebug() << Q_FUNC_INFO << "They match.";
                classIndexTMD = i;
                break;
            }
            if(i+1 == inheritedFile->dictionary.size()){
                parent->log("Section of type " + signature.type + " was not found in " + inheritedFile->fullFileName() + "| " + QString(Q_FUNC_INFO));
                return 1;
            }
        }
        dictionary[sectionIndex].inheritedDictionaryIndex = classIndexTMD;

        //qDebug() << Q_FUNC_INFO << "Getting attributes from inherited file for class" << dictionary[sectionIndex].name;
        while(!sectionEnd){
            //check if the current line is empty - if it is, we end the section and continue to the next.
            //qDebug() << Q_FUNC_INFO << "reading a section line at" << fileData->currentPosition;
            if(!fileData->skipLine(true)){
                sectionEnd = true;
                fileData->skipLine(); //skip the // SectionName line
                continue;
            }

            itemName = fileData->textWord().remove(quoteRemover);
            if(itemName.contains(":")){
                itemName = itemName.right(itemName.length()-1);
                //qDebug() << Q_FUNC_INFO << "inheriting data internally from:" << itemName;
                dictionary[sectionIndex].copiedClass = itemName;
                for(int i = 0; i < dictionary.size(); i++){
                    //qDebug() << Q_FUNC_INFO << "comparing class" << i << dictionary[i].name << "to inherited class" << itemName;
                    if(dictionary[i].name == itemName){
                        //qDebug() << Q_FUNC_INFO << "They match. inherited name" << dictionary[i].name << "search name:" << itemName << "inherited attribute count:" << dictionary[i].attributes.size();
                        for(int j = 0; j < dictionary[i].attributes.size(); j++){
//                            std::shared_ptr<taData> dictionaryCopy = dictionary[i].attributes[j]->clone();
//                            if(dictionaryCopy == nullptr){
//                                dictionaryCopy = dictionary[i].attributes[j];
//                            }
                            dictionary[sectionIndex].attributes.push_back(dictionary[i].attributes[j]->clone());
                            //keep an eye on this line  v
                            dictionary[sectionIndex].attributes[j]->inherited = true;
                        }
                    }
                }
                //inherit data from a previously read item
                fileData->nextLine();
                continue;
            }
            for(int i =0; i < inheritedFile->dictionary[classIndexTMD].attributes.size(); i++){
                //qDebug() << Q_FUNC_INFO << "comparing class" << i << inheritedFile->dictionary[classIndexTMD].attributes[i]->name << "to inherited class" << itemName;
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == itemName){
                    //qDebug() << Q_FUNC_INFO << "They match. adding item name:" << itemName << "attribute" << i;
                    std::shared_ptr<taData> dictionaryCopy = inheritedFile->dictionary[classIndexTMD].attributes[i]->clone();
                    if(dictionaryCopy == nullptr){
                        //qDebug() << Q_FUNC_INFO << "failed to clone";
                        dictionaryCopy = inheritedFile->dictionary[classIndexTMD].attributes[i];
                    }
                    dictionary[sectionIndex].attributes.push_back(dictionaryCopy);
                    break;
                }
            }
            for(int i =0; i < inheritedFile->dictionary[classIndexTMD].attributes.size(); i++){
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "PrototypeName"){
                    dictionary[sectionIndex].prototype = inheritedFile->dictionary[classIndexTMD].attributes[i]->stringValue();
                }
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "Position"){
                    dictionary[sectionIndex].position = inheritedFile->dictionary[classIndexTMD].attributes[i]->vectorValue();
                }
                if(inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "Orientation"){
                    dictionary[sectionIndex].orientation = inheritedFile->dictionary[classIndexTMD].attributes[i]->quatValue();
                }
                if(itemName == "PickupPlaced" && inheritedFile->dictionary[classIndexTMD].attributes[i]->name == "PickupToSpawn"){
                    if(inheritedFile->dictionary[classIndexTMD].attributes[i]->intValue() == 3){
                        dictionary[sectionIndex].prototype = "DATADISK_DISK";
                    } else if (inheritedFile->dictionary[classIndexTMD].attributes[i]->intValue() > 3){
                        dictionary[sectionIndex].prototype = "PANEL";
                    }
                }
            }
            if(itemName == "CreatureWarpGate"){
                dictionary[sectionIndex].prototype = "WARP_ANIM";
            }

            fileData->nextLine();
            typeIndexTDB++;
        }

        sectionIndex++;
    }

    /*for(int i = 0; i < dictionary.size(); i++){
        for(int j = 0; j < dictionary[i].attributes.size(); j++){
            qDebug() << Q_FUNC_INFO << "class" << i << dictionary[i].name << "item" << j << "is" << dictionary[i].attributes[j]->index << dictionary[i].attributes[j]->type << dictionary[i].attributes[j]->name;
        }
    }*/
    return 0;
}

int DatabaseFile::readInstances(){
    //for TMD files
    QString dataType;
    int itemIndex = 0;
    int sectionIndex = 0;
    //dictItem itemDetails;
    bool instancesEnd = false;
    bool sectionEnd = false;
    //bool isPickup = false;
    SectionHeader signature;
    //itemDetails.file = this;


    //fileDictionary[sectionIndex].name = getName();

    while(!instancesEnd){
        //qDebug() << Q_FUNC_INFO << "reading a instance line at" << fileData->currentPosition;
        if(!fileData->skipLine(true)){
            qDebug() << Q_FUNC_INFO << "End of instance reached.";
            instancesEnd = true;
            continue;
        }
        instances.resize(sectionIndex+1);
        sectionEnd = false;
        fileData->textSignature(&signature);
        instances[sectionIndex].name = signature.type;
        qDebug() << Q_FUNC_INFO << "reading class" << instances[sectionIndex].name;
        for(int i = 0; i < dictionary.size(); i++){
            //qDebug() << Q_FUNC_INFO << "comparing class" << i << dictionary[i].name << "to inherited class" << signature.type;
            if(dictionary[i].name == signature.type){
                instances[sectionIndex].name = dictionary[i].name;
                instances[sectionIndex].copiedClass = dictionary[i].copiedClass;
                //qDebug() << Q_FUNC_INFO << "Adding attributes for class" << instances[sectionIndex].name << instances[sectionIndex].instanceIndex;
                for(int j = 0; j < dictionary[i].attributes.size(); j++){
                    //qDebug() << Q_FUNC_INFO << "adding attribute" << j << "name" << dictionary[i].attributes[j]->name;
                    std::shared_ptr<taData> dictionaryCopy = dictionary[i].attributes[j]->clone();
                    if(dictionaryCopy == nullptr){
                        //qDebug() << Q_FUNC_INFO << "failed to clone";
                        dictionaryCopy = dictionary[i].attributes[j];
                    }
                    instances[sectionIndex].attributes.push_back(dictionaryCopy);
                }
                //instances[sectionIndex] = dictionary[i];
                break;
            }
        }
        for(int j = 0; j < instances[sectionIndex].attributes.size(); j++){
            qDebug() << Q_FUNC_INFO << "class" << sectionIndex << instances[sectionIndex].name << "item" << j << "is" << instances[sectionIndex].attributes[j]->index
                   << instances[sectionIndex].attributes[j]->type << instances[sectionIndex].attributes[j]->name;
        }
        fileData->nextLine();
        fileData->skipLine();
        itemIndex = 0;

        while(!sectionEnd){
            //check if the current line is empty - if it is, we end the section and continue to the next.
            //qDebug() << Q_FUNC_INFO << "reading a section line at" << fileData->currentPosition;
            if(!fileData->skipLine(true)){
                sectionEnd = true;
                fileData->skipLine(); //skip the // SectionName line
                continue;
            }
            dataType = fileData->textWord();
            //qDebug() << Q_FUNC_INFO << "data type read as" << dataType << "at" << fileData->currentPosition;
            if(dataType.contains(":")){
                fileData->currentPosition += 6;
                //qDebug() << Q_FUNC_INFO << "Reading object ID at" << fileData->currentPosition;
                instances[sectionIndex].instanceIndex = fileData->textWord().toInt();
                qDebug() << Q_FUNC_INFO << "setting instance index to" << instances[sectionIndex].instanceIndex;
                fileData->nextLine();
                continue;
            }
            instances[sectionIndex].attributes[itemIndex]->file = this;
            if(dataType == "NotDefault"){
                //if default, the value is already set
                //qDebug() << Q_FUNC_INFO << "reading new value for instance" << sectionIndex << "attribute" << itemIndex << "named" << instances[sectionIndex].name;
                instances[sectionIndex].attributes[itemIndex]->read();
                instances[sectionIndex].attributes[itemIndex]->isDefault = false;
                if(instances[sectionIndex].attributes[itemIndex]->name == "PrototypeName"){
                    instances[sectionIndex].prototype = instances[sectionIndex].attributes[itemIndex]->stringValue();
                }
                if(instances[sectionIndex].attributes[itemIndex]->name == "Position"){
                    instances[sectionIndex].position = instances[sectionIndex].attributes[itemIndex]->vectorValue();
                }
                if(instances[sectionIndex].attributes[itemIndex]->name == "Orientation"){
                    instances[sectionIndex].orientation = instances[sectionIndex].attributes[itemIndex]->quatValue();
                }
                if(signature.type == "PickupPlaced" && instances[sectionIndex].attributes[itemIndex]->name == "PickupToSpawn"){
                    //isPickup = true;
                    if(instances[sectionIndex].attributes[itemIndex]->intValue() == 3){
                        instances[sectionIndex].prototype = "DATADISK_DISK";
                    } else if (instances[sectionIndex].attributes[itemIndex]->intValue() > 3){
                        instances[sectionIndex].prototype = "PANEL";
                    }
                }
            }
            if(instances[sectionIndex].name == "CreatureWarpGate"){
                instances[sectionIndex].prototype = "WARP_ANIM";
            }
            /*if(isPickup){
                //pickups.push_back(instances[sectionIndex]);
                isPickup = false;
            }*/
            fileData->nextLine();
            qDebug() << Q_FUNC_INFO << "Data read as:" << instances[sectionIndex].attributes[itemIndex]->type << instances[sectionIndex].attributes[itemIndex]->name
                     << instances[sectionIndex].attributes[itemIndex]->active << instances[sectionIndex].attributes[itemIndex]->display() << instances[sectionIndex].attributes[itemIndex]->comment;

            itemIndex++;
        }

        sectionIndex++;
    }
    maxInstances = instances.size();
    qDebug() << Q_FUNC_INFO << "file" << fileName << "has instance count" << instances.size();
    return 0;
}

void DatabaseFile::createItem(){
    bool cancelled;
    QStringList options;
    for(int i = 0; i < dictionary.size(); i++){
        options.append(dictionary[i].name);
    }

    QString chosenClass = QInputDialog::getItem(parent, parent->tr("Select TDB File:"), parent->tr("File Name:"), options, 0, false, &cancelled);
    //then a dialog box for each value in that class prompting for user input
    //give an option for "default"
}


int DictionaryFile::readText(){
    //read file
    //file.split("\n")
    //line.trimmed().split(" ")
    //with this, the data is now prepped - store the above like we would store a FileData array

    //might be better if we go with a purely binary approach, but not sure how hard that'll be
    //could also just have a separate type of FileData - TextFileData or something

    /*text equivalent of "signature":
    read first item of line. if first character is ~, this is the beginning of a new section
    skip the next line - it's just an open bracket
    search through the following lines until you reach a line that is "} // signature.type"
    or
    until you reach an empty line, then verify the next line is "} // signature.type"
    the number of lines between the starting bracket and empty line is how many lines of data the section has

    best part is, this should be compatible with the existing sectionheader class
    using EFFECTSMATERIALASSIGNMENT.TMD, visually
    signature.type = "Dictionary"
    signature.sectionLocation = 8
    signature.sectionLength = 18
    just need another fileData function for reading a text signature instead
    */
    int failed = 0;
    taData itemDetails;
    itemDetails.file = this;
    SectionHeader majorSignature;
    SectionHeader minorSignature;
    //QString majorName;
    //qDebug() << Q_FUNC_INFO << "THE FUNCTION RUNS. file length" << parent->fileData.dataBytes.size();

    fileData->currentPosition = 0;
    versionNumber = fileData->readHex(4).toInt(nullptr);
    fileData->nextLine();
    //majorName = getName();
    fileData->textSignature(&majorSignature);
    fileData->nextLine();
    fileData->skipLine();

    qDebug() << Q_FUNC_INFO << "version number" << versionNumber << "signature read as" << majorSignature.type << "currently at" << fileData->currentPosition;

    while (fileData->currentPosition != -1 and fileData->currentPosition < fileData->dataBytes.size()){
        //majorSections.push_back(majorSignature.type);

        qDebug() << Q_FUNC_INFO << "Checking current signature" << majorSignature.type << "at" << fileData->currentPosition;
        if(majorSignature.type == "IncludedFiles"){
            QString includePath = "";
            //if there are no included files, then this section will contain a line with a single tab character (0x09)
            if(fileData->skipLine(true)){
                includePath = fileData->textWord();
                fileData->nextLine();
            }
            if (readIncludedFiles(includePath)) {
                return 1;
            }
            fileData->skipLine();
            fileData->skipLine(); //skip 2 lines at the end of IncludedFiles - there's an extra space between this and dictionary/filedictionary sections
            qDebug() << Q_FUNC_INFO << "finished reading included files at" << fileData->currentPosition;

        }

        if(majorSignature.type == "Dictionary"){
            qDebug() << Q_FUNC_INFO << "Reading a dictionary section - TMD files only.";
            failed = readDictionary();
            if(failed){
                qDebug() << Q_FUNC_INFO << "File name:" << fileName << "and type" << fileExtension;
                parent->messageError("There was an error while reading the Dictionary section.");
                return 1;
            }
            fileData->nextLine();
        }

        if(majorSignature.type == "FileDictionary"){
            qDebug() << Q_FUNC_INFO << "Reading a filedictionary section - TDB files only.";
            failed = readFileDictionary();
            if(failed){
                parent->messageError("There was an error while reading the FileDictionary section.");
                return 1;
            }
            fileData->nextLine();
            //qDebug() << Q_FUNC_INFO << "finsihed reading filedictionary section at" << fileData->currentPosition;
        }

        if(majorSignature.type == "Instances"){
            qDebug() << Q_FUNC_INFO << "Reading an Instances section - TDB files only.";
            failed = readInstances();
            if(failed){
                parent->messageError("There was an error while reading the Instance section.");
                return 1;
            }
            fileData->nextLine();
        }
        if(fileData->currentPosition == fileData->dataBytes.length()){
            break;
        }
        fileData->skipLine(true);
        fileData->textSignature(&majorSignature);
        fileData->nextLine();
        fileData->skipLine();
    }

    return 0;
}


void DatabaseFile::writeText(){
    //QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output TDB"), QDir::currentPath() + "/TDB/", parent->tr("Definition Files (*.tdb)"));
    qDebug() << Q_FUNC_INFO << "writing text database (TDB) file";
    QFile tdbOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    QString value;
    int startPoint = 0; //used for inherited classes

    if (tdbOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&tdbOut);

        tdbOut.write(QString::number(versionNumber).toUtf8());
        tdbOut.write(" \r\n");
        tdbOut.write("~IncludedFiles \r\n{\r\n");
        tdbOut.write("	");
        if(inheritedFile->fileName.trimmed().size() != 0){
            //need the relative path here. the ../../ stuff.
            qDebug() << Q_FUNC_INFO << "includedFile value" << inheritedFileName.toUtf8();
            tdbOut.write("\"" + inheritedFileName.toUtf8() + "\" \r\n");
        }
        tdbOut.write("	\r\n} // IncludedFiles\r\n");
        tdbOut.write("\r\n~FileDictionary \r\n{");
        for(int i = 0; i < dictionary.size(); i++){
            startPoint = 0;
            tdbOut.write("\r\n	~");
            tdbOut.write(dictionary[i].name.toUtf8());

            tdbOut.write(" \r\n	{");
            //qDebug() << Q_FUNC_INFO << "writing attributes for class" << dictionary[i].name << "with inherited class" << dictionary[i].copiedClass <<". Attributes to write:" << dictionary[i].attributes.size();
            for(int j = startPoint; j < dictionary[i].attributes.size(); j++){
                //qDebug() << Q_FUNC_INFO << "checking attribute" << dictionary[i].attributes[j]->name << ". is inherited?" << dictionary[i].attributes[j]->inherited;
                //check the dictionary copier - both the inherited and inheritor classes are not
                //being written properly in the file dicitonary
                tdbOut.write("\r\n		");
                tdbOut.write("\"" + dictionary[i].attributes[j]->name.toUtf8() + "\" ");
            }
            tdbOut.write("\r\n		\r\n	} // " + dictionary[i].name.toUtf8());
        }
        tdbOut.write("\r\n	\r\n} // FileDictionary\r\n");
        tdbOut.write("~Instances \r\n{");
        for(int i = 0; i < instances.size(); i++){
            startPoint = 0;
            tdbOut.write("\r\n	~");
            tdbOut.write(instances[i].name.toUtf8());

            tdbOut.write(" \r\n	{");
            //qDebug() << Q_FUNC_INFO << "class" << fileDictionary[i].name << "inherits" << fileDictionary[i].inheritedClass;
            tdbOut.write("\r\n		");
            tdbOut.write("ObjectId:       " + QString::number(instances[i].instanceIndex).toUtf8() + " ");
            //qDebug() << Q_FUNC_INFO << "type index" << typeIndexTMD << "start point" << startPoint;
            for(int j = startPoint; j < instances[i].attributes.size(); j++){
                tdbOut.write("\r\n		");
                if(instances[i].attributes[j]->isDefault){
                    tdbOut.write("Default ");
                } else {
                    value = instances[i].attributes[j]->databaseOutput();
                    tdbOut.write("NotDefault " + value.toUtf8());
                }
            }
            tdbOut.write("\r\n		\r\n	} // " + instances[i].name.toUtf8());
        }
        tdbOut.write("\r\n	\r\n} // Instances\r\n");
    }
}

void DefinitionFile::writeText(){
    //QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output TMD"), QDir::currentPath() + "/TMD/", parent->tr("Definition Files (*.tmd)"));
    qDebug() << Q_FUNC_INFO << "writing text definition (TMD) file.";
    QFile tmdOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    QString value;
    int startPoint = 0; //used for inherited classes

    if (tmdOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&tmdOut);

        tmdOut.write(QString::number(versionNumber).toUtf8());
        tmdOut.write(" \r\n");
        tmdOut.write("~IncludedFiles \r\n{\r\n");
        tmdOut.write("	");
        if(inheritedFile != nullptr){
            //qDebug() << Q_FUNC_INFO << "includedFile value" << inheritedFile->fullFileName().trimmed().toUtf8();
            tmdOut.write(inheritedFile->fileName.trimmed().toUtf8() + "\r\n");
        }
        tmdOut.write("\r\n} // IncludedFiles\r\n");
        tmdOut.write("\r\n~Dictionary \r\n{");
        //qDebug() << Q_FUNC_INFO << "file has" << dictionary.size() << "classes";
        for(int i = 0; i < dictionary.size(); i++){
            startPoint = 0;
            tmdOut.write("\r\n	~");
            tmdOut.write(dictionary[i].name.toUtf8());

            tmdOut.write(" \r\n	{");
            //qDebug() << Q_FUNC_INFO << "class" << dictionary[i].name << "inherits" << dictionary[i].inheritedClass;
            if (dictionary[i].copiedClass != "") {
                tmdOut.write("\r\n		");
                tmdOut.write(":" + dictionary[i].copiedClass.toUtf8() + " ");
                //typeIndexTMD = indexIn(dictionary[i].inheritedClass);
                //startPoint = dictionary[typeIndexTMD].attributes.size();
                //qDebug() << Q_FUNC_INFO << "type index" << typeIndexTMD << "start point" << startPoint;
            }
            qDebug() << Q_FUNC_INFO << "class" << dictionary[i].name << "has" << dictionary[i].attributes.size() << "items";
            for(int j = startPoint; j < dictionary[i].attributes.size(); j++){
                qDebug() << Q_FUNC_INFO << "item" << j << dictionary[i].attributes[j]->name << "has inherited class?" << dictionary[i].attributes[j]->inherited;
                if(!dictionary[i].attributes[j]->inherited){
                    tmdOut.write("\r\n		");
                    tmdOut.write(dictionary[i].attributes[j]->type.toUtf8());
                    if (dictionary[i].attributes[j]->active) {
                        tmdOut.write(" \"True\"");
                    } else {
                        tmdOut.write(" \"False\"");
                    }
                    tmdOut.write(" \"" + dictionary[i].attributes[j]->name.toUtf8() + "\" ");
                    value = dictionary[i].attributes[j]->definitionOutput();
                    //function that returns qstring "value" based on item type
                    tmdOut.write(value.toUtf8());
                }
            }
            tmdOut.write("\r\n		\r\n	} // " + dictionary[i].name.toUtf8());
        }
        tmdOut.write("\r\n	\r\n} // Dictionary\r\n");
    }
}

void DefinitionFile::writeBinary(){
    //QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output BMD"), QDir::currentPath() + "/BMD/", parent->tr("Definition Files (*.bmd)"));
    QFile bmdOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    long sectionLength = 4; //section length of the dictionary
    int checkLength = 0; //purely for debugging
    QString includedFilePath = includedFileRelativePath();

    if (bmdOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&bmdOut);
        //getFileLengths();

        bmdOut.write("FISH");
        parent->binChanger.intWrite(bmdOut, versionNumber);
        bmdOut.write("~IncludedFiles");
        parent->binChanger.shortWrite(bmdOut, 0);
        if(inheritedFile != nullptr){
            qDebug() << Q_FUNC_INFO << "includedFile value" << includedFilePath << "length" << 4+includedFilePath.length();
            parent->binChanger.intWrite(bmdOut, 4+includedFilePath.length());
            bmdOut.write(includedFilePath.toUtf8());
        } else {
            parent->binChanger.intWrite(bmdOut, 4);
        }
        bmdOut.write("~Dictionary");
        parent->binChanger.shortWrite(bmdOut, 0);

        for(int i = 0; i < dictionary.size(); i++){
            sectionLength += 4; //int length for dictItem
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                checkLength = dictionary[i].attributes[j]->binarySize();
                //qDebug() << Q_FUNC_INFO << "first pass attribute" << dictionary[i].attributes[j]->name << "is length" << checkLength;
                sectionLength += checkLength;
            }
            sectionLength += dictionary[i].name.length()+3; //2 for spacing, 1 for tilde
        }

        parent->binChanger.intWrite(bmdOut, sectionLength);

        for(int i = 0; i < dictionary.size(); i++){
            dictionary[i].length = 4; //setting this just to be sure - not sure if this is initialized elsewhere
            bmdOut.write("~" + dictionary[i].name.toUtf8());
            parent->binChanger.shortWrite(bmdOut, 0);
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                checkLength = dictionary[i].attributes[j]->binarySize();
                //qDebug() << Q_FUNC_INFO << "second pass attribute" << dictionary[i].attributes[j]->name << "is length" << checkLength;
                dictionary[i].length += checkLength;
            }
            parent->binChanger.intWrite(bmdOut, dictionary[i].length);
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                qDebug() << Q_FUNC_INFO << "class" << dictionary[i].name << "item" << j << "is type" << dictionary[i].attributes[j]->type;
                parent->binChanger.intWrite(bmdOut, dictionary[i].attributes[j]->type.length());
                bmdOut.write(dictionary[i].attributes[j]->type.toUtf8());
                if(dictionary[i].attributes[j]->active){
                    parent->binChanger.intWrite(bmdOut, 4);
                    bmdOut.write("True");
                } else {
                    parent->binChanger.intWrite(bmdOut, 5);
                    bmdOut.write("False");
                }
                parent->binChanger.intWrite(bmdOut, dictionary[i].attributes[j]->name.length());
                bmdOut.write(dictionary[i].attributes[j]->name.toUtf8());
                dictionary[i].attributes[j]->write(bmdOut);
                //binaryOutput(bmdOut, dictionary[i].attributes[j]);
            }
        }
    }
}

void DatabaseFile::writeBinary(){
    //QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output BDB"), QDir::currentPath() + "/BDB/", parent->tr("Definition Files (*.bdb)"));
    QFile bmdOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();
    long sectionLength = 4;
    int checkLength = 0;
    QString includedFilePath = includedFileRelativePath();

    if (bmdOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&bmdOut);
        //getFileLengths();

        bmdOut.write("FISH");
        parent->binChanger.intWrite(bmdOut, versionNumber);
        bmdOut.write("~IncludedFiles");
        parent->binChanger.shortWrite(bmdOut, 0);
        if(inheritedFile != nullptr){
            qDebug() << Q_FUNC_INFO << "includedFile value" << includedFilePath.toUtf8() << "length" << 4+includedFilePath.length();
            parent->binChanger.intWrite(bmdOut, 8+includedFilePath.length());
            parent->binChanger.intWrite(bmdOut, includedFilePath.length());
            bmdOut.write(includedFilePath.toUtf8());
        } else {
            parent->binChanger.intWrite(bmdOut, 4);
        }
        bmdOut.write("~FileDictionary");
        parent->binChanger.shortWrite(bmdOut, 0);

        for(int i = 0; i < dictionary.size(); i++){
            sectionLength += 4; //int length for dictItem
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                checkLength = dictionary[i].attributes[j]->name.length()+4;
                //qDebug() << Q_FUNC_INFO << "first pass attribute" << dictionary[i].attributes[j]->name << "is length" << checkLength;
                sectionLength += checkLength;
            }
            sectionLength += dictionary[i].name.length()+3; //2 for spacing, 1 for tilde
        }

        parent->binChanger.intWrite(bmdOut, sectionLength);

        for(int i = 0; i < dictionary.size(); i++){
            dictionary[i].length = 4;
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                checkLength = dictionary[i].attributes[j]->name.length()+4;
                //qDebug() << Q_FUNC_INFO << "second pass attribute" << dictionary[i].attributes[j]->name << "is length" << checkLength;
                dictionary[i].length += checkLength;
            }
            bmdOut.write("~" + dictionary[i].name.toUtf8());
            parent->binChanger.shortWrite(bmdOut, 0);
            parent->binChanger.intWrite(bmdOut, dictionary[i].length);
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                parent->binChanger.intWrite(bmdOut, dictionary[i].attributes[j]->name.length());
                bmdOut.write(dictionary[i].attributes[j]->name.toUtf8());
            }
        }

        bmdOut.write("~Instances");
        parent->binChanger.shortWrite(bmdOut, 0);
        sectionLength = 4;

        for(int i = 0; i < instances.size(); i++){
            sectionLength += instances[i].name.length()+3; //2 for spacing, 1 for tilde
            sectionLength += 4; //int length for dictItem
            sectionLength += 2; //short length for instance ID
            for(int j = 0; j < instances[i].attributes.size(); j++){
                checkLength = 1; //bool for isDefault
                if(!instances[i].attributes[j]->isDefault){
                    checkLength += instances[i].attributes[j]->size();
                }
                //qDebug() << Q_FUNC_INFO << "first pass attribute" << instances[i].attributes[j]->name << "is length" << checkLength;
                sectionLength += checkLength;
            }
        }

        parent->binChanger.intWrite(bmdOut, sectionLength);

        for(int i = 0; i < instances.size(); i++){
            instances[i].length += 6;
            for(int j = 0; j < instances[i].attributes.size(); j++){
                checkLength = 1; //bool for isDefault
                if(!instances[i].attributes[j]->isDefault){
                    checkLength += instances[i].attributes[j]->size();
                }
                //qDebug() << Q_FUNC_INFO << "second pass attribute" << instances[i].attributes[j]->name << "is length" << checkLength;
                instances[i].length += checkLength;
            }
            bmdOut.write("~" + instances[i].name.toUtf8());
            parent->binChanger.shortWrite(bmdOut, 0);
            parent->binChanger.intWrite(bmdOut, instances[i].length);
            parent->binChanger.shortWrite(bmdOut, instances[i].instanceIndex);
            for(int j = 0; j < instances[i].attributes.size(); j++){
                if(instances[i].attributes[j]->isDefault){
                    parent->binChanger.byteWrite(bmdOut, 1);
                } else {
                    parent->binChanger.byteWrite(bmdOut, 0);
                    instances[i].attributes[j]->write(bmdOut);
                }
            }
        }
    }
}

void DatabaseFile::acceptVisitor(ProgWindow& visitor){
    visitor.visit(*this);
}

std::vector<dictItem> DatabaseFile::sendInstances(QString instanceType){
    std::vector<dictItem> itemList;
    for(int i = 0; i < instances.size(); i++){
        if(instances[i].name == instanceType){
            itemList.push_back(instances[i]);
        }
    }
    return itemList;
}

std::vector<std::shared_ptr<taData>> DatabaseFile::generateAttributes(QString className){
    int classIndexTMD = 0;
    std::vector<std::shared_ptr<taData>> generatedAttributes;
    for(int i = 0; i < inheritedFile->dictionary.size(); i++){
        qDebug() << "comparing class" << i << inheritedFile->dictionary[i].name << "to inherited class" << className;
        if(inheritedFile->dictionary[i].name == className){
            qDebug() << Q_FUNC_INFO << "They match.";
            classIndexTMD = i;
            break;
        }
    }
    for(int j =0; j < inheritedFile->dictionary[classIndexTMD].attributes.size(); j++){
        qDebug() << "adding attribute from class" << inheritedFile->dictionary[classIndexTMD].name << ":" << inheritedFile->dictionary[classIndexTMD].attributes[j]->name;
        std::shared_ptr<taData> dictionaryCopy = inheritedFile->dictionary[classIndexTMD].attributes[j]->clone();
        if(dictionaryCopy == nullptr){
            dictionaryCopy = inheritedFile->dictionary[classIndexTMD].attributes[j];
        }
        generatedAttributes.push_back(dictionaryCopy);
    }
    qDebug() << Q_FUNC_INFO << "generated attributes:" << generatedAttributes.size();
    return generatedAttributes;
}

int DefinitionFile::createClass(){
    /*for a class definition, we need:
        Name
        Name of inherited class, if applicable
        List of attributes*/

    dictItem customClass;
    bool isDialogOpen = true;

    QStringList neededItems = {"lineedit", "combobox"};
    CustomPopup* dialogCreateClass = parent->makeSpecificPopup(isDialogOpen, neededItems, {"Class Name:", "Inherited Class:"});
    dialogCreateClass->setWindowTitle("Create Database Definition");

    QLineEdit::connect(dialogCreateClass->lineOption, &QLineEdit::textEdited, [=, &customClass](QString text){customClass.name = text;});

    /*to avoid making a for loop starting on 1,
    test if inserting at 0 after the loop works intuitively*/
    dialogCreateClass->comboOption->insertItem(0, "No Inheritance");
    for(int i = 1; i <= dictionary.size(); i++){
        dialogCreateClass->comboOption->insertItem(i, dictionary[i-1].name);
    }


    int resultDialog = 0;
    QComboBox::connect(dialogCreateClass->comboOption, &QComboBox::currentIndexChanged, [=, &customClass](int index)
    {
        if(index>0){
            customClass.copiedClass = dictionary[index-1].name;
            customClass.inheritedDictionaryIndex = index-1;
        } else {
            customClass.copiedClass = "";
            customClass.inheritedDictionaryIndex = 0;
        }
    });

    dialogCreateClass->open();
    while(isDialogOpen){
        parent->forceProcessEvents();
    }
    resultDialog = dialogCreateClass->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return 1;
    }

    if(customClass.copiedClass != ""){
        for(int i = 0; i < dictionary.size(); i++){
            if(dictionary[i].name == inheritedClassName){
                customClass.attributes = dictionary[i].attributes;
            }
        }
    }

    /*then we can repeat with the attributes - yay*/
    dictionary.push_back(customClass);
    updateCenter();

    return 0;
}

int DatabaseFile::createInstance(){
    /*Propts the user to select a class, then adds a default instance of that class to the instance list. The values can be edited as needed with the DatabaseFile::editAttribute function*/
    dictItem addedInstance;
    bool isDialogOpen = true;

    /*The "makePopup" function can probably be expanded into its own class
    needed interactions:
        checkbox for boolean input. Should only need one
        line edit for names. should only need one
        combo box for selection from a list. should only need one?
        multi line edit for enum options. definitely only need one.*/
    QDialog* dialogAddInstance = parent->makePopup(isDialogOpen);
    dialogAddInstance->setWindowTitle("Chose Class of Item to Add");

    QComboBox* comboInstanceOptions = new QComboBox(dialogAddInstance);
    comboInstanceOptions->setGeometry(QRect(QPoint(20, 60), QSize(150,30)));
    for(int i = 1; i <= dictionary.size(); i++){
        comboInstanceOptions->insertItem(i, dictionary[i].name);
    }

    int selectedItem = 0;
    QComboBox::connect(comboInstanceOptions, &QComboBox::currentIndexChanged, [=, &selectedItem](int index){selectedItem = index;});

    dialogAddInstance->open();
    while(isDialogOpen){
        parent->forceProcessEvents();
    }
    int resultDialog = dialogAddInstance->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return 1;
    }

    addedInstance.name = dictionary[selectedItem].name;
    addedInstance.copiedClass = dictionary[selectedItem].copiedClass;
    addedInstance.prototype = dictionary[selectedItem].prototype;
    addedInstance.position = dictionary[selectedItem].position;
    addedInstance.orientation = dictionary[selectedItem].orientation;
    for(int j = 0; j < dictionary[selectedItem].attributes.size(); j++){
        std::shared_ptr<taData> dictionaryCopy = dictionary[selectedItem].attributes[j]->clone();
        dictionaryCopy->file = this; //have to replace the file attribute when copying from dictionary to filedictionary
        if(dictionaryCopy == nullptr){
            dictionaryCopy = dictionary[selectedItem].attributes[j];
        }
        addedInstance.attributes.push_back(dictionaryCopy);
    }
    addedInstance.instanceIndex = maxInstances + 1;

    instances.push_back(addedInstance);
    return 0;
}

dictItem* DictionaryFile::getDictionaryItem(QString itemName){
    for(int i = 0; i < dictionary.size(); i++){
        if(dictionary[i].name == itemName){
            return &dictionary[i];
        }
    }
    return nullptr;
}

int DatabaseFile::getInstance(QString itemName){
    for(int i = 0; i < instances.size(); i++){
        if(instances[i].name == itemName){
            return i;
        }
    }
    return -1;
}

int DatabaseFile::getInstance(int instanceID){
    qDebug() << Q_FUNC_INFO << "Checking for instance ID" << instanceID << "from" << instances.size() << "instances";
    for(int i = 0; i < instances.size(); i++){
        qDebug() << Q_FUNC_INFO << "instance" << i << "has index" << instances[i].instanceIndex;
        if(instances[i].instanceIndex == instanceID){
            qDebug() << Q_FUNC_INFO << "returning instance" << instances[i].name;
            return i;
        }
    }
    qDebug() << Q_FUNC_INFO << "returning nullptr";
    return -1;
}

void DefinitionFile::editRow(QModelIndex selected){
    QString firstColumn = selected.siblingAtColumn(0).data().toString();
    QString secondColumn = selected.siblingAtColumn(1).data().toString();
    QString parentName = selected.parent().data().toString();
    qDebug() << Q_FUNC_INFO << "selection column 0:" << firstColumn << "column 1" << secondColumn << "parent name" << parentName;
    //if parentName == "", user selected the file header. prompt to add a class.
    //if parentName == file name, user selected a class. for definition files, provide an option to add a new attribute
    //if parentName == a class (or secondColumn != ""), user selected an attribute. for definition files, provide all attribute editing options
    if(parentName == ""){
        /*User selected the file header row. Prompt to add a new class.*/
        createClass();
    } else if(parentName == fileName){
        /*User selected a class header. Prompt to add a new attribute to that class.*/
        for(int i = 0; i < dictionary.size(); i++){
            if(dictionary[i].name == firstColumn){
                qDebug() << Q_FUNC_INFO << "item" << dictionary[i].name << "has" << dictionary[i].attributes.size() << "attriutes before adding";
                while(!dictionary[i].addAttribute()){

                }
                qDebug() << Q_FUNC_INFO << "item" << dictionary[i].name << "has" << dictionary[i].attributes.size() << "attriutes after adding";
            }
        }
    } else if(secondColumn != ""){
        /*User selected an attribute. Prompt to edit that attribute's properties.
        if the attribute is an enum, the user can add new options since this is a definition file*/
        int selectedClassIndex = 0;
        for(int i = 0; i < dictionary.size(); i++){
            if(parentName == dictionary[i].name){
                selectedClassIndex = i;
            }
        }
        for(int i = 0; i < dictionary[selectedClassIndex].attributes.size(); i++){
            std::shared_ptr<taData> editedData = nullptr;
            if(secondColumn == "Enum" && firstColumn == dictionary[selectedClassIndex].attributes[i]->name){
                editedData = dictItem::editEnumDefinition(dictionary[selectedClassIndex].attributes[i]);
                if(editedData == nullptr){
                    /*this one should never happen - editEnumDefinition always returns the fed item on failure*/
                    parent->log("Type " + secondColumn + " is not currently supported by the database edit system.");
                    return;
                }
                dictionary[selectedClassIndex].attributes[i] = editedData;
            } else if(firstColumn == dictionary[selectedClassIndex].attributes[i]->name){
                editedData = dictItem::editAttributeValue(secondColumn, dictionary[selectedClassIndex].attributes[i]);
                if(editedData == nullptr){
                    parent->log("Type " + secondColumn + " is not currently supported by the database edit system.");
                    return;
                }
                dictionary[selectedClassIndex].attributes[i] = editedData;
            }
        }
    }

    //change this to just update the selected row - updating center minimizes all
    updateCenter();

}

void DefinitionFile::setItemExpansion(QModelIndex expanded, bool state){
    QString firstColumn = expanded.siblingAtColumn(0).data().toString();
    QString secondColumn = expanded.siblingAtColumn(1).data().toString();
    QString parentName = expanded.parent().data().toString();
    qDebug() << Q_FUNC_INFO << "expansion column 0:" << firstColumn << "column 1" << secondColumn << "parent name" << parentName;
    //if parentName == "", user expanded the file header. Do nothing
    //if parentName == file name, user expanded a class.
    //if parentName == a class (or secondColumn != ""), user somehow expanded an attribute. also do nothing
    if(parentName == ""){
        /*User expanded the file header row.*/
        return;
    } else if(parentName == fileName){
        /*User expanded a class. */
        for(int i = 0; i < dictionary.size(); i++){
            if(dictionary[i].name == firstColumn){
                qDebug() << Q_FUNC_INFO << "item" << dictionary[i].name << "has been expanded.";
                dictionary[i].expanded = state;
            }
        }
    }
}

void DatabaseFile::setItemExpansion(QModelIndex expanded, bool state){
    QString firstColumn = expanded.siblingAtColumn(0).data().toString();
    QString secondColumn = expanded.siblingAtColumn(1).data().toString();
    QString parentName = expanded.parent().data().toString();
    qDebug() << Q_FUNC_INFO << "expansion column 0:" << firstColumn << "column 1" << secondColumn << "parent name" << parentName;
    /*if parentName == "" and firstColumn == "File Dictionary",
    */

    if(parentName == "" and firstColumn == "File Dictionary"){
        //user expanded the file dictionary.
        return;
    } else if(parentName == "" and firstColumn == "Instances"){
        //user expanded the instance list.
        return;
    } else if(parentName == "File Dictionary"){
        //user expanded a class.
        for(int i = 0; i < dictionary.size(); i++){
            if(dictionary[i].name == firstColumn){
                dictionary[i].expanded = state;
            }
        }
    } else if(parentName == "Instances"){
        //user expanded an instance of an item.
        for(int i = 0; i < instances.size(); i++){
            if(instances[i].instanceIndex == secondColumn.toInt()){
                instances[i].expanded = state;
            }
        }
    }
}

void DatabaseFile::editRow(QModelIndex selected){
    QString firstColumn = selected.siblingAtColumn(0).data().toString();
    QString secondColumn = selected.siblingAtColumn(1).data().toString();
    QString parentName = selected.parent().data().toString();
    qDebug() << Q_FUNC_INFO << "selection column 0:" << firstColumn << "column 1" << secondColumn << "parent name" << parentName;
    /*if parentName == "" and firstColumn == "File Dictionary",
    */

    if(parentName == "" and firstColumn == "File Dictionary"){
        //user selected the file dictionary. provide an option to pull another class from the definition.
        addFileDictionaryClass();
    } else if(parentName == "" and firstColumn == "Instances"){
        //user selected the instance list. provide an option to add a new instance of an item.
        addNewInstance();
    } else if(parentName == "File Dictionary"){
        //user selected a class. for database files, provide an option to pull another attribute from the definition file
        addFileDictionaryAttributes(firstColumn);
    } else if(parentName == "Instances"){
        //user selected an instance of an item. provide an option to copy or delete that instance
        copyOrDeleteInstance(secondColumn.toInt());
    } else if(parentName != ""){
        //by process of elimination, we know the user selected an attribute. What we do now depends on where that attribute is
        if(selected.parent().parent().data().toString() == "Instances"){
            int selectedInstanceID = selected.parent().siblingAtColumn(1).data().toInt();
            //user is editing an instance. We can edit the value of this one
            editAttributeValue(selectedInstanceID, parentName, firstColumn);
        }
        if(selected.parent().parent().data().toString() == "File Dictionary"){
            //user wants to edit the file dictionary. Can't do that, should edit the definition file directly.
            parent->messageError("The attributes in the File Dictionary cannot be edited from the Database file. Edit these from the related Definition file instead.");
        }
    }

    //change this to just update the selected row - updating center minimizes all
    updateCenter();
}

void DatabaseFile::copyOrDeleteInstance(int instanceID){
    bool isDialogOpen = true;
    CustomPopup* dialogCopyDelete = ProgWindow::makeSpecificPopup(isDialogOpen, {"combobox"}, {""});

    dialogCopyDelete->comboOption->addItem("Create copy");
    dialogCopyDelete->comboOption->addItem("Delete instance");

    dialogCopyDelete->open();
    while(isDialogOpen){
        ProgWindow::forceProcessEvents();
    }
    int resultDialog = dialogCopyDelete->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }

    int userSelection = dialogCopyDelete->comboOption->currentIndex();
    switch(userSelection){
        case 0: //make a copy
            copyInstance(instanceID);
            break;
        case 1: //User chose to delete
            removeInstance(instanceID);
            break;
        case 2: //user chose to change instance type
            //not implemented yet but could be useful
            break;
        default:
            qDebug() << Q_FUNC_INFO << "Unexpected selection value - how did you even do this?";
            break;
    }

}

void DatabaseFile::copyInstance(int instanceID){
    qDebug() << Q_FUNC_INFO << "Getting instance ID" << instanceID;
    int indexToCopy = getInstance(instanceID);
    maxInstances++;
    int nextInstanceID = instances.size();
    instances.resize(instances.size()+1);
    instances[nextInstanceID].instanceIndex = maxInstances;
    instances[nextInstanceID].name = instances[indexToCopy].name;
    for(int i = 0; i < instances[indexToCopy].attributes.size(); i++){
        instances[nextInstanceID].attributes.push_back(instances[indexToCopy].attributes[i]->clone());
    }
}

void DatabaseFile::editAttributeValue(int selectedInstanceID, QString instanceName, QString attributeName){
    /*Should do two popups - one asks if the value will as for default/change, the other edits the value if change is chosen*/
    bool isDialogOpen = true;
    CustomPopup* dialogGetDefault = ProgWindow::makeSpecificPopup(isDialogOpen, {"checkbox"}, {""});
    dialogGetDefault->setWindowTitle("Set to Default?");
    dialogGetDefault->checkOption->setText("Check to set value to default.");
    dialogGetDefault->open();
    while(isDialogOpen){
        ProgWindow::forceProcessEvents();
    }
    int resultDialog = dialogGetDefault->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }
    int indexToEdit = getInstance(selectedInstanceID);
    std::shared_ptr<taData> instanceData;
    instanceData = instances[indexToEdit].getAttribute(attributeName);
    if(dialogGetDefault->checkOption->isChecked()){
        std::shared_ptr<taData> defaultData;
        defaultData = getDictionaryItem(instanceName)->getAttribute(attributeName);
        instanceData->setValue(defaultData->display());
        instanceData->isDefault = true;
    } else {
        instanceData = dictItem::editAttributeValue(instanceData->type, instanceData);
        instanceData->isDefault = false;
    }
}

void DatabaseFile::addFileDictionaryClass(){
    bool isDialogOpen = true;
    CustomPopup* dialogGetClassName = ProgWindow::makeSpecificPopup(isDialogOpen, {"combobox"}, {"Class:"});
    dialogGetClassName->setWindowTitle("Choose class to add");

    bool alreadyIncluded = false;
    for(int i = 0; i < inheritedFile->dictionary.size(); i++){
        for(int j = 0; j < dictionary.size(); j++){
            if(inheritedFile->dictionary[i].name == dictionary[j].name){
                alreadyIncluded = true;
            }
        }
        if(!alreadyIncluded){
            dialogGetClassName->comboOption->addItem(inheritedFile->dictionary[i].name);
        }
        alreadyIncluded = false;
    }

    dialogGetClassName->open();
    while(isDialogOpen){
        ProgWindow::forceProcessEvents();
    }
    int resultDialog = dialogGetClassName->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }

    int sectionIndex = dictionary.size();
    dictionary.resize(sectionIndex+1);

    QString chosenClassName = dialogGetClassName->comboOption->currentText();
    dictionary[sectionIndex].name = chosenClassName;

    addFileDictionaryAttributes(chosenClassName);
}

void DatabaseFile::addFileDictionaryAttributes(QString chosenClassName){
    dictItem* inheritClass = nullptr;

    for(int i = 0; i < inheritedFile->dictionary.size(); i++){
        if(inheritedFile->dictionary[i].name == chosenClassName){
            inheritClass = &inheritedFile->dictionary[i];
        }
    }

    if(inheritClass == nullptr){
        return;
    }

    int sectionIndex = 0;
    for(int i = 0; i < dictionary.size(); i++){
        if(dictionary[i].name == chosenClassName){
            sectionIndex = i;
        }
    }

    bool isDialogOpen = true;
    CustomPopup *dialogGetClassName = ProgWindow::makeSpecificPopup(isDialogOpen, {"list"}, {"Select Attributes:"});
    dialogGetClassName->setWindowTitle("Choose attributes to use");
    bool alreadyIncluded = false;
    for(int i = 0; i < inheritClass->attributes.size(); i++){
        for(int j = 0; j < dictionary[sectionIndex].attributes.size(); j++){
            if(inheritClass->attributes[i]->name == dictionary[sectionIndex].attributes[j]->name){
                alreadyIncluded = true;
            }
        }
        QListWidgetItem* possibleAttribute = new QListWidgetItem(inheritClass->attributes[i]->name, dialogGetClassName->listOption);
        possibleAttribute->setFlags(possibleAttribute->flags() | Qt::ItemIsUserCheckable);
        if(!alreadyIncluded){
            possibleAttribute->setCheckState(Qt::Unchecked);
        } else {
            //might remove this - if it's included and used, then removing it is a bad idea.
            possibleAttribute->setCheckState(Qt::Checked);
        }
        dialogGetClassName->listOption->addItem(possibleAttribute);
        alreadyIncluded = false;
    }

    isDialogOpen = true;
    dialogGetClassName->open();
    while(isDialogOpen){
        ProgWindow::forceProcessEvents();
    }
    int resultDialog = dialogGetClassName->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }


    for(int i = 0; i < dialogGetClassName->listOption->count(); i++){
        for(int j = 0; j < inheritClass->attributes.size(); j++){
            if(dialogGetClassName->listOption->item(i)->text() == inheritClass->attributes[j]->name && dialogGetClassName->listOption->item(i)->checkState()){
                dictionary[sectionIndex].attributes.push_back(inheritClass->attributes[j]->clone());
            }
        }
    }
}

void DatabaseFile::addNewInstance(){
    bool isDialogOpen = true;
    CustomPopup* dialogGetClassName = ProgWindow::makeSpecificPopup(isDialogOpen, {"combobox"}, {"Instance:"});
    dialogGetClassName->setWindowTitle("Choose instance type to add");

    for(int i = 0; i < dictionary.size(); i++){
        dialogGetClassName->comboOption->addItem(dictionary[i].name);
    }

    dialogGetClassName->open();
    while(isDialogOpen){
        ProgWindow::forceProcessEvents();
    }
    int resultDialog = dialogGetClassName->result();

    if(resultDialog == 0){
        qDebug() << Q_FUNC_INFO << "Process cancelled.";
        return;
    }

    int instanceSize = instances.size();
    instances.resize(instanceSize+1);
    instances[instanceSize].instanceIndex = maxInstances + 1;
    maxInstances++;

    QString chosenInstanceName = dialogGetClassName->comboOption->currentText();
    instances[instanceSize].name = chosenInstanceName;
    for(int i = 0; i < dictionary.size(); i++){
        if(dictionary[i].name == chosenInstanceName){
            for(int j = 0; j < dictionary[i].attributes.size(); j++){
                instances[instanceSize].attributes.push_back(dictionary[i].attributes[j]->clone());
            }
        }
    }

}
