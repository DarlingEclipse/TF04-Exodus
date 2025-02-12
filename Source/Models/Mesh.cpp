#include "Headers/Main/mainwindow.h"

void LODInfo::clear(){
    levels= 0;
    targetIndecies.clear();
    fileLocation = 0;
    fileData = nullptr;
}

void PositionArray::clear(){
    arrayID = 0;
    vertexCount = 0;
    fileLocation = 0;
    meshName = "";
    positionList.clear();
    fileData = nullptr;
}

void Mesh::clear(){
    headerData.clear();
    vertexSet.positionArray.clear();
    elementArray.lodInfo.clear();
    mods.clear();
    sectionList.clear();
    sectionTypes.clear();
    file = nullptr;
    fileData = nullptr;
}

const void Mesh::operator=(Mesh input){
    headerData = input.headerData;
    vertexSet.positionArray = input.vertexSet.positionArray;
    mods = input.mods;
    file = input.file;
    sectionList = input.sectionList;
    sectionTypes = input.sectionTypes;
    elementArray.lodInfo = input.elementArray.lodInfo;
    elementArray.lodInfo.fileData = input.elementArray.lodInfo.fileData;
    elementArray.lodInfo.fileLocation = input.elementArray.lodInfo.fileLocation;
    elementArray.lodInfo.levels = input.elementArray.lodInfo.levels;
    elementArray.lodInfo.targetIndecies = input.elementArray.lodInfo.targetIndecies;
}

void Material::debug(){
//    qDebug() << Q_FUNC_INFO << "sectionLength" << sectionLength;
//    qDebug() << Q_FUNC_INFO << "version" << version;
//    qDebug() << Q_FUNC_INFO << "nameLength" << nameLength;
//    qDebug() << Q_FUNC_INFO << "name" << name;
//    qDebug() << Q_FUNC_INFO << "diffuse" << diffuse;
//    qDebug() << Q_FUNC_INFO << "specular" << specular;
//    qDebug() << Q_FUNC_INFO << "ambient" << ambient;
//    qDebug() << Q_FUNC_INFO << "specularPower" << specularPower;
}

const void PositionArray::operator=(PositionArray input){
    arrayID = input.arrayID;
    fileData = input.fileData;
    meshName = input.meshName;
    positionList = input.positionList;
    vertexCount = input.vertexCount;
    fileLocation = input.fileLocation;
}

void CellManager::readPortals(){
    qDebug() << Q_FUNC_INFO << "Portal section started reading at" << fileData->currentPosition << "with" << portalCount << "portals";
    SectionHeader signature;
    for(int i = 0; i < portalCount; i++){
        Portal *currentPortal = new Portal;
        fileData->signature(&signature); //should read "Portal"
        currentPortal->possibleVersion = fileData->readInt();
        for(int i = 0; i < 4; i++){
            currentPortal->references.push_back(fileData->readInt(2));
        }
        currentPortal->unknownPoint = fileData->read3DVector();
        for(int i = 0; i < 5; i++){
            currentPortal->pointList.push_back(fileData->read3DVector());
        }
        currentPortal->unknownValue = fileData->readFloat();
        //fileData->currentPosition = signature.sectionLocation + signature.sectionLength;
        portalList.push_back(currentPortal);
    }
    qDebug() << Q_FUNC_INFO << "Portal section finished reading at" << fileData->currentPosition;
}

void CellManager::readCell(){
    SectionHeader signature;
    fileData->signature(&signature); //should read "Cell"
    qDebug() << Q_FUNC_INFO << "read section:" << signature.type;
    if(signature.type != "Cell"){
        file->parent->log(QString::number(fileData->currentPosition) + " | Found unexpected section. Expected: Cell. | " + QString(Q_FUNC_INFO));
    }
    Cell *currentCell = new Cell;
    currentCell->possibleVersion = fileData->readInt();
    currentCell->unknownShort = fileData->readInt(2);
    fileData->signature(&signature); //should be AABB
    for(int i = 0; i < 2; i++){
        currentCell->axisBounds.push_back(fileData->read3DVector());
    }
    qDebug() << Q_FUNC_INFO << "finished reading axis bounds at" << fileData->currentPosition << "with values" << currentCell->axisBounds;
    //fileData->currentPosition = signature.sectionLocation + signature.sectionLength;
    fileData->signature(&signature); //should be OBB
    for(int i = 0; i < 5; i++){
        currentCell->orientationBounds.push_back(fileData->read3DVector());
    }
    qDebug() << Q_FUNC_INFO << "finished reading orientation bounds at" << fileData->currentPosition << "with values" << currentCell->orientationBounds;
    //fileData->currentPosition = signature.sectionLocation + signature.sectionLength;

    int portalCount = fileData->readInt();
    fileData->signature(&signature); //should be Portals
    for(int i = 0; i < portalCount; i++){
        currentCell->portals.push_back(fileData->readInt());
    }
    int excludedCellCount = fileData->readInt();
    fileData->signature(&signature); //should be ExcludedCells
    for(int i = 0; i < excludedCellCount; i++){
        currentCell->excludedCells.push_back(fileData->readInt());
    }

}

