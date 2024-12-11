#include "Headers/Main/mainwindow.h"

void MeshVBIN::save(QString toType){
    if(toType == "STL"){
        outputDataSTL();
    } else if (toType == "DAE"){
        outputDataDAE();
    }
}

void MeshVBIN::load(QString fromType){
    int failedRead = 0;
    qDebug() << Q_FUNC_INFO << "this was called for type" << fromType;
    if(fromType == "MESH.VBIN"){
        failedRead = readData();
    } else {
        failedRead = 1;
    }
    if(failedRead){
        parent->messageError("There was an error reading " + fileName);
        return;
    }
}

void GeometrySet::getVerticies(){

    float u_val = 0;
    float v_val = 0;
    int loopcounter = 0;
    int endSection = 0;
    int headerLength = 0;
    bool recognizedSection = false;
    bool first6E = true;
    int geoSetSections = 0;
    int possibleSectionEnd = 0;
    int finalSectionCheck = 0;

    std::vector<QVector3D> processedVerticies;
    TriangleStrip processedIndecies;
    int mask = 0;
    int sectionVertecies = 0;

    version = fileData->readInt();
    unknownValue1 = fileData->readInt();
    vertexAttributeGroup = fileData->readInt();
    vertexFormat = fileData->readInt();
    unknownValue2 = fileData->readInt();
    unknownValue3 = fileData->readInt();
    unknownValue4 = fileData->readInt(2); //likely vertex count
    unknownValue5 = fileData->readInt();
    unknownValue6 = fileData->readInt(2);
    unknownValue12 = fileData->readInt();
    possibleSectionEnd = (unknownValue6*16) + fileData->currentPosition;
    //qDebug() << Q_FUNC_INFO << "position after first part header read" << fileData->currentPosition;
    fileData->currentPosition += 12;
    headerLength = fileData->readInt(1);
    if(headerLength & 2){
        fileData->currentPosition += 79;
    } else {
        fileData->currentPosition += 111;
    }
    if((vertexFormat != 7 and vertexFormat != 6) or unknownValue2 != 8){
        //file->parent->log("GeometrySet " + QString::number(geoSetID) + " at " + QString::number(headerData.sectionLocation) + " has vertex format " + QString::number(vertexFormat) + " and unkown value " + QString::number(unknownValue2));
    }
    //while(fileData->currentPosition < headerData.sectionLocation + headerData.sectionLength){
    while(!endSection){
    //for(int section = 0; section < possibleItemCount; section++){
        //qDebug() << Q_FUNC_INFO << "current position" << fileData->currentPosition << "vs end of section" << headerData.sectionLength + headerData.sectionLocation;

        int header1 = fileData->readUInt(1);
        int header2 = fileData->readUInt(1);
        int vertexCount = fileData->readUInt(1);
        int properties = fileData->readUInt(1);
        recognizedSection = false;

        //qDebug().noquote() << Q_FUNC_INFO << "header property value" << QString::number(header1, 16) << QString::number(header2, 16)
        //         << QString::number(vertexCount, 16) << QString::number(properties, 16) << "at" << fileData->currentPosition;

//        if(properties == 69){
//            qDebug().noquote() << Q_FUNC_INFO << "header property value" << QString::number(header1, 16) << QString::number(header2, 16)
//                     << QString::number(vertexCount, 16) << QString::number(properties, 16) << "at" << fileData->currentPosition;
//        }

        //qDebug() << Q_FUNC_INFO << "vertex count for item" << loopcounter << "is" << vertexCount << ", property is" << QString::number(properties, 16) << "at" << fileData->currentPosition;

        if(properties == 104){ //0x68
            recognizedSection = true;
            if(vertexCount == 1){
                fileData->currentPosition += 12;
            } else {
                sectionVertecies = vertexCount;
                for (int i = 0; i < vertexCount; i++){
                    geoSetVerticies.push_back(fileData->read3DVector());
                }
            }
        }

        if(properties == 105){ //0x69
            recognizedSection = true;
            if(vertexCount == 1){
                fileData->currentPosition += 12;
            } else {
                sectionVertecies = vertexCount;
                for (int i = 0; i < vertexCount; i++){
                    geoSetNormals.push_back(fileData->readMini3DVector());
                }
            }
            if(first6E){
                fileData->currentPosition += 8;
                first6E = false;
            }
            fileData->currentPosition += 2*(vertexCount%2);
        }

        if(properties == 110){ //0x6e
            recognizedSection = true;
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "section 0x6e check count" << vertexCount*4;
            for(int i = 0 ; i < vertexCount; i++){
                QColor possibleColor;
                possibleColor.setRedF(float(fileData->readInt(1))*2/255);
                possibleColor.setGreenF(float(fileData->readInt(1))*2/255);
                possibleColor.setBlueF(float(fileData->readInt(1))*2/255);
                possibleColor.setAlphaF(fmax(float(fileData->readInt(1))*2/255, 0));
                //qDebug() << Q_FUNC_INFO << "0x6e color read as" << possibleColor;

                geoSetColors.push_back(possibleColor);
            }
            //qDebug() << Q_FUNC_INFO << "6E ending" << fileData->currentPosition << "is first 6E" << first6E;
            if(first6E){
                fileData->currentPosition += 12;
                first6E = false;
            } else {
                fileData->currentPosition += 4;
            }
        }

        if (properties == 98) { //0x62
            /*This is probably the worst way to handle these sections, but I haven't been able to find ANY conssitency with the
            post-data footers. Some have 8 bytes, some have 9, and the properties are exactly the same for both.*/
            recognizedSection = true;
            for(int i = 0 ; i < vertexCount; i++){
                QColor possibleColor;
                float alphaValue = float(fileData->readInt(1))/255*2;
                possibleColor.setAlphaF(fmax(alphaValue, 0));
                geoSetColors.push_back(possibleColor);
            }
            QByteArray skipRead;

            int extraDataSkip = (28-vertexCount)%4;
            fileData->hexValue(&skipRead, extraDataSkip);
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "0x62" << skipRead.toHex(' ');
            fileData->hexValue(&skipRead, 8);
           // qDebug() << Q_FUNC_INFO << fileData->currentPosition << "0x62" << skipRead.toHex(' ');

            first6E = false;
        }

        if(properties == 114){ //0x72
            recognizedSection = true;
            QByteArray skipRead;
            int currentIndex = geoSetColors.size() - vertexCount;
            for(int i = 0 ; i < vertexCount; i++){
                float colorValue = float(fileData->readInt(1))/255*2;
                geoSetColors[currentIndex+i].setRedF(colorValue);
                geoSetColors[currentIndex+i].setGreenF(colorValue);
                geoSetColors[currentIndex+i].setBlueF(colorValue);
                //qDebug() << Q_FUNC_INFO << "0x62/0x72 color read as" << geoSetColors[currentIndex+i];
            }

//            for(int i = 0; i < vertexCount; i++){
//                qDebug() << Q_FUNC_INFO << "section 72 value:" << i << "is:" << fileData->readInt(1);
//            }
            int extraDataSkip = (28-vertexCount)%4;
            fileData->hexValue(&skipRead, extraDataSkip);
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "0x72" << skipRead.toHex(' ');
            fileData->hexValue(&skipRead, 12);
            //qDebug() << Q_FUNC_INFO << fileData->currentPosition << "0x72" << skipRead.toHex(' ');
        }

        if(properties == 117){ //0x75
            recognizedSection = true;
            for (int i = 0; i < vertexCount; i++){
//                u_val = fileData->readMiniFloat();
//                v_val = fileData->readMiniFloat();
                u_val = float(fileData->readInt(2))/2048;
                v_val = 1.0-(float(fileData->readInt(2))/2048);
                //qDebug() << Q_FUNC_INFO << "0x75 data" << u_val << v_val;
                geoSetTexCoords.push_back(QVector2D(u_val, v_val));
            }

            int sectionPossibleVersion = fileData->readInt();
            //qDebug() << Q_FUNC_INFO << "finished reading 0x75 data at" << fileData->currentPosition;
        }

        if(properties == 101){ //0x65
            recognizedSection = true;
//            indexList = fileData->readSpecial();
            indexList = 0;
            indexList += fileData->readUInt(1)<<16; //8
            indexList += fileData->readUInt(1)<<24;//0
            indexList += fileData->readUInt(1); //24
            indexList += fileData->readUInt(1) << 8; //16
            //int reversedList = 0;

            //read 4, 00 00 00 17
            finalSectionCheck = fileData->readInt();
            //qDebug() << Q_FUNC_INFO << "finished reading 0x65 data at" << fileData->currentPosition <<". end of section is at" << possibleSectionEnd;


            int indexOffset = processedIndecies.stripIndecies.size();
            for(int i = 0; i < indexArray.size(); i++){
                indexOffset += indexArray[i].stripIndecies.size();
            }
            //qDebug() << Q_FUNC_INFO << "processing" << sectionVertecies << "vertecies. index offset:" << indexOffset;
            for(int i = 0; i < sectionVertecies; i++){
                mask = 3221225472 >> i;
                //qDebug() << Q_FUNC_INFO << "checking value" << QString::number(indexList, 2) << "against mask" << QString::number(mask, 2) << "gives value:" << (indexList & mask);
                if((indexList & mask) == mask and processedIndecies.stripIndecies.size() > 2){
                    indexArray.push_back(processedIndecies);
                    processedIndecies.stripIndecies.clear();
                }
                processedIndecies.stripIndecies.push_back(i+indexOffset);

            }

            //qDebug() << Q_FUNC_INFO << "leftover vertecies:" << processedVerticies.size() << "processed verts:" << processedCount;
            //geoSetVerticies.clear();
        }

        if(properties == 4){
            recognizedSection = true;
        }

        if(properties == 5){ //found immediately before 0x69 sections
            recognizedSection = true;
        }

        if(properties == 17){
            recognizedSection = true;
            //add the final triangle strip - there should be a way to catch this earlier, but this should work for now.
            indexArray.push_back(processedIndecies);
            //qDebug() << Q_FUNC_INFO << "starting end-section read at" << fileData->currentPosition;
            fileData->currentPosition = headerData.sectionLength + headerData.sectionLocation - 57;
            //qDebug() << Q_FUNC_INFO << "moving to" << fileData->currentPosition;

            //skip 16 unknown
            fileData->currentPosition += 16;

            //read 4 - count of unique vers in geo set
            int uniqueVerts = fileData->readInt();

            //read 4 - count of faces in set's first triangle strip?? seems weird but okay
            int firstStripCount = fileData->readInt();

            //read 4 - length of geoset?? again??
            int footerSectionLength = fileData->readInt();

            //skip 0's
            fileData->currentPosition += 4;

            ///qDebug() << Q_FUNC_INFO << "finished reading section at" << fileData->currentPosition;
            endSection = true;
        }

        if (!recognizedSection) {
            file->parent->log("Unrecognized section type " + QString::number(properties, 16) + " at " + QString::number(fileData->currentPosition) + " | " + QString(Q_FUNC_INFO));
            fileData->currentPosition = headerData.sectionLocation + headerData.sectionLength;
            return;
        }
        loopcounter++;
        geoSetSections++;
    }
    //qDebug() << Q_FUNC_INFO << "geoset" << geoSetID << "at" << headerData.sectionLocation << "has vertex format" << vertexFormat << "has this many 0x68 sections:" << section68s;

//    int vertexRepeats = 0;
//    for(int i = 0; i < geoSetVerticies.size(); i++){
//        qDebug() << Q_FUNC_INFO << "point index" << i << "has value" << geoSetVerticies[i];
//    }
    //qDebug() << Q_FUNC_INFO << "geo set had" << geoSetSections << "sections";
    //qDebug() << Q_FUNC_INFO << "Next Geometry set.";

    //qDebug() << Q_FUNC_INFO << "successfully exited the loop at" << fileData->currentPosition;
    //fileData->currentPosition += 20;

    position1 = fileData->read3DVector();
    position2 = fileData->read3DVector();

    fileData->currentPosition += 1;


}

