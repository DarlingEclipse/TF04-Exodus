#include "Headers/Main/mainwindow.h"

/*
The overall plan with VBIN files is to read through them section by section. The previous code looked for specific sections (positionarrays and indexarrays)
and used those to build the mesh, but this is a cheap shortcut that prevented progress for quite some time.
Reading the file in order, as you do with files, means it'll be easier to track down how mesh and node offsets are handled.
*/

VBIN::VBIN(){
    highestLOD = 0;
    fileExtension = "VBIN";
}

void SceneNode::clear(){
    headerData.clear();
    mods.clear();
    sectionTypes.clear();
    sectionList.clear();
    sectionEnd = 0;

    return;
}

void Modifications::clear(){
    modByte = 0;
    offset = QVector3D();
    rotation = QQuaternion();
    scale = 1;
    return;
}

void BoundingVolume::populateData(){
    fileData->currentPosition = headerData.sectionLocation + 25;
    hasVolume = fileData->readInt(1);
    type = fileData->readInt();
    center.setX(fileData->readFloat());
    center.setY(fileData->readFloat());
    center.setZ(fileData->readFloat());
    radius = fileData->readFloat();
    qDebug() << Q_FUNC_INFO << "location" << headerData.sectionLocation << "radius" << radius << "hasvolume" << hasVolume << "type" << type;
}

int VBIN::readDataVBIN(){

    //gets all scene nodes in the file, populates their PositionArrays and IndexArrays, and gets modifications
    currentLocation = 0;
    base.file = this;
    //base.getSceneNodeTree(0, parent->fileData.dataBytes.length(), 0);
    if(getSceneNodeTree()){
        qDebug() << Q_FUNC_INFO << "Error while reading scene node tree";
        return 1;
    }

    //base.printInfo(0);
    //std::vector<Modifications> addedMods;
    //base.modifyPosArrays(addedMods);

    //parent->createLevelList(highestLOD);

    selectedLOD = highestLOD;

    qDebug() << Q_FUNC_INFO << "section list size" << base.sectionList.size() << "mesh list size" << base.meshList.size();

    qDebug() << Q_FUNC_INFO << "Successfully loaded the scene node tree.";

    return 0;
}

void FileSection::printInfo(int depth){
    //qDebug() << Q_FUNC_INFO << name << "section list size" << sectionList.size() << "mesh list size" << meshList.size();
    //qDebug() << Q_FUNC_INFO << "file name" << file->fileName;
    if(depth > 0){
        qDebug() << Q_FUNC_INFO << "depth" << depth << "section name" << headerData.name << "with parent" << parent->headerData.name;
    }
    for(int i = 0; i < sectionList.size(); i++){
        sectionList[i]->printInfo(depth+1);
    }
    for(int i = 0; i < meshList.size(); i++){
        //qDebug() << Q_FUNC_INFO << meshList[i]->name << "position list has" << meshList[i]->vertexSet.positionArray.positionList.size();
        meshList[i]->printInfo(depth+1);
    }
}

/*void FileSection::modifyPosArrays(std::vector<Modifications> addedMods){
    QMatrix3x3 rotMatrix;
    Mesh modifyMesh;
    SceneNode modifyNode;
    if(headerData.name != "vbinbase"){
        addedMods.push_back(mods);
        //qDebug() << Q_FUNC_INFO << "Getting modifications for " << name << " current scale: " << mods.scale << "passed mods" << addedMods[addedMods.size()-1].scale;
    }

    for (int i = 0; i < int(meshList.size()); i++) {
        meshList[i]->modifyPosArrays(addedMods);
        meshList[i]->modify(addedMods);
    }
    for(int i = 0; i < int(sectionList.size()); i++){
        sectionList[i]->modifyPosArrays(addedMods);
    }
    return;
}*/

const void BoundingVolume::operator=(BoundingVolume input){
    headerData = input.headerData;
    hasVolume = input.hasVolume;
    type = input.type;
    center = input.center;
    radius = input.radius;
    fileData = input.fileData;
}

const void FileSection::operator=(FileSection input){
    file = input.file;
    headerData = input.headerData;
    boundVol = input.boundVol;
    mods = input.mods;
    sectionList = input.sectionList;
    sectionTypes = input.sectionTypes;
}


void VBIN::readAnimationPrototype(){
    SectionHeader signature;
    int testProto = parent->fileData.readInt();
    parent->fileData.currentPosition -= 4;
    if(testProto > 20){
        float unknownFloat1 = parent->fileData.readFloat();
        int unknown4Byte4 = parent->fileData.readInt();
        int unknown4Byte5 = parent->fileData.readInt();

        fileData->signature(&signature);
        if(signature.type == "NeutralData"){
            parent->fileData.currentPosition = signature.sectionLength + signature.sectionLocation;
        }
    }
}