int Mesh::readMesh(){
    SectionHeader signature;
    int indexEnd = 0;
    float x_position = 0;
    float y_position = 0;
    float z_position = 0;
    float a_position = 0;
    file->meshCount += 1;
    bool hasVertexSet = false;
    //qDebug() << Q_FUNC_INFO << "Do mesh stuff";

    elementCount = fileData->readInt();
    fileData->signature(&signature); //get extended info
    if(signature.type == "ExtendedInfo"){
        fileData->currentPosition = signature.sectionLength + signature.sectionLocation;
        fileData->signature(&signature); //then read vertex set
    }
    if (signature.type == "VertexSet") {
        hasVertexSet = true;
        //fileData->currentPosition += subSectionLength - 4;
        vertexSet.vertexCount = fileData->readInt();
        //qDebug() << Q_FUNC_INFO << "reading unknown values at" << fileData->currentPosition;
        int unknown4Byte1 = fileData->readInt();
        int unknown4Byte2 = fileData->readInt();
        //qDebug() << Q_FUNC_INFO << "unknown1" << unknown4Byte1 << "unknown2" << unknown4Byte2;

        //Position Array
        fileData->signature(&signature);
        if (signature.type == "PositionArray") {
            for(int position = 0; position < (signature.sectionLength-4)/12; position++){
                x_position = fileData->readFloat();
                //qDebug() << Q_FUNC_INFO << "X float: " << x_position << " from hex: " << file->parent->binChanger.reverse_input(fileData->mid(positionLocation + (i*12), 4).toHex(), 2);
                y_position = fileData->readFloat();
                z_position = fileData->readFloat();
                vertexSet.positionArray.positionList.push_back(QVector3D(x_position, y_position, z_position));
            }

            fileData->signature(&signature);
        } else {
            //qDebug() << Q_FUNC_INFO << "Model does not have PositionArray";
        }


        //Normal Array
        if (signature.type == "NormalArray") {
            for(int position = 0; position < (signature.sectionLength-4)/12; position++){
                /*This needs to be placed elsewhere - bbatt's reading algorithm is structured differently from mine.
                if (newTriangleStrip){
                    QVector3D averageNormal = normalArray.positionList[i] + normalArray.positionList[i+1] + normalArray.positionList[i+2];
                    QVector3D faceNormal = QVector3D::crossProduct((normalArray.positionList[i+1] - normalArray.positionList[i]), (normalArray.positionList[i+2] + normalArray.positionList[i])).normalize();
                    float dotResult = QVector3D::dotProduct(faceNormal, averageNormal);
                    if(dotResult > 0){
                        //write faceNormal? I guess?
                    } else {
                        //write -faceNormal
                    }


                }*/
                vertexSet.normalArray.positionList.push_back(fileData->read3DVector());
            }
            fileData->signature(&signature);
        } else {
            //qDebug() << Q_FUNC_INFO << "Model does not have NormalArray";
        }


        //Color Array
        if (signature.type == "ColorArray") {
            for(int position = 0; position < (signature.sectionLength-4)/16; position++){
                vertexSet.colorArray.positionList.push_back(fileData->readColor(true));
            }

            fileData->signature(&signature);
        } else {
            //qDebug() << Q_FUNC_INFO << "Model does not have ColorArray.";
        }


        //Texture Coords
        if (signature.type == "TextureCoords") {
            //qDebug() << Q_FUNC_INFO << "getting texture coords for" << headerData.name;
            for(int position = 0; position < (signature.sectionLength-4)/8; position++){
                x_position = fileData->readFloat();
                y_position = 1.0 - fileData->readFloat();
                vertexSet.textureCoords.positionList.push_back(QVector2D(x_position, y_position));
            }
            //qDebug() << Q_FUNC_INFO << vertexSet.textureCoords.positionList;
            fileData->signature(&signature);
        } else {
            //qDebug() << Q_FUNC_INFO << "Model does not have TextureCoords.";
        }
    } else {
        hasVertexSet = false;
        //qDebug() << Q_FUNC_INFO << "Model does not have VertexSet. Expected at " << fileData->currentPosition << ": " << signature.type << ". Moving on.";
    }



    //Element Array
    if (signature.type != "ElementArray") {
        qDebug() << Q_FUNC_INFO << "Expected ElementArray. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Exiting.";
        return 1;
    }
    //ElementArray *elementArray = new ElementArray;
    for(int readElement = 0; readElement < elementCount; readElement++){
        Element *element = new Element;
        fileData->signature(&signature);
        element->meshFaceSet.indexArray.triangleCount = 0;
        //need to check for LODINFO bit to see if we even read LODInfo
        if (signature.type == "LODInfo") {
            qDebug() << Q_FUNC_INFO << "Reached LOD Info at " << fileData->currentPosition << ": " << signature.type;
            break;
        }
        element->version = fileData->readInt();

        fileData->signature(&signature);
        if (signature.type == "MeshFaceSet") {
            element->meshFaceSet.version = fileData->readInt();
            element->meshFaceSet.indexCount = fileData->readInt();
            element->meshFaceSet.primitiveType = fileData->readInt();
            fileData->signature(&signature);
        } else {
            //qDebug() << Q_FUNC_INFO << "Model does not have MeshFaceSet. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Moving on.";
        }


        int triStripLength = 0;
        if (signature.type == "IndexArray") {
            indexEnd = signature.sectionLength + signature.sectionLocation;
            //qDebug() << Q_FUNC_INFO << "subsectionlength" << subSectionLength << "indexEnd" << indexEnd;
            //skipping IndexArray for now
            //fileData->currentPosition += subSectionLength - 4;
            while(fileData->currentPosition < indexEnd){
                TriangleStrip *triangleStrip = new TriangleStrip;
                if(element->meshFaceSet.primitiveType == 1){
                    //triangle strips
                    triangleStrip->stripIndecies.resize(fileData->readInt(2));
                    for (int triangle = 0; triangle<triangleStrip->stripIndecies.size();triangle++){
                        element->meshFaceSet.indexArray.triangleCount += 1;
                        triangleStrip->stripIndecies[triangle] = fileData->readInt(2);
                    }
                } else {
                    //just triangles, not triangle strips. All triangles are read to strip 0 for simplicity
                    triangleStrip->stripIndecies.resize(element->meshFaceSet.indexCount);
                    for (int triangle = 0; triangle<triangleStrip->stripIndecies.size();triangle++){
                        element->meshFaceSet.indexArray.triangleCount += 1;
                        triangleStrip->stripIndecies[triangle] = fileData->readInt(2);
                    }
                }
                element->meshFaceSet.indexArray.triangleStrips.push_back(*triangleStrip);
            }

            //qDebug() << Q_FUNC_INFO << "Triangle strip count for this mesh is " << element->meshFaceSet.indexArray.triangleStrips.size();
//            for(int i = 0; i < element->meshFaceSet.indexArray.triangleStrips.size(); i++){
//                qDebug() << Q_FUNC_INFO << "Triangle Strip " << i << ": " << element->meshFaceSet.indexArray.triangleStrips[i].stripIndecies.size();
//                for(int j = 0; j < element->meshFaceSet.indexArray.triangleStrips[i].stripIndecies.size(); j++){
//                    qDebug() << Q_FUNC_INFO << "index" << j << ":" << vertexSet.positionArray.positionList[element->meshFaceSet.indexArray.triangleStrips[i].stripIndecies[j]];
//                }
//            }

            fileData->signature(&signature);
        } else if(file->isSplitFile){
            int targetGeoSet = elementOffset+readElement;
            element->meshFaceSet.indexArray.triangleStrips.resize(file->meshFile->geoSets[targetGeoSet].indexArray.size());
            for(int i = 0; i < file->meshFile->geoSets[targetGeoSet].indexArray.size(); i++){
                for(int j = 0; j < file->meshFile->geoSets[targetGeoSet].indexArray[i].stripIndecies.size(); j++){
                    element->meshFaceSet.indexArray.triangleStrips[i].stripIndecies.push_back(file->meshFile->geoSets[targetGeoSet].indexArray[i].stripIndecies[j] + vertexSet.positionArray.positionList.size());
                }
            }
            //element->meshFaceSet.indexArray.triangleStrips = file->meshFile->geoSets[targetGeoSet].indexArray;
            if(!hasVertexSet){
                for(int i = 0; i < file->meshFile->geoSets[targetGeoSet].geoSetVerticies.size(); i++){
                    vertexSet.positionArray.positionList.push_back(file->meshFile->geoSets[targetGeoSet].geoSetVerticies[i]);
                }
                for(int i = 0; i < file->meshFile->geoSets[targetGeoSet].geoSetTexCoords.size(); i++){
                    vertexSet.textureCoords.positionList.push_back(file->meshFile->geoSets[targetGeoSet].geoSetTexCoords[i]);
                }
                for(int i = 0; i < file->meshFile->geoSets[targetGeoSet].geoSetNormals.size(); i++){
                    vertexSet.normalArray.positionList.push_back(file->meshFile->geoSets[targetGeoSet].geoSetNormals[i]);
                }
                for(int i = 0; i < file->meshFile->geoSets[targetGeoSet].geoSetColors.size(); i++){
                    vertexSet.colorArray.positionList.push_back(file->meshFile->geoSets[targetGeoSet].geoSetColors[i]);
                }
            }
            //qDebug() << Q_FUNC_INFO << "Model does not have IndexArray. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Moving on.";
        }

        if (signature.type != "SurfaceProperties") {
            qDebug() << Q_FUNC_INFO << "Expected SurfaceProperties. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Exiting.";
            return 1;
        }
        element->surfaceProperties.version = fileData->readInt(); //seems like a version number
        int nameLength = fileData->readInt();
        fileData->hexValue(&element->surfaceProperties.textureName, nameLength);
        //element->surfaceProperties.textureName = fileData->readHex(nameLength);
//        if(!hasVertexSet){
//            file->textureNameList.push_back(element->surfaceProperties.textureName);
//        }
        if(!file->textureNameList.contains(element->surfaceProperties.textureName) && element->surfaceProperties.textureName != ""){
            file->textureNameList.push_back(element->surfaceProperties.textureName);
        }
        nameLength = fileData->readInt();
        fileData->hexValue(&element->surfaceProperties.texture2Name, nameLength);
        //element->surfaceProperties.texture2Name = fileData->readHex(nameLength);
        //qDebug() << Q_FUNC_INFO << "texture for mesh" << name << "read as" << element->surfaceProperties.textureName;

        fileData->signature(&signature);
        if (signature.type != "Material") {
            qDebug() << Q_FUNC_INFO << "Expected Material. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Exiting.";
            return 1;
        }
        element->surfaceProperties.material.sectionLength = signature.sectionLength;

        if(element->surfaceProperties.version == 5){
            //materialRelated 3 is seen on TFA/PICKUPS models, 4 is seen on LightningRod
            element->surfaceProperties.material.version = fileData->readInt();

            element->surfaceProperties.material.nameLength = fileData->readInt();
            fileData->hexValue(&element->surfaceProperties.material.name, element->surfaceProperties.material.nameLength);
            //element->surfaceProperties.material.name = fileData->readHex(element->surfaceProperties.material.nameLength);
        }

        element->surfaceProperties.material.diffuse = fileData->read4DVector();
        element->surfaceProperties.material.specular = fileData->read4DVector();
        element->surfaceProperties.material.ambient = fileData->read4DVector();
        element->surfaceProperties.material.specularPower = fileData->readFloat();

        element->surfaceProperties.material.debug();

        fileData->signature(&signature);
        if (signature.type != "RenderStateGroup") {
            qDebug() << Q_FUNC_INFO << "Expected RenderStateGroup. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Exiting.";
            return 1;
        }
        element->surfaceProperties.renderStateGroup1.version = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.pixelBlend = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.textureBlend = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.textureClamp = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.textureFlip = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.textureFilter = fileData->readInt();
        element->surfaceProperties.renderStateGroup1.cullMode = fileData->readInt();

        //qDebug() << Q_FUNC_INFO << "material version number" << element->surfaceProperties.materialRelated;
        if(element->surfaceProperties.version > 2){
            //materialRelated 3 is seen on TFA/PICKUPS models
            fileData->signature(&signature);
            if (signature.type != "RenderStateGroup") {
                qDebug() << Q_FUNC_INFO << "Expected RenderStateGroup. Unexpected section found at " << fileData->currentPosition << ": " << signature.type <<". Exiting.";
                return 1;
            }
            element->surfaceProperties.renderStateGroup2.version = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.pixelBlend = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.textureBlend = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.textureClamp = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.textureFlip = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.textureFilter = fileData->readInt();
            element->surfaceProperties.renderStateGroup2.cullMode = fileData->readInt();
        }

        element->attributes = fileData->readInt();

        if(element->surfaceProperties.version > 3){
            element->attributes = fileData->readInt();
        }

        elementArray.elementArray.push_back(*element);
    }

    bool hasLodInfo = false;
    qDebug() << Q_FUNC_INFO << "checking for lod info at" << fileData->currentPosition;
    qDebug() << Q_FUNC_INFO << "suface properties version:" << elementArray.elementArray[0].surfaceProperties.version << "attributes:" << elementArray.elementArray[0].attributes;
    if(elementArray.elementArray[0].surfaceProperties.version > 2){// and elementArray.elementArray[0].attributes != 16){
        hasLodInfo = fileData->readBool();
    }
    qDebug() << Q_FUNC_INFO << "has lod info:" << hasLodInfo;
    if (hasLodInfo) {
        fileData->signature(&signature);
        elementArray.lodInfo.levels = fileData->readInt();
        std::vector<int> levelTargetArray;
        for (int readLevels = 0; readLevels < elementArray.lodInfo.levels; readLevels++) {
            levelTargetArray.push_back(std::max(fileData->readInt(), 0));
            levelTargetArray.push_back(std::max(fileData->readInt(), 0));
            elementArray.lodInfo.targetIndecies.push_back(levelTargetArray);
            levelTargetArray.clear();
            elementArray.lodInfo.levelDistance.push_back(fileData->readFloat());
        }
        if (elementArray.lodInfo.levels > file->highestLOD){
            file->highestLOD = elementArray.lodInfo.levels;
        }
    }

    qDebug() << Q_FUNC_INFO << "finished reading mesh at " << fileData->currentPosition;
    return 0;

}