int MeshVBIN::readData(){
    parent->fileData.currentPosition = 4;
    int geoSetCount = 0;
    int totalVerts = 0;

    SectionHeader signature;
    parent->fileData.input = true;

    //only reading the first geo set for now
    parent->fileData.signature(&signature);
    qDebug() << Q_FUNC_INFO << "starting signature:" << signature.type;
    while(signature.type == "GeometrySet"){
        GeometrySet geoSet;
        geoSet.file = this;
        geoSet.headerData = signature;
        geoSet.fileData = &parent->fileData;
        geoSet.geoSetID = geoSetCount;
        /*if(geoSetCount <= graphFile->textureNameList.size()-1){
            geoSet.textureName = graphFile->textureNameList[geoSetCount];
        } else {
            geoSet.textureName = "dummytexture";
        }*/
        geoSet.getVerticies();

        geoSets.push_back(geoSet);
        //qDebug() << Q_FUNC_INFO << "geo set" << geoSetCount << "has" << geoSets[geoSetCount].geoSetVerticies.size() << "vertecies";
        totalVerts += geoSets[geoSetCount].geoSetVerticies.size();
        geoSetCount++;

        //qDebug() << Q_FUNC_INFO << "starting next geo set at" << parent->fileData.currentPosition;
        parent->fileData.signature(&signature);
    }
   parent->log("total geo sets: " + QString::number(geoSets.size()) + " | " + QString(Q_FUNC_INFO));
    qDebug() << Q_FUNC_INFO << "total geo set verts:" << totalVerts;

    qDebug() << Q_FUNC_INFO << "next signature:" << signature.type;

    return 0;
}