void VBIN::readBoundingVolume(SectionHeader* signature){
    //qDebug() << Q_FUNC_INFO << "current position 1" << parent->fileData.currentPosition;
    parent->fileData.currentPosition = signature->sectionLength + signature->sectionLocation;
    //qDebug() << Q_FUNC_INFO << "current position 2" << parent->fileData.currentPosition;

    fileData->signature(signature);
    //this is such a bad way to do this but w/e
    if(signature->type == "BoundingVolume"){
        parent->fileData.currentPosition = signature->sectionLength + signature->sectionLocation;
    }
}

void FileSection::readModifications(){
    int offsets = 0;
    float m_value = 0;
    //offset properties
    offsets = fileData->readInt(1);
    //qDebug() << Q_FUNC_INFO << "offsets read as" << offsets << "at" << fileData->currentPosition;
    if (!(offsets & 1)) {
        //position offset
        mods.offset = fileData->read3DVector();
    } else {
        mods.offset = QVector3D(0,0,0);
    }
    if (!(offsets & 2)) {
        //rotation offset
        mods.rotation = fileData->readQuaternion();
    } else {
        mods.rotation = QQuaternion(1,0,0,0);
    }
    if (!(offsets & 4)) {
        //scale offset
        m_value = fileData->readFloat();
        mods.scale = m_value;
    } else {
        mods.scale = 1;
    }
}

void VBIN::readModifications(){
    //This only exists for the AnimationPrototype section
    //eventually AnimationPrototype needs to be a filesection like the rest
    int offsets = 0;
    //offset properties
    offsets = parent->fileData.readInt(1);
    //qDebug() << Q_FUNC_INFO << "offsets read as" << offsets;
    if (!(offsets & 1)) {
        //position offset
        parent->fileData.currentPosition += 12;
    }
    if (!(offsets & 2)) {
        //rotation offset
        parent->fileData.currentPosition += 16;
    }
    if (!(offsets & 4)) {
        //scale offset
        parent->fileData.currentPosition += 4;
    }
}

void FileSection::readNode(){

}

bool FileSection::meshListContains(QString checkName){
    for(int i = 0; i<meshList.size(); i++){
        //qDebug() << Q_FUNC_INFO << "comparing name" << checkName << "against" << meshList[i]->headerData.name;
        if(meshList[i]->headerData.name == checkName){
            //qDebug() << Q_FUNC_INFO << "names" << checkName << "and" << meshList[i]->headerData.name << "matched, returning true";
            return true;
        } else if (meshList[i]->meshListContains(checkName)){
            return true;
        }
    }
    for(int i = 0; i<sectionList.size(); i++){
        if(sectionList[i]->meshListContains(checkName)){
            return true;
        }
    }
    return false;
}