void Mesh::modify(std::vector<Modifications> addedMods){
    addedMods.push_back(mods);

//    for (int i = 0; i < addedMods.size(); i++) {
//        qDebug() << name << "mod" << i << "is scale" << addedMods[i].scale;
//        qDebug() << name << "mod" << i << "is rotation" << addedMods[i].rotation;
//        qDebug() << name << "mod" << i << "is offset" << addedMods[i].offset;
//    }

//    for (int j = 0; j < vertexSet.positionArray.positionList.size(); ++j) {
//        for(int k = addedMods.size()-1; k > -1; k--){
//            vertexSet.positionArray.positionList[j] = vertexSet.positionArray.positionList[j] * addedMods[k].scale;
//            vertexSet.positionArray.positionList[j] = addedMods[k].rotation.rotatedVector(vertexSet.positionArray.positionList[j]);
//            vertexSet.positionArray.positionList[j] = vertexSet.positionArray.positionList[j] + addedMods[k].offset;
//        }
//    }
}

//template<typename WriteType>
void FileSection::writeNodes(QTextStream &fileOut) {
    float x = 0;
    float y = 0;
    float z = 0;
    float angle = 0;
    QString scaleValue;
    QVector3D tempOffset;
    for (int i = 0; i < meshList.size(); i++) {
        meshList[i]->mods.rotation.getAxisAndAngle(&x, &y, &z, &angle);
        QString meshName = meshList[i]->headerData.name;
        meshName = file->fileName + "_" + meshName.right(meshName.length() - (meshName.indexOf(".")+1));
        qDebug() << Q_FUNC_INFO << "offset rotation for " << meshName << "as xyza:" << x << y << z << angle;

        scaleValue = QString::number(meshList[i]->mods.scale, 'g', 3);
        tempOffset = meshList[i]->mods.offset;


        fileOut << "      <node id=\"" + meshName + "\" name=\"" + meshName + "\" type=\"NODE\">" << Qt::endl;

        fileOut << "        <translate sid=\"translate\">" + QString::number(tempOffset.x(), 'g', 3) + " " + QString::number(tempOffset.y(), 'g', 3)
                   + " " + QString::number(tempOffset.z(), 'g', 3) + "</translate>" << Qt::endl;

        fileOut << "        <rotate sid=\"rotate\">" + QString::number(x, 'g', 3) + " " + QString::number(y, 'g', 3)
                   + " " + QString::number(z, 'g', 3) + " " + QString::number(angle, 'g', 3) + "</rotate>" << Qt::endl;

        fileOut << "        <scale sid=\"scale\">" + scaleValue + " " + scaleValue + " " + scaleValue + "</scale>" << Qt::endl;

        fileOut << "        <instance_geometry url=\"#" + meshName + "-mesh\" name=\"" + meshName + "\">" << Qt::endl;

        meshList[i]->file = file;
        meshList[i]->writeNodesDAE(fileOut);
        fileOut << "        </instance_geometry>" << Qt::endl;
        //write(meshList[i], fileOut);
        meshList[i]->writeNodes(fileOut);
        fileOut << "      </node>" << Qt::endl;
    }
    for (int i = 0; i < sectionList.size(); i++) {
        sectionList[i]->mods.rotation.getAxisAndAngle(&x, &y, &z, &angle);
        QString nodeName = file->fileName + "_" + sectionList[i]->headerData.name;
        qDebug() << Q_FUNC_INFO << "offset rotation for" << nodeName << "as xyza:" << x << y << z << angle;

        scaleValue = QString::number(sectionList[i]->mods.scale, 'g', 3);
        tempOffset = sectionList[i]->mods.offset;

        fileOut << "      <node id=\"" + nodeName + "\" name=\"" + nodeName + "\" type=\"NODE\">" << Qt::endl;

        fileOut << "        <translate sid=\"translate\">" + QString::number(tempOffset.x(), 'g', 3) + " " + QString::number(tempOffset.y(), 'g', 3)
                   + " " + QString::number(tempOffset.z(), 'g', 3) + "</translate>" << Qt::endl;

        fileOut << "        <rotate sid=\"rotate\">" + QString::number(x, 'g', 3) + " " + QString::number(y, 'g', 3)
                   + " " + QString::number(z, 'g', 3) + " " + QString::number(angle, 'g', 3) + "</rotate>" << Qt::endl;

        fileOut << "        <scale sid=\"scale\">" + scaleValue + " " + scaleValue + " " + scaleValue + "</scale>" << Qt::endl;
        sectionList[i]->file = file;
        sectionList[i]->writeNodesDAE(fileOut);
        sectionList[i]->writeNodes(fileOut);
        fileOut << "      </node>" << Qt::endl;
    }
}