void GeometrySet::writeDataSTL(QTextStream &fileOut){
    QVector3D tempVec;
    QVector3D triangle[3];

    //qDebug() << Q_FUNC_INFO << "triangle strip count:" << indexArray.size();
    for(int index = 0; index < indexArray.size(); index++){
        //qDebug() << Q_FUNC_INFO << "length of triangle strip" << index << ":" << indexArray[index].size();
        for(int tristrip = 0; tristrip < indexArray[index].stripIndecies.size()-2; tristrip++){
            triangle[0] = geoSetVerticies[indexArray[index].stripIndecies[tristrip]];
            triangle[1] = geoSetVerticies[indexArray[index].stripIndecies[tristrip]+1];
            triangle[2] = geoSetVerticies[indexArray[index].stripIndecies[tristrip]+2];
            if(triangle[0] == triangle[1] or triangle[1] == triangle[2] or triangle[0] == triangle[2]){
                tristrip+=1;
                continue;
            }
            fileOut << "  facet normal 0 0 0" << Qt::endl;
            fileOut << "    outer loop" << Qt::endl;
            for (int n = 0; n < 3; ++n) {
                fileOut << "      vertex ";
                tempVec = triangle[n]; //just for readability for the next line
                fileOut << QString::number(tempVec.x(), 'f', 3) << " " << QString::number(tempVec.y(), 'f', 3) << " " << QString::number(tempVec.z(), 'f', 3) << Qt::endl;
            }
            fileOut << "    endloop" << Qt::endl;
            fileOut << "  endfacet" << Qt::endl;
        }
    }
}