int VBIN::getSceneNodeTree(){
    fileData = &parent->fileData;
    fileData->currentPosition = 4; //moving past FISH
    //not a huge fan of while(1)'s but I'll put a loopbreaker in just to be safe
    int loopBreaker = 0;
    SectionHeader signature;
    int unknown4Byte1 = 0; //appears to be a version number
    int unknown4Byte2 = 0;
    int unknown4Byte3 = 0;
    int nameAdjust = 0;
    FileSection* currentBranch;
    FileSection* possibleBranch;
    int elementCount = 0; //this is used to interface graph.vbins with mesh.vbins
    meshCount = 0;

    base.headerData = signature;
    base.headerData.name = "vbinbase";
    currentBranch = &base;
    possibleBranch = &base;

    qDebug() << Q_FUNC_INFO << "section" << signature.name << "of type" << signature.type <<"is" << signature.sectionLength << "bytes long at" << signature.sectionLocation;

    while(1){

        if (parent->fileData.currentPosition == base.sectionEnd){
            qDebug() << Q_FUNC_INFO << "Reached end of tree. Exiting now.";
            return 0;
        }

        //Checks if we've reached the end of a branch in the scene node tree. If we're at the root of the tree, there's nowhere to go.
        if(currentBranch->sectionEnd == parent->fileData.currentPosition && currentBranch->headerData.sectionLocation !=0){
            while(currentBranch->sectionEnd == currentBranch->parent->sectionEnd){
                currentBranch = currentBranch->parent;
                //qDebug() << Q_FUNC_INFO << "Branch completed, moving back up a layer";
            }
            currentBranch = currentBranch->parent;
        } else if (possibleBranch->sectionEnd != parent->fileData.currentPosition) {
            currentBranch = possibleBranch;
        }

        fileData->signature(&signature);


        qDebug() << Q_FUNC_INFO << "section" << signature.name << "of type" << signature.type <<"is" << signature.sectionLength << "bytes long at" << signature.sectionLocation;

        if (base.headerData.sectionLength == 0) {
            base.headerData.sectionLength = signature.sectionLength;
            base.sectionEnd = parent->fileData.currentPosition -4 + signature.sectionLength;
        }

        if(signature.type == "Mesh"){
            Mesh *meshSection = new Mesh();
            meshSection->file = this; //file will need to be reassigned later as the current VBIN object is temporary
            meshSection->fileData = &parent->fileData;
            meshSection->parent = currentBranch;
            meshSection->headerData = signature;
            meshSection->sectionEnd = signature.sectionLength + signature.sectionLocation;
            meshSection->elementOffset = elementCount;
            meshCount++;

            //This exists to remove duplicate mesh names. This is necessary for DAE outputs
            //an arguement could be made that this should only be done for the DAE output, but it's easier to just do it when we read the file in the first place.
            if(base.meshListContains(meshSection->headerData.name)){
                nameAdjust++;
                meshSection->headerData.name = meshSection->headerData.name + "_" + QString::number(nameAdjust);
            }

//            if (signature.name == "CollisionMesh") {
//                parent->messageError("It looks like you're trying to read a Collision Mesh. "
//                                     "These models have weird stuff going on and are not currently compatible with this program.");
//                return 1;
//            }

            if(meshSection->readMesh()){
                qDebug() << "Error while reading mesh";
                return 1;
            }
            elementCount += meshSection->elementCount;
            unknown4Byte1 = parent->fileData.readInt();
            meshSection->readModifications();
            //qDebug() << Q_FUNC_INFO << "mesh offsets: scale" << meshSection->mods.scale << "offset" << meshSection->mods.offset << "rotation" << meshSection->mods.rotation;

            currentBranch->meshList.push_back(meshSection);
            possibleBranch = meshSection;
            //qDebug() << Q_FUNC_INFO << "name " << meshSection->name << "with parent" << meshSection->parent->name << "finished at" << fileData->currentPosition;
        }

        if(signature.type == "SceneNode"){
            SceneNode *sceneSection = new SceneNode();
            sceneSection->file = this;
            sceneSection->fileData = &parent->fileData;
            sceneSection->parent = currentBranch;
            sceneSection->headerData = signature;
            sceneSection->sectionEnd = signature.sectionLength + signature.sectionLocation;
            unknown4Byte1 = parent->fileData.readInt(); //appears to be a version number of some kind
            sceneSection->readModifications();
            currentBranch->sectionList.push_back(sceneSection);
            possibleBranch = sceneSection;
            //qDebug() << Q_FUNC_INFO << "name " << sceneSection->name << "with parent" << sceneSection->parent->name;
        }

        if(signature.type == "Instance"){
            Instance *instanceSection = new Instance();
            instanceSection->file = this;
            instanceSection->fileData = &parent->fileData;
            instanceSection->parent = currentBranch;
            instanceSection->headerData = signature;
            instanceSection->sectionEnd = signature.sectionLength + signature.sectionLocation;
            instanceSection->unknownValue = fileData->readInt();
            int referenceLength = fileData->readInt();
            instanceSection->modelReference = QString(fileData->readHex(referenceLength));
            if(!instanceNameList.contains(instanceSection->modelReference)){
                instanceNameList.push_back(instanceSection->modelReference);
            }

            unknown4Byte1 = parent->fileData.readInt(); //appears to be a version number of some kind
            instanceSection->readModifications();
            currentBranch->sectionList.push_back(instanceSection);
            possibleBranch = instanceSection;
            //qDebug() << Q_FUNC_INFO << "name " << sceneSection->name << "with parent" << sceneSection->parent->name;
        }

        if(signature.type == "vlCellManager"){
            CellManager *cellSection = new CellManager();
            cellSection->file = this;
            cellSection->fileData = &parent->fileData;
            cellSection->parent = currentBranch;
            cellSection->headerData = signature;
            cellSection->sectionEnd = signature.sectionLength + signature.sectionLocation;

            fileData->signature(&signature); //should read "~CellManager"
            //if we wanted to entirely skip the cellmanager data without losing the rest of the file, we could use this signature's section length
            //but might as well read the portals and skip each of those

            cellSection->unknownValue1 = fileData->readInt();
            cellSection->portalCount = fileData->readInt();

            fileData->signature(&signature); //should read "~Portals"
            cellSection->readPortals();

            unknown4Byte1 = parent->fileData.readInt(); //appears to be a version number of some kind
            cellSection->readModifications();
            currentBranch->sectionList.push_back(cellSection);
            possibleBranch = cellSection;
        }

        if(signature.type == "vlCell"){
            CellManager *cellSection = new CellManager();
            cellSection->file = this;
            cellSection->fileData = &parent->fileData;
            cellSection->parent = currentBranch;
            cellSection->headerData = signature;
            cellSection->sectionEnd = signature.sectionLength + signature.sectionLocation;

            cellSection->readCell();
            /*fileData->signature(&signature); //should read "~Cell"
            fileData->currentPosition = signature.sectionLocation + signature.sectionLength; //just skipping the cell data itself, for now*/


            unknown4Byte1 = parent->fileData.readInt(); //appears to be a version number of some kind
            cellSection->readModifications();
            currentBranch->sectionList.push_back(cellSection);
            possibleBranch = cellSection;
        }

        if(signature.type == "vlLODSwitcher"){
            vlLodSwitcher *lodSwitcher = new vlLodSwitcher();
            lodSwitcher->file = this;
            lodSwitcher->fileData = &parent->fileData;
            lodSwitcher->parent = currentBranch;
            lodSwitcher->headerData = signature;
            lodSwitcher->sectionEnd = signature.sectionLength + signature.sectionLocation;
            parent->log("vlLODSwitcher found in file. Export will not be accurate. Section name: " + signature.name + " | " + QString(Q_FUNC_INFO));
            fileData->signature(&signature); //gets data for LODSwitcher section
            fileData->currentPosition += 4;
            qDebug() << Q_FUNC_INFO << "vllodswitcher 1" << signature.name << "of type" << signature.type <<"is" << signature.sectionLength << "bytes long at" << signature.sectionLocation;
            fileData->signature(&signature); //gets data for levelmasks
            qDebug() << Q_FUNC_INFO << "vllodswitcher 2" << signature.name << "of type" << signature.type <<"is" << signature.sectionLength << "bytes long at" << signature.sectionLocation;
            /*qDebug() << Q_FUNC_INFO << "comparing levelmasks section length" << signature.sectionLength <<"to lodswitcher section end" << lodSwitcher->sectionEnd <<". currently at" << fileData->currentPosition;
            if(signature.sectionLength > lodSwitcher->sectionEnd){
                signature.sectionLength = lodSwitcher->sectionEnd - fileData->currentPosition - 16;
            }*/
            //skip to end of section
            fileData->currentPosition = signature.sectionLength + signature.sectionLocation;
            unknown4Byte1 = parent->fileData.readInt(); //appears to be a version number of some kind
            lodSwitcher->readModifications();
            currentBranch->sectionList.push_back(lodSwitcher);
        }

        if(signature.type == "anAnimationPrototype"){
            readAnimationPrototype();
            unknown4Byte1 = parent->fileData.readInt();
            readModifications();
        }

        if(!knownSections.contains(signature.type)){
            /*unknown types include:
            
            (found in geometry.vbin for Cybertron only)
            taCybertronUnicronLocationManager
                CybertronUnicronLocationManager
            taCybertronTowerManager
                CybertronTowerManager
                Towers
                Tower
            
            anAnimationActor
            anAnimationSourceSet
            
            (found in geometry.vbin/geometry.graph.vbin files and tower.vbin files for cybertron so far)
            vlLODSwitcher
                LODSwitcher
                LevelMasks
                
            (found in geometry.vbin/geometry.graph.vbin files)
            vlCellManager
                Portals
                vlCell
                Cell 
                ExcludedCells
            Instance
            taOcclusionNodeMapMarker
                taOcclusionMap
                taCompressedShadowMap
                
            (found in ecocollection.vbin)
            ecoCollection
                ecoSecondaryCollection
                ecoSecondaryObjectLayout
                SecondaryObjectMesh
                ecoQuadtree
                ecoSpriteGroupArray
                ecoSpriteGroup
                ecoQuadtreeNode
                ChildNodes
                
            (found in buildingplacement.vbin)
            InformationNode
                IntegerDictionary
                StringDictionary
                
            (found in ecoglobe.vbin)
            ecoGlobe
                ecoGlobe_TriangleSet
                TriangleTypeArray
                ecoGlobePatchArray
                ecoGlobePatch
            
            */
            parent->log("Found section of " + signature.type + " at " + QString::number(parent->fileData.currentPosition) + ". These are not currently handled. Data will be skipped. Length:" + QString::number(signature.sectionLength) + " | " + QString(Q_FUNC_INFO));
            //qDebug() << Q_FUNC_INFO << "Found" << signature.type << "at" << parent->fileData.currentPosition << "- skipping for now";
            parent->fileData.currentPosition = signature.sectionLength + signature.sectionLocation;
            qDebug() << Q_FUNC_INFO << " position after skip " << parent->fileData.currentPosition;
            continue;
        }

        if(signature.type == "anAnimationSourceSet"){
            qDebug() << Q_FUNC_INFO << "Reading animation data.";
            //qDebug() << Q_FUNC_INFO << "End of tree located - leaving at Animation Data";
            //break;
//            animationSet.file = this;
//            animationSet.fileLocation = parent->fileData.currentPosition-4;
//            animationSet.sectionLength = signature.sectionLength;
//            animationSet.sectionEnd = animationSet.fileLocation + signature.sectionLength;
//            animationSet.name =signature.name;
//            animationSet.readAnimationSet();
            qDebug() << Q_FUNC_INFO << "Found animation data at " << parent->fileData.currentPosition << "- skipping for now";
            parent->fileData.currentPosition = signature.sectionLength + signature.sectionLocation;
            qDebug() << Q_FUNC_INFO << "position after skip" << parent->fileData.currentPosition;
            continue; //continuing after reading animation, the bounding volume is contained in them.
        }

        //readModifications();
        unknown4Byte2 = parent->fileData.readInt();
        if (unknown4Byte1 > 3) {
            parent->fileData.currentPosition += 8;
            unknown4Byte3 = parent->fileData.readInt(); //this should take us to the end
        }

        fileData->signature(&signature);

        if(signature.type == "BoundingVolume"){
            readBoundingVolume(&signature);
        }
        //qDebug() << Q_FUNC_INFO << "Expected end of section at location: " << parent->fileData.currentPosition << "with loops" << loopBreaker;

        loopBreaker++;
        if (loopBreaker > 5000 or parent->fileData.currentPosition > base.sectionEnd){
            qDebug() << Q_FUNC_INFO << "Excessive looping detected or node tree exceeded. loopbreaker:" << loopBreaker << "current position:" << parent->fileData.currentPosition;
            parent->messageError("Excessive looping detected or node tree exceeded for file " + fileName);
            return 1;
        }

        if (parent->fileData.currentPosition == base.sectionEnd){
            qDebug() << Q_FUNC_INFO << "Reached end of tree. Exiting now.";
            qDebug() << Q_FUNC_INFO << "file contans" << meshCount << "meshes";
            qDebug() << Q_FUNC_INFO << "texture list contains" << textureNameList.size() << "textures";
            return 0;
        }

        //qDebug() << Q_FUNC_INFO << loopBreaker << "loops completed";

    }

    return 0; //read successfully
}