//template<typename WriteType>
void FileSection::searchListsWriteDAE(QTextStream &fileOut, void (Mesh::*write)(QTextStream&)) {
    for (int i = 0; i < meshList.size(); i++) {
        meshList[i]->file = file;
        (meshList[i]->*write)(fileOut);
        //write(meshList[i], fileOut);
        meshList[i]->searchListsWriteDAE(fileOut, write);
    }
    for (int i = 0; i < sectionList.size(); i++) {
        sectionList[i]->file = file;
        sectionList[i]->searchListsWriteDAE(fileOut, write);
    }
}

void Mesh::writeMaterialsDAE(QTextStream &fileOut){
    std::vector<int> chosenLOD = getChosenElements();
    QString textureName;
    for (int element = chosenLOD[0]; element <= chosenLOD[chosenLOD.size()-1]; element++) {
        textureName = elementArray.elementArray[element].surfaceProperties.textureName;
        fileOut << "    <material id=\"" + textureName +"Texture-material\" name=\"" + textureName + "Texture\">" << Qt::endl;
        fileOut << "      <instance_effect url=\"#" + textureName + "Texture-effect\"/>" << Qt::endl;
        fileOut << "    </material>" << Qt::endl;
    }
}

void Mesh::writeEffectsDAE(QTextStream &fileOut){
    std::vector<int> chosenLOD = getChosenElements();
    QString textureName;
    QString meshName = headerData.name;
    //QString meshName = file->fileName + "-" + headerData.name;
    meshName = file->fileName + "_" + meshName.right(meshName.length() - (meshName.indexOf(".")+1));
    for (int element = chosenLOD[0]; element <= chosenLOD[chosenLOD.size()-1]; element++) {
        //qDebug() << Q_FUNC_INFO << "element array";
        textureName = elementArray.elementArray[element].surfaceProperties.textureName;
        fileOut << "    <effect id=\"" + textureName +"Texture-effect\">" << Qt::endl;
        fileOut << "      <profile_COMMON>" << Qt::endl;
        fileOut << "        <newparam sid=\"" + textureName +"_png-surface\">" << Qt::endl;
        fileOut << "          <surface type=\"2D\">" << Qt::endl;
        fileOut << "            <init_from>" + textureName +"_png</init_from>" << Qt::endl;
        fileOut << "          </surface>" << Qt::endl;
        fileOut << "        </newparam>" << Qt::endl;
        fileOut << "        <newparam sid=\"" + textureName +"_png-sampler\">" << Qt::endl;
        fileOut << "          <sampler2D>" << Qt::endl;
        fileOut << "            <source>" + textureName +"_png-surface</source>" << Qt::endl;
        fileOut << "          </sampler2D>" << Qt::endl;
        fileOut << "        </newparam>" << Qt::endl;
        fileOut << "        <technique sid=\"common\">" << Qt::endl;
        fileOut << "          <lambert>" << Qt::endl;
        fileOut << "            <emission>" << Qt::endl;
        fileOut << "              <color sid=\"emission\">0 0 0 1</color>" << Qt::endl;
        fileOut << "            </emission>" << Qt::endl;
        fileOut << "            <diffuse>" << Qt::endl;
        fileOut << "              <texture texture=\"" + textureName +"_png-sampler\" texcoord=\""+meshName+"-mesh-texcoords\"/>" << Qt::endl;
        fileOut << "            </diffuse>" << Qt::endl;
        fileOut << "            <index_of_refraction>" << Qt::endl;
        fileOut << "              <float sid=\"ior\">1.45</float>" << Qt::endl;
        fileOut << "            </index_of_refraction>" << Qt::endl;
        fileOut << "          </lambert>" << Qt::endl;
        fileOut << "        </technique>" << Qt::endl;
        fileOut << "      </profile_COMMON>" << Qt::endl;
        fileOut << "    </effect>" << Qt::endl;
    }
}