void GeometrySet::writeMaterialsDAE(QTextStream &fileOut){
    fileOut << "    <material id=\"" + textureName +"Texture-material\" name=\"" + textureName + "Texture\">" << Qt::endl;
    fileOut << "      <instance_effect url=\"#" + textureName + "Texture-effect\"/>" << Qt::endl;
    fileOut << "    </material>" << Qt::endl;
}

void GeometrySet::writeEffectsDAE(QTextStream &fileOut){
    QString meshName = "geometryset-" + QString::number(geoSetID);

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

void GeometrySet::writeImagesDAE(QTextStream &fileOut){
    fileOut << "    <image id=\"" + textureName +"_png\" name=\"" + textureName +"_png\">" << Qt::endl;
    fileOut << "      <init_from>" + textureName +".png</init_from>" << Qt::endl;
    fileOut << "    </image>" << Qt::endl;
}

void GeometrySet::writeNodesDAE(QTextStream &fileOut){
    QString meshName = "geometryset-" + QString::number(geoSetID);

    fileOut << "      <node id=\"" + meshName + "\" name=\"" + meshName + "\" type=\"NODE\">" << Qt::endl;
    fileOut << "        <matrix sid=\"transform\">1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1</matrix>" << Qt::endl;
    fileOut << "        <instance_geometry url=\"#" + meshName + "-mesh\" name=\"" + meshName + "\">" << Qt::endl;

    //textureName = elementArray.elementArray[element].surfaceProperties.textureName;
    fileOut << "          <bind_material>" << Qt::endl;
    fileOut << "            <technique_common>" << Qt::endl;
    fileOut << "              <instance_material symbol=\"" + textureName +"Texture-material\" target=\"#" + textureName +"Texture-material\">" << Qt::endl;
    fileOut << "                <bind_vertex_input semantic=\"" + meshName + "-mesh-texcoords\" input_semantic=\"TEXCOORD\" input_set=\"0\"/>" << Qt::endl;
    fileOut << "              </instance_material>" << Qt::endl;
    fileOut << "            </technique_common>" << Qt::endl;
    fileOut << "          </bind_material>" << Qt::endl;
    fileOut << "        </instance_geometry>" << Qt::endl;

    fileOut << "      </node>" << Qt::endl;
}

void GeometrySet::writeDataDAE(QTextStream &fileOut){
    QString meshName = "geometryset-" + QString::number(geoSetID);
    qDebug() << Q_FUNC_INFO << meshName;
    int triangle[3];
    QVector3D tempVec;

    fileOut << "    <geometry id=\"" + meshName + "-mesh\" name=\"" + meshName + "\">" << Qt::endl;
    fileOut << "      <mesh>" << Qt::endl;

    fileOut << "        <source id=\"" + meshName + "-mesh-positions\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-positions-array\" count = \"" + QString::number(geoSetVerticies.size()*3) + "\">";
    for(int position = 0; position < geoSetVerticies.size(); position++){
        fileOut << QString::number(geoSetVerticies[position].x()) << " ";
        fileOut << QString::number(geoSetVerticies[position].y()) << " ";
        fileOut << QString::number(geoSetVerticies[position].z()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-positions-array\" count=\"" + QString::number(geoSetVerticies.size()) + "\" stride=\"3\">" << Qt::endl;
    fileOut << "              <param name=\"X\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Y\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Z\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    /*fileOut << "        <source id=\"" + meshName + "-mesh-normals\">" << Qt::endl;
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
    fileOut << "        </source>" << Qt::endl;*/

    fileOut << "        <source id=\"" + meshName + "-mesh-map-0\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-map-0-array\" count = \"" + QString::number(geoSetTexCoords.size()*2) + "\">";
    for(int position = 0; position < geoSetTexCoords.size(); position++){
        fileOut << QString::number(geoSetTexCoords[position].x()) << " ";
        fileOut << QString::number(geoSetTexCoords[position].y()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-map-0-array\" count=\"" + QString::number(geoSetTexCoords.size()) + "\" stride=\"2\">" << Qt::endl;
    fileOut << "              <param name=\"U\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"V\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + meshName + "-mesh-colors\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + meshName + "-mesh-colors-array\" count = \"" + QString::number(geoSetColors.size()*4) + "\">";
    for(int color = 0; color < geoSetColors.size(); color++){
        fileOut << QString::number(geoSetColors[color].redF()) << " ";
        fileOut << QString::number(geoSetColors[color].greenF()) << " ";
        fileOut << QString::number(geoSetColors[color].blueF()) << " ";
        fileOut << QString::number(geoSetColors[color].alphaF()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#"+ meshName +"-mesh-colors-array\" count=\"" + QString::number(geoSetColors.size()) + "\" stride=\"4\">" << Qt::endl;
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

    int triangleCount = 0;
    for(int strip = 0; strip < indexArray.size(); strip++){
        triangleCount += indexArray[strip].stripIndecies.size();
    }
    fileOut << "        <triangles material=\"" + textureName +"Texture-material\" count=\"" + QString::number(triangleCount) + "\">" << Qt::endl;
    //fileOut << "        <triangles count=\"" + QString::number(triangleCount) + "\">" << Qt::endl;
    fileOut << "          <input semantic=\"VERTEX\" source=\"#"+ meshName +"-mesh-vertices\" offset=\"0\"/>" << Qt::endl;
    //fileOut << "          <input semantic=\"NORMAL\" source=\"#"+ meshName +"-mesh-normals\" offset=\"0\"/>" << Qt::endl;
    fileOut << "          <input semantic=\"COLOR\" source=\"#"+ meshName +"-mesh-texcoords\" offset=\"0\"/>" << Qt::endl; //not sure if this is even an option in DAE
    fileOut << "          <input semantic=\"TEXCOORD\" source=\"#"+ meshName +"-mesh-map-0\" offset=\"0\" set=\"0\"/>" << Qt::endl;

    fileOut << "          <p>";
    for (int strip = 0; strip < indexArray.size(); strip++){
        for (int triangle = 0 ; triangle < indexArray[strip].stripIndecies.size()-2; triangle++ ){
            fileOut << QString::number(indexArray[strip].stripIndecies[triangle]) << " ";
            fileOut << QString::number(indexArray[strip].stripIndecies[triangle+1]) << " ";
            fileOut << QString::number(indexArray[strip].stripIndecies[triangle+2]) << " ";
        }
    }
    fileOut << "</p>" << Qt::endl;
    fileOut << "        </triangles>" << Qt::endl;

    fileOut << "      </mesh>" << Qt::endl;
    fileOut << "    </geometry>" << Qt::endl;
}

int MeshVBIN::outputDataSTL(){

    std::vector<int> allowedMeshes = {1,2,3};
    std::vector<int> chosenLOD;

    //applyKeyframe();

//    QString fileOut = QFileDialog::getSaveFileName(parent, parent->tr("Select Output STL"), QDir::currentPath() + "/STL/", parent->tr("Model Files (*.stl)"));
//    if(fileOut.isEmpty()){
//        parent->messageError("STL export cancelled.");
//    }
    QFile stlOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if(!stlOut.open(QIODevice::ReadWrite)){
        return 1;
    }
    QTextStream stream(&stlOut);
    stream << "solid Default" << Qt::endl;
    qDebug() << Q_FUNC_INFO << "sections:" << geoSets.size();
//    geoSets[0].writeDataSTL(stream);
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeDataSTL(stream);
    }
    stream << "endsolid Default" << Qt::endl;

    parent->messageSuccess("STL file saved.");
    qDebug() << Q_FUNC_INFO << "STL output complete.";

    return 0;
}

int MeshVBIN::outputDataDAE(){
    QFile stlOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if(!stlOut.open(QIODevice::ReadWrite)){
        parent->messageError("DAE export failed, could not open output file.");
        return 1;
    }
    QTextStream stream(&stlOut);
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
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeEffectsDAE(stream);
    }
    stream << "  </library_effects>" << Qt::endl;


    stream << "  <library_images>" << Qt::endl;
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeImagesDAE(stream);
    }
    stream << "  </library_images>" << Qt::endl;

    stream << "  <library_materials>" << Qt::endl;
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeMaterialsDAE(stream);
    }
    stream << "  </library_materials>" << Qt::endl;

    stream << "  <library_geometries>" << Qt::endl;
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeDataDAE(stream);
    }
    stream << "  </library_geometries>" << Qt::endl;

    stream << "  <library_visual_scenes>" << Qt::endl;
    stream << "    <visual_scene id=\"Scene\" name=\"Scene\">" << Qt::endl;
    for(int i = 0; i < geoSets.size(); i++){
        geoSets[i].writeNodesDAE(stream);
    }
    stream << "    </visual_scene>" << Qt::endl;
    stream << "  </library_visual_scenes>" << Qt::endl;

    stream << "  <scene>" << Qt::endl;
    stream << "    <instance_visual_scene url=\"#Scene\"/>" << Qt::endl;
    stream << "  </scene>" << Qt::endl;

    stream << "</COLLADA>" << Qt::endl;

    parent->messageSuccess("DAE file saved.");
    qDebug() << Q_FUNC_INFO << "DAE output complete.";
    return 0;
}