void VBIN::updateCenter(){
    QComboBox* ListLods = new QComboBox(parent->centralContainer);
    ListLods -> setGeometry(QRect(QPoint(250,150), QSize(150,30)));
    if (highestLOD <= 0){
        ListLods->insertItem(0, "1");
        highestLOD = 1;
    } else {
        for(int i=0; i<highestLOD; ++i){
            ListLods->insertItem(i, QString::number(i+1));
        }
    }

    QComboBox::connect(ListLods, &QComboBox::currentIndexChanged, parent, [ListLods, this] {setLevel(ListLods->currentIndex());});
    //QAbstractButton::connect(ListLods, &QComboBox::currentIndexChanged, parent, [parent = this->parent]() {parent->levelSelectChange();});
    ListLods->show();
    parent->currentModeWidgets.push_back(ListLods);
    ListLods->setCurrentIndex(highestLOD-1);

    QRadioButton* radioSingle = new QRadioButton("Single file output", parent->centralContainer);
    radioSingle->setGeometry(QRect(QPoint(340,120), QSize(200,30)));
    radioSingle->setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0)");
    QAbstractButton::connect(radioSingle, &QRadioButton::toggled, parent, [radioSingle, this] {setOutput(radioSingle->isChecked());});
    radioSingle->toggle();
    radioSingle->show();
    parent->currentModeWidgets.push_back(radioSingle);


    QRadioButton* radioMultiple = new QRadioButton("Multi-file output", parent->centralContainer);
    radioMultiple->setGeometry(QRect(QPoint(540,120), QSize(120,30)));
    radioMultiple->setStyleSheet("color: rgb(255, 255, 255); background-color: rgba(255, 255, 255, 0)");
    radioMultiple->show();
    parent->currentModeWidgets.push_back(radioMultiple);

}