void Mesh::writeImagesDAE(QTextStream &fileOut){
    std::vector<int> chosenLOD = getChosenElements();
    QString textureName;
    for (int element = chosenLOD[0]; element <= chosenLOD[chosenLOD.size()-1]; element++) {
        textureName = elementArray.elementArray[element].surfaceProperties.textureName;
        fileOut << "    <image id=\"" + textureName +"_png\" name=\"" + textureName +"_png\">" << Qt::endl;
        fileOut << "      <init_from>" + textureName +".png</init_from>" << Qt::endl;
        fileOut << "    </image>" << Qt::endl;
    }
}

void Mesh::writeDataDAE(QTextStream &fileOut){
    std::vector<int> chosenLOD = getChosenElements();
    QString meshName = headerData.name;
    //QString meshName = file->fileName + "-" + headerData.name;
    meshName = file->fileName + "_" + meshName.right(meshName.length() - (meshName.indexOf(".")+1));
    qDebug() << Q_FUNC_INFO << meshName;
    QString textureName;
    int triangle[3];
    int normalFlip = 0;
    QVector3D tempVec;

    fileOut << "    <geometry id=\"" + meshName + "-mesh\" name=\"" + meshName + "\">" << Qt::endl;
    fileOut << "      <mesh>" << Qt::endl;

    fileOut << "        <source id=\"" + meshName + "-mesh-positions\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-positions-array\" count = \"" + QString::number(vertexSet.positionArray.positionList.size()*3) + "\">";
    for(int position = 0; position < vertexSet.positionArray.positionList.size(); position++){
        fileOut << QString::number(vertexSet.positionArray.positionList[position].x()) << " ";
        fileOut << QString::number(vertexSet.positionArray.positionList[position].y()) << " ";
        fileOut << QString::number(vertexSet.positionArray.positionList[position].z()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-positions-array\" count=\"" + QString::number(vertexSet.positionArray.positionList.size()) + "\" stride=\"3\">" << Qt::endl;
    fileOut << "              <param name=\"X\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Y\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Z\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    if(!file->isSplitFile){
        fileOut << "        <source id=\"" + meshName + "-mesh-normals\">" << Qt::endl;
        fileOut << "          <float_array id=\"" + meshName + "-mesh-normals-array\" count = \"" + QString::number(vertexSet.normalArray.positionList.size()*3) + "\">";
        for(int position = 0; position < vertexSet.normalArray.positionList.size(); position++){
            fileOut << QString::number(vertexSet.normalArray.positionList[position].x()) << " ";
            fileOut << QString::number(vertexSet.normalArray.positionList[position].y()) << " ";
            fileOut << QString::number(vertexSet.normalArray.positionList[position].z()) << " ";
        }
        fileOut << "</float_array>" << Qt::endl;
        fileOut << "          <technique_common>" << Qt::endl;
        fileOut << "            <accessor source=\"#"+ meshName +"-mesh-normals-array\" count=\"" + QString::number(vertexSet.normalArray.positionList.size()) + "\" stride=\"3\">" << Qt::endl;
        fileOut << "              <param name=\"X\" type=\"float\"/>" << Qt::endl;
        fileOut << "              <param name=\"Y\" type=\"float\"/>" << Qt::endl;
        fileOut << "              <param name=\"Z\" type=\"float\"/>" << Qt::endl;
        fileOut << "            </accessor>" << Qt::endl;
        fileOut << "          </technique_common>" << Qt::endl;
        fileOut << "        </source>" << Qt::endl;
    }

    fileOut << "        <source id=\"" + meshName + "-mesh-map-0\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-map-0-array\" count = \"" + QString::number(vertexSet.textureCoords.positionList.size()*2) + "\">";
    for(int position = 0; position < vertexSet.textureCoords.positionList.size(); position++){
        fileOut << QString::number(vertexSet.textureCoords.positionList[position].x()) << " ";
        fileOut << QString::number(vertexSet.textureCoords.positionList[position].y()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-map-0-array\" count=\"" + QString::number(vertexSet.textureCoords.positionList.size()) + "\" stride=\"2\">" << Qt::endl;
    fileOut << "              <param name=\"U\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"V\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + meshName + "-mesh-colors\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-colors-array\" count = \"" + QString::number(vertexSet.colorArray.positionList.size()*4) + "\">";
    for(int position = 0; position < vertexSet.colorArray.positionList.size(); position++){
        fileOut << QString::number(vertexSet.colorArray.positionList[position].redF()) << " ";
        fileOut << QString::number(vertexSet.colorArray.positionList[position].greenF()) << " ";
        fileOut << QString::number(vertexSet.colorArray.positionList[position].blueF()) << " ";
        fileOut << QString::number(vertexSet.colorArray.positionList[position].alphaF()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-colors-array\" count=\"" + QString::number(vertexSet.colorArray.positionList.size()) + "\" stride=\"4\">" << Qt::endl;
    fileOut << "              <param name=\"R\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"G\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"B\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"A\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <vertices id=\"" + meshName + "-mesh-vertices\">" << Qt::endl;
    fileOut << "          <input semantic=\"POSITION\" source=\"#"+ meshName +"-mesh-positions\"/>" << Qt::endl;
    fileOut << "        </vertices>" << Qt::endl;

    int pointIndecies[3];
    QVector3D normal1;
    QVector3D normal2;
    QVector3D normal3;
    QVector3D position1;
    QVector3D position2;
    QVector3D position3;
    QVector3D averageNormal;
    QVector3D faceNormal;
    bool normalOrder = true;
    for (int element = chosenLOD[0]; element <= chosenLOD[chosenLOD.size()-1]; element++) {
        int triangleCount = 0;
        Element *currentElement = &elementArray.elementArray[element];
        textureName = currentElement->surfaceProperties.textureName;
        for(int strip = 0; strip < currentElement->meshFaceSet.indexArray.triangleStrips.size(); strip++){
            triangleCount += currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies.size();
        }
        fileOut << "        <triangles material=\"" + textureName +"Texture-material\" count=\"" + QString::number(triangleCount) + "\">" << Qt::endl;
        fileOut << "          <input semantic=\"VERTEX\" source=\"#"+ meshName +"-mesh-vertices\" offset=\"0\"/>" << Qt::endl;
        if(!file->isSplitFile){
            fileOut << "          <input semantic=\"NORMAL\" source=\"#"+ meshName +"-mesh-normals\" offset=\"0\"/>" << Qt::endl;
        }
        fileOut << "          <input semantic=\"COLOR\" source=\"#"+ meshName +"-mesh-colors\" offset=\"0\"/>" << Qt::endl; //not sure if this is even an option in DAE
        fileOut << "          <input semantic=\"TEXCOORD\" source=\"#"+ meshName +"-mesh-map-0\" offset=\"0\" set=\"0\"/>" << Qt::endl;

        fileOut << "          <p>";
        for (int strip = 0; strip < currentElement->meshFaceSet.indexArray.triangleStrips.size(); strip++){
            for (int triangle = 0 ; triangle < currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies.size()-2; triangle++ ){
                pointIndecies[0] = currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle];
                pointIndecies[1] = currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+1];
                pointIndecies[2] = currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+2];
                if(pointIndecies[0] == pointIndecies[1] or pointIndecies[0] == pointIndecies[2] or pointIndecies[1] == pointIndecies[2]){
                    qDebug() << Q_FUNC_INFO << "skipping triangle:" << pointIndecies[0] << pointIndecies[1] << pointIndecies[2] << "which is triangle"
                             << triangle << "in strip length" << currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies.size()-2;
                    continue;
                }
                if(vertexSet.normalArray.positionList.empty()){
                    normalOrder = true;
                } else {
                    normal1 = vertexSet.normalArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle]];
                    normal2 = vertexSet.normalArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+1]];
                    normal3 = vertexSet.normalArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+2]];
                    position1 = vertexSet.positionArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle]];
                    position2 = vertexSet.positionArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+1]];
                    position3 = vertexSet.positionArray.positionList[currentElement->meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[triangle+2]];
                    averageNormal = normal1 + normal2 + normal3;
                    faceNormal = QVector3D::crossProduct((position2 - position1), (position3 - position1));
                    faceNormal.normalize();
                    float dotResult = QVector3D::dotProduct(faceNormal, averageNormal);
                    if(dotResult > 0){
                        normalOrder = true;
                    } else {
                        normalOrder = false;
                    }
                }

                if(normalOrder){
                    fileOut << QString::number(pointIndecies[0]) << " ";
                    fileOut << QString::number(pointIndecies[1]) << " ";
                    fileOut << QString::number(pointIndecies[2]) << " ";
                } else {
                    fileOut << QString::number(pointIndecies[0]) << " ";
                    fileOut << QString::number(pointIndecies[2]) << " ";
                    fileOut << QString::number(pointIndecies[1]) << " ";
                }
            }
        }
        fileOut << "</p>" << Qt::endl;
        fileOut << "        </triangles>" << Qt::endl;
    }
    fileOut << "      </mesh>" << Qt::endl;
    fileOut << "    </geometry>" << Qt::endl;
}