void VBIN::setOutput(bool single){
    singleOutput = single;
}

void VBIN::setLevel(int level){
    selectedLOD = level;
}

void VBIN::save(QString toType){
    if(toType == "STL"){
        outputDataSTL();
    } else if (toType == "DAE"){
        outputDataDAE();
    }
}

void VBIN::save(QString toType, QTextStream &stream){
    if (toType == "Effect"){
        writeEffectListDAE(stream);
    } else if (toType == "Image"){
        writeImageListDAE(stream);
    } else if (toType == "Material"){
        writeMaterialListDAE(stream);
    } else if (toType == "Geometry"){
        writeGeometryListDAE(stream);
    } else if (toType == "Animation"){
        writeAnimationListDAE(stream);
    } else if (toType == "Node"){
        writeNodeListDAE(stream);
    }
}

void VBIN::load(QString fromType){
    int failedRead = 0;
    if(fromType == "VBIN" or fromType == "GRAPH.VBIN"){
        failedRead = readDataVBIN();
    } else {
        failedRead = 1;
    }
    if(failedRead){
        parent->messageError("There was an error reading " + fileName);
        return;
    }
}

//void ProgWindow::convertVBINToSTL(){
//    if(vbinFiles.empty()){
//        messageError("No VBIN files currently loaded to export.");
//        return;
//    }