void FileSection::writeNodesDAE(QTextStream &fileOut){
    qDebug() << Q_FUNC_INFO << "no data to write for this node:" << headerData.name;
}

void Instance::writeNodesDAE(QTextStream &fileOut){
    std::shared_ptr<TFFile> testLoaded;
    for(int i = 0; i < file->instanceNameList.size(); i++){
        file->parent->loadRequiredFile(file, file->instanceNameList[i], "VBIN");
        /*testLoaded = file->parent->matchFile(file->instanceNameList[i] + ".VBIN");
        while(testLoaded == nullptr){
            file->parent->messageError("Please load a file " + file->instanceNameList[i]+".VBIN");
            file->parent->openFile("VBIN");
            testLoaded = file->parent->matchFile(file->instanceNameList[i] + ".VBIN");
        }*/
        //testLoaded->outputPath = file->outputPath;
    }
    QString instanceName = file->fileName + "_instance_" + headerData.name;
    fileOut << "      <node id=\"" + instanceName + "_node\" name=\"" + instanceName + "_node\" type=\"NODE\">" << Qt::endl;
    fileOut << "      <node id=\"" + instanceName + "_another_node\" name=\"" + instanceName + "_another_node\" type=\"NODE\">" << Qt::endl;
    fileOut << "        <instance_node url=\"" + modelReference + ".DAE\" name=\"" + instanceName + "\"/>" << Qt::endl;
    fileOut << "      </node>" << Qt::endl;
    fileOut << "      </node>" << Qt::endl;
}