//    vbinFiles[ListFiles->currentIndex()].outputDataSTL();

//    return;
//}

//void ProgWindow::convertVBINToDAE(){
//    if(vbinFiles.empty()){
//        messageError("No VBIN files currently loaded to export.");
//        return;
//    }

//    vbinFiles[ListFiles->currentIndex()].outputDataDAE();

//    return;
//}

void FileSection::writeSectionListSTL(QTextStream &fileOut){
    //for writing to a single file
    for (int i = 0; i < int(meshList.size()); i++) {
        meshList[i]->file = file;
        meshList[i]->writeDataSTL(fileOut);
        meshList[i]->writeSectionListSTL(fileOut);
    }
    for(int i = 0; i < int(sectionList.size()); i++){
        sectionList[i]->file = file;
        sectionList[i]->writeSectionListSTL(fileOut);
    }
}

void FileSection::writeSectionListSTL(QString path){
    //for writing to multiple files

    for (int i = 0; i < int(meshList.size()); i++) {
        QString multiFileOut = path + "/" + meshList[i]->headerData.name + ".stl";
        QFile stlOut(multiFileOut);
        QFile fileOut(multiFileOut);
        fileOut.open(QFile::WriteOnly|QFile::Truncate);
        fileOut.close();

        if (!stlOut.open(QIODevice::ReadWrite)){
            continue;
        }
        QTextStream stream(&stlOut);
        stream << "solid Default" << Qt::endl;

        meshList[i]->file = file;
        meshList[i]->writeDataSTL(stream);
        stream << "endsolid Default" << Qt::endl;
        meshList[i]->writeSectionListSTL(path);
    }
    for(int i = 0; i < int(sectionList.size()); i++){
        sectionList[i]->file = file;
        sectionList[i]->writeSectionListSTL(path);
    }
}

void VBIN::outputDataSTL(){

    std::vector<int> allowedMeshes = {1,2,3};
    std::vector<int> chosenLOD;
    base.file = this;

    //applyKeyframe();

    if(singleOutput){
//        QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output STL"), QDir::currentPath() + "/STL/", parent->tr("Model Files (*.stl)"));
//        if(fileOut.isEmpty()){
//            parent->messageError("STL export cancelled.");
//        }
        QFile stlOut(outputPath);
        QFile file(outputPath);
        file.open(QFile::WriteOnly|QFile::Truncate);
        file.close();

        if(!stlOut.open(QIODevice::ReadWrite)){
            return;
        }
        QTextStream stream(&stlOut);
        stream << "solid Default" << Qt::endl;
        qDebug() << Q_FUNC_INFO << "sections:" << base.sectionList.size();
        qDebug() << Q_FUNC_INFO << "meshes:" << base.meshList.size();
        base.writeSectionListSTL(stream);
        stream << "endsolid Default" << Qt::endl;
    } else {
        QString fileOut = QFileDialog::getExistingDirectory(parent, parent->tr("Select Output STL"), QDir::currentPath() + "/STL/", QFileDialog::ShowDirsOnly);
        if(fileOut.isEmpty()){
            parent->messageError("STL export cancelled.");
        }
        base.writeSectionListSTL(fileOut);
    }

    parent->messageSuccess("STL file saved.");
    qDebug() << Q_FUNC_INFO << "STL output complete.";

    return;
}

void VBIN::outputDataDAE(){
    base.file = this;

    //applyKeyframe();

//    QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output DAE"), QDir::currentPath() + "/DAE/", parent->tr("Model Files (*.dae)"));
//    if(fileOut.isEmpty()){
//        parent->messageError("DAE export cancelled.");
//        return;
//    }
    std::shared_ptr<TFFile> testLoaded;
    std::vector<std::shared_ptr<TFFile>> loadedInstances;
    for(int i = 0; i < instanceNameList.size(); i++){
        /*This should probably be moved to the loading side of things*/
        parent->loadRequiredFile(inputPath, instanceNameList[i], "VBIN");
        testLoaded = parent->matchFile(instanceNameList[i] + ".VBIN");
        /*if(testLoaded == nullptr){
            //putting this separate from the while - don't want to run this check multiple times if it won't do anything
            QFileInfo dir(inputPath);
            QString instancePath = dir.absolutePath() + "/" + instanceNameList[i] + ".VBIN";
            bool isFileInDirectory = QFileInfo::exists(instancePath);
            qDebug() << Q_FUNC_INFO << "file directory is" << dir.absolutePath() << "and file exists?" << isFileInDirectory;
            if(isFileInDirectory){
                parent->openFile("VBIN", instancePath);
            }
            testLoaded = parent->matchFile(instanceNameList[i] + ".VBIN");
        }
        while(testLoaded == nullptr){
            parent->messageError("Please load a file " + instanceNameList[i]+".VBIN");
            parent->openFile("VBIN");
            testLoaded = parent->matchFile(instanceNameList[i] + ".VBIN");
        }*/
        testLoaded->outputPath = outputPath;
        loadedInstances.push_back(testLoaded);
    }

    for(int i = 0; i < loadedInstances.size(); i++){
        QFileInfo dir(outputPath);
        QString instancePath = dir.absolutePath() + "/" + loadedInstances[i]->fileName + ".DAE";
        bool isFileInDirectory = QFileInfo::exists(instancePath);
        qDebug() << Q_FUNC_INFO << "file" << loadedInstances[i]->fileName << "has already been exported to this directory?" << isFileInDirectory;
        if(!isFileInDirectory){
            loadedInstances[i]->outputPath = instancePath;
            loadedInstances[i]->save("DAE");
        }
    }

    QFile daeOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if(!daeOut.open(QIODevice::ReadWrite)){
        parent->messageError("DAE export failed, could not open output file.");
        return;
    }
    QTextStream stream(&daeOut);
    //return;

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << Qt::endl;
    stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" << Qt::endl;

    stream << "  <asset>" << Qt::endl;
    stream << "    <contributor>" << Qt::endl;
    stream << "      <author>PrincessTrevor</author>" << Qt::endl;
    stream << "      <authoring_tool>Exodus v" << QString::number(parent->version) << "</authoring_tool>" << Qt::endl;
    stream << "    </contributor>" << Qt::endl;
    stream << "    <created>" << QDateTime::currentDateTime().toString("yyyy-MM-dd") + "T" + QDateTime::currentDateTime().toString("hh:mm:ss") << "</created>" << Qt::endl;
    stream << "    <modified>" << QDateTime::currentDateTime().toString("yyyy-MM-dd") + "T" + QDateTime::currentDateTime().toString("hh:mm:ss") << "</modified>" << Qt::endl;
    stream << "    <unit name=\"meter\" meter=\"1\"/>" << Qt::endl;
    stream << "    <up_axis>Z_UP</up_axis>" << Qt::endl;
    stream << "  </asset>" << Qt::endl;

    stream << "  <library_effects>" << Qt::endl;
    /*for(int i = 0; i < instanceNameList.size(); i++){
        loadedInstances[i]->save("Effect", stream);
    }*/
    writeEffectListDAE(stream);
    //base.searchListsWriteDAE(stream, &Mesh::writeEffectsDAE);
    stream << "  </library_effects>" << Qt::endl;

    stream << "  <library_images>" << Qt::endl;
    /*for(int i = 0; i < instanceNameList.size(); i++){
        loadedInstances[i]->save("Image", stream);
    }*/
    writeImageListDAE(stream);
    stream << "  </library_images>" << Qt::endl;

    stream << "  <library_materials>" << Qt::endl;
    /*for(int i = 0; i < instanceNameList.size(); i++){
        loadedInstances[i]->save("Material", stream);
    }*/
    writeMaterialListDAE(stream);
    stream << "  </library_materials>" << Qt::endl;

    stream << "  <library_geometries>" << Qt::endl;
    /*for(int i = 0; i < instanceNameList.size(); i++){
        loadedInstances[i]->save("Geometry", stream);
    }*/
    writeGeometryListDAE(stream);
    //base.searchListsWriteDAE(stream, &Mesh::writeDataDAE);
    //base.writeSectionListDAE(stream);
    stream << "  </library_geometries>" << Qt::endl;

    stream << "  <library_animations>" << Qt::endl;
    writeAnimationListDAE(stream);
    //animationSet.writeAnimationsDAE(stream);
    stream << "  </library_animations>" << Qt::endl;

    stream << "  <library_nodes>" << Qt::endl;
    /*for(int i = 0; i < instanceNameList.size(); i++){
        loadedInstances[i]->save("Node", stream);
    }*/
    stream << "  </library_nodes>" << Qt::endl;

    stream << "  <library_visual_scenes>" << Qt::endl;
    stream << "    <visual_scene id=\"Scene\" name=\"Scene\">" << Qt::endl;
    ////base.searchListsWriteDAE(stream, &Mesh::writeNodesDAE);
    writeNodeListDAE(stream);
    //base.writeNodes(stream);
    ////base.writeSceneListDAE(stream);
    stream << "    </visual_scene>" << Qt::endl;
    stream << "  </library_visual_scenes>" << Qt::endl;

    stream << "  <scene>" << Qt::endl;
    stream << "    <instance_visual_scene url=\"#Scene\"/>" << Qt::endl;
    stream << "  </scene>" << Qt::endl;

    stream << "</COLLADA>" << Qt::endl;

    qDebug() << Q_FUNC_INFO << "DAE output complete.";
}