void Mesh::writeNodesDAE(QTextStream &fileOut){
    std::vector<int> chosenLOD = getChosenElements();
    QString textureName;
    QString meshName = headerData.name;
    //QString meshName = file->fileName + "-" + headerData.name;
    meshName = file->fileName + "_" + meshName.right(meshName.length() - (meshName.indexOf(".")+1));

    for (int element = chosenLOD[0]; element <= chosenLOD[chosenLOD.size()-1]; element++) {
        textureName = elementArray.elementArray[element].surfaceProperties.textureName;
        fileOut << "          <bind_material>" << Qt::endl;
        fileOut << "            <technique_common>" << Qt::endl;
        fileOut << "              <instance_material symbol=\"" + textureName +"Texture-material\" target=\"#" + textureName +"Texture-material\">" << Qt::endl;
        fileOut << "                <bind_vertex_input semantic=\"" + meshName + "-mesh-texcoords\" input_semantic=\"TEXCOORD\" input_set=\"0\"/>" << Qt::endl;
        fileOut << "              </instance_material>" << Qt::endl;
        fileOut << "            </technique_common>" << Qt::endl;
        fileOut << "          </bind_material>" << Qt::endl;
    }
    //fileOut << "      </node>" << Qt::endl;
}

std::vector<int> Mesh::getChosenElements(){
    std::vector<int> chosenLOD;
    if (elementArray.lodInfo.targetIndecies.size() <= file->selectedLOD) {
        //chosenLOD = {0,static_cast<int>(elementArray.elementArray.size()-1)};
        for(int i = 0; i < elementArray.elementArray.size(); i++){
            chosenLOD.push_back(i);
        }
    } else {
        chosenLOD = elementArray.lodInfo.targetIndecies[file->selectedLOD];
    }

    return chosenLOD;
}