void VBIN::writeEffectListDAE(QTextStream &stream){
    base.searchListsWriteDAE(stream, &Mesh::writeEffectsDAE);
}

void VBIN::writeImageListDAE(QTextStream &stream){
    QString textureName;
    for (int texture = 0; texture < textureNameList.size(); texture++) {
        textureName = textureNameList[texture];
        stream << "    <image id=\"" + textureName +"_png\" name=\"" + textureName +"_png\">" << Qt::endl;
        stream << "      <init_from>" + textureName +".png</init_from>" << Qt::endl;
        stream << "    </image>" << Qt::endl;
    }
}

void VBIN::writeMaterialListDAE(QTextStream &stream){
    QString textureName;
    for (int material = 0; material < textureNameList.size(); material++) {
        textureName = textureNameList[material];
        stream << "    <material id=\"" + textureName +"Texture-material\" name=\"" + textureName + "Texture\">" << Qt::endl;
        stream << "      <instance_effect url=\"#" + textureName + "Texture-effect\"/>" << Qt::endl;
        stream << "    </material>" << Qt::endl;
    }
}

void VBIN::writeGeometryListDAE(QTextStream &stream){
    base.searchListsWriteDAE(stream, &Mesh::writeDataDAE);
}

void VBIN::writeAnimationListDAE(QTextStream &stream){
    animationSet.writeAnimationsDAE(stream);
}

void VBIN::writeNodeListDAE(QTextStream &stream){
    base.writeNodes(stream);
}

void VBIN::applyKeyframe(){
    qDebug() << Q_FUNC_INFO << "current animation index" << parent->ListAnimation->currentIndex() << "with current frame index" << parent->ListFrame->currentIndex();
    QString chosenType;

    QVector3D translation;
    QQuaternion rotation;
    QString channelName;
    for (int channel = 0; channel < animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray.size(); channel++) {
        chosenType = animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray[channel]->animationType;
        if(chosenType == "~anAnimationTranslation"){
            translation = animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray[channel]->vectorList[parent->ListFrame->currentIndex()];
            channelName = animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray[channel]->name;
            base.sendKeyframe(translation, channelName);
        } else if (chosenType == "~anAnimationOrientation"){
            rotation = animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray[channel]->rotationList[parent->ListFrame->currentIndex()];
            channelName = animationSet.streamArray[parent->ListAnimation->currentIndex()]->channelArray[channel]->name;
            base.sendKeyframe(rotation, channelName);
        }
        qDebug() << Q_FUNC_INFO << "animation type" << chosenType << "is not currently supported";
        return;
    }
}

void FileSection::sendKeyframe(QVector3D keyOffset, QString channelName){
    for(int mesh = 0; mesh < meshList.size(); mesh++){
        if(meshList[mesh]->headerData.name == channelName){
            meshList[mesh]->applyKeyframe(keyOffset);
        }
        meshList[mesh]->sendKeyframe(keyOffset, channelName);
    }
    for(int section = 0; section < sectionList.size(); section++){
        sectionList[section]->sendKeyframe(keyOffset, channelName);
    }
}

void FileSection::sendKeyframe(QQuaternion keyRotation, QString channelName){
    for(int mesh = 0; mesh < meshList.size(); mesh++){
        if(meshList[mesh]->headerData.name == channelName){
            meshList[mesh]->applyKeyframe(keyRotation);
        }
        meshList[mesh]->sendKeyframe(keyRotation, channelName);
    }
    for(int section = 0; section < sectionList.size(); section++){
        sectionList[section]->sendKeyframe(keyRotation, channelName);
    }
}

void FileSection::modify(std::vector<Modifications> addedMods){
    qDebug() << Q_FUNC_INFO << "THIS SHOULDN'T RUN";
}