void Mesh::writeDataSTL(QTextStream &fileOut){
    std::vector<int> chosenLOD;
    int triangle[3];
    QVector3D tempVec;

    //qDebug() << Q_FUNC_INFO << "mesh file" << file->fileName;
    //qDebug() << Q_FUNC_INFO << "lodinfo list size" << elementArray.lodInfo.targetIndecies.size() << "chosen lod" << file->parent->ListLevels->currentIndex();
    if (elementArray.lodInfo.targetIndecies.size() <= file->selectedLOD) {
        chosenLOD = {0,static_cast<int>(elementArray.elementArray.size()-1)};
    } else {
        chosenLOD = elementArray.lodInfo.targetIndecies[file->selectedLOD];
    }
    //qDebug() << Q_FUNC_INFO << "chosen LOD index targets: " << chosenLOD << " for mesh " << name;
    //qDebug() << Q_FUNC_INFO << "position list size" << vertexSet.positionArray.positionList.size();

    for (int index = chosenLOD[0]; index <= chosenLOD[chosenLOD.size()-1]; index++){
        //qDebug() << Q_FUNC_INFO << "index target" << index << "has" << int(elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips.size()) << "triangle strips";
        for (int strip = 0; strip < int(elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips.size());strip++){
            //qDebug() << Q_FUNC_INFO << "index" << index << "strip" << strip << "has" << int(elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips[strip].stripIndecies.size()) << "triangles";
            for (int m = 0; m < int(elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips[strip].stripIndecies.size()-2); m++){
                triangle[0] = elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[m];
                triangle[1] = elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[m+1];
                triangle[2] = elementArray.elementArray[index].meshFaceSet.indexArray.triangleStrips[strip].stripIndecies[m+2];
                if(triangle[0] == triangle[1] or triangle[1] == triangle[2] or triangle[0] == triangle[2]){
                    continue;
                }
                fileOut << "  facet normal 0 0 0" << Qt::endl;
                fileOut << "    outer loop" << Qt::endl;
                for (int n = 0; n < 3; ++n) {
                    fileOut << "      vertex ";
                    tempVec = vertexSet.positionArray.positionList[triangle[n]]; //just for readability for the next line
                    fileOut << QString::number(tempVec.x(), 'f', 3) << " " << QString::number(tempVec.y(), 'f', 3) << " " << QString::number(tempVec.z(), 'f', 3) << Qt::endl;
                }
                fileOut << "    endloop" << Qt::endl;
                fileOut << "  endfacet" << Qt::endl;
            }
        }
    }
}

void Mesh::applyKeyframe(QVector3D keyOffset){
    for(int position = 0; position < vertexSet.positionArray.positionList.size(); position++){
        vertexSet.positionArray.positionList[position] += keyOffset;
    }
}

void Mesh::applyKeyframe(QQuaternion keyRotation){
    for(int position = 0; position < vertexSet.positionArray.positionList.size(); position++){
        vertexSet.positionArray.positionList[position] = keyRotation.rotatedVector(vertexSet.positionArray.positionList[position]);
    }
}
