#include "Headers/Models/Antioch2.h"
#include "Headers/Main/BinChanger.h"
#include "Headers/Models/vbin.h"
#include "Headers/Main/exDebugger.h"

AnimationBase *makeAnimation(QString animationSignature){
    if (animationSignature == "anAnimationTranslation") {
        return new AnimationTranslation;
    } else if (animationSignature == "anAnimationOrientation") {
        return new AnimationOrientation;
    } /*else if (animationSignature == "~anAnimationScale") {
        return new AnimationScale;
    } else {
        return nullptr;
    }*/
    return nullptr;
}

void AnimationSourceSet::readAnimationSet(){
    version = file->fileData->readInt();
    sourceCount = file->fileData->readInt();
    SectionHeader signature;


    file->fileData->signature(&signature);
    if(signature.type != "StreamArray"){
        qDebug() << Q_FUNC_INFO << "Expected Stream Array at" << file->fileData->currentPosition;
        return;
    }

    for (int animation = 0; animation < sourceCount; animation++) {
        AnimationStream *animationStream = new AnimationStream;
        animationStream->file = file;
        animationStream->readAnimationStream();
        streamArray.push_back(animationStream);
    }
}

void AnimationStream::readAnimationStream(){
    SectionHeader signature;
    file->fileData->signature(&signature);
    if(signature.type != "anAnimationStream") {
        qDebug() << Q_FUNC_INFO << "Expected Animation Stream at" << file->fileData->currentPosition << ". Instead found" << signature.type;
        file->fileData->currentPosition = sectionEnd;
        return;
    }

    name = signature.name;
    sectionLength = signature.sectionLength;

    //int nameLength = file->fileData->readInt();
    //file->fileData->hexValue(&name, nameLength);
    //name = file->fileData->readHex(nameLength);
    //sectionLength = file->fileData->readInt();

    version1 = file->fileData->readInt();
    minimumLOD = file->fileData->readInt();
    version2 = file->fileData->readInt();
    animationPlaybackType = file->fileData->readInt();
    defaultPlaybackSpeed = file->fileData->readFloat();
    channelCount = file->fileData->readInt();

    file->fileData->signature(&signature);
    if(signature.type != "channelArray") {
        qDebug() << Q_FUNC_INFO << "Expected Channel Array at" << file->fileData->currentPosition << ". Instead found" << signature.type;
        file->fileData->currentPosition = sectionEnd;
        return;
    }

    qDebug() << Q_FUNC_INFO << "total channels" << channelCount;
    for(int channelIndex = 0; channelIndex < channelCount; channelIndex++){
        qDebug() << Q_FUNC_INFO << "checking channel" << channelIndex << "at" << file->fileData->currentPosition;
        SectionHeader channel;
        file->fileData->signature(&channel);
        int ending = channel.sectionLength + channel.sectionLocation;

        AnimationBase *animationChannel = makeAnimation(channel.type);
        if (animationChannel == nullptr) {
            qDebug() << Q_FUNC_INFO << "Unsupported channel type" << channel.type << " found at" << file->fileData->currentPosition << ". Skipping for now.";
            file->fileData->currentPosition = ending;
            continue;
        }
        animationChannel->hasScalar = false;
        qDebug() << Q_FUNC_INFO << "reading an animation channel" << channel.name << "of type" << channel.type << "at" << file->fileData->currentPosition;
        animationChannel->file = file;
        animationChannel->animationType = channel.type;

        animationChannel->name = channel.name;
        animationChannel->sectionLength = channel.sectionLength;
        animationChannel->version3 = file->fileData->readInt();
        animationChannel->keyframeCount = file->fileData->readInt();
        animationChannel->subVersion = file->fileData->readInt();
        if(channel.type == "anAnimationOrientation"){
            animationChannel->has4DSpline = file->fileData->readBool();
            animationChannel->hasPackedSpline = file->fileData->readBool();
        }

        SectionHeader motion;
        file->fileData->signature(&motion);
        animationChannel->motionType = motion.type;

        if(motion.type == "Scalar"){
            animationChannel->readScalar();
            file->fileData->signature(&motion);
        }

        if (motion.type == "Linear") {
            animationChannel->readLinear();
        } else if (motion.type == "Cubic") {
            animationChannel->readCubic();
        } else if (motion.type == "CubicC2") {
            animationChannel->readCubicC2();
        } else {
            qDebug() << Q_FUNC_INFO << "Unsupported motion type" << motion.type << ". Skipping for now.";
            file->fileData->currentPosition = ending;
            continue;
        }
        animationChannel->maxTime = 0;
        for(int i = 0; i < animationChannel->keyframeTimes.size(); i++){
            if(animationChannel->keyframeTimes[i] > animationChannel->maxTime){
                animationChannel->maxTime = animationChannel->keyframeTimes[i];
            }
        }
        channelArray.push_back(animationChannel);
    }
    qDebug() << Q_FUNC_INFO << "location after animation read:" << file->fileData->currentPosition;
}

void AnimationBase::readScalar(){
    qDebug() << Q_FUNC_INFO << "reading an animation scalar modifier at" << file->fileData->currentPosition;
    hasScalar = true;
    //sectionLength = file->fileData->readInt();
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    float x_pos = file->fileData->readFloat();
    float y_pos = file->fileData->readFloat();
    float z_pos = file->fileData->readFloat();
    vectorList.push_back(QVector3D(x_pos, y_pos, z_pos));
    qDebug() << Q_FUNC_INFO << "scalar modifier read as" << QVector3D(x_pos, y_pos, z_pos);
}

void AnimationTranslation::readLinear(){
    qDebug() << Q_FUNC_INFO << "reading a translation animation linear motion";
    //sectionLength = file->fileData->readInt();
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    qDebug() << Q_FUNC_INFO << "reading" << keyframeCount << "values";
    for (int frameData = 0; frameData < keyframeCount; frameData++) {
        keyframeTimes.push_back(file->fileData->readFloat());
        float x_pos = file->fileData->readFloat();
        float y_pos = file->fileData->readFloat();
        float z_pos = file->fileData->readFloat();
        vectorList.push_back(QVector3D(x_pos, y_pos, z_pos));
    }
    qDebug() << Q_FUNC_INFO << "linear translation has keyframes" << keyframeTimes;
    qDebug() << Q_FUNC_INFO << "linear translation has values:" << vectorList;
}

void AnimationTranslation::readCubic(){
    if(hasScalar){
        qDebug() << Q_FUNC_INFO << "reading a translation animation cubic motion";
        //sectionLength = file->fileData->readInt();
        version3 = file->fileData->readInt();
        keyframeCount = file->fileData->readInt();
        subVersion = file->fileData->readInt();
        qDebug() << Q_FUNC_INFO << "reading" << keyframeCount << "values at" << file->fileData->currentPosition;
        for (int frameData = 0; frameData < keyframeCount; frameData++) {
            keyframeTimes.push_back(file->fileData->readFloat());
            float x_pos = file->fileData->readFloat();
            float y_pos = file->fileData->readFloat();
            float z_pos = file->fileData->readFloat();
            vectorList.push_back(QVector3D(x_pos, y_pos, z_pos));
        }
    } else {
        keyframeTimes.push_back(file->fileData->readFloat());
        for(int i = 0; i < 3; i++){
            float x_pos = file->fileData->readFloat();
            float y_pos = file->fileData->readFloat();
            float z_pos = file->fileData->readFloat();
            vectorList.push_back(QVector3D(x_pos, y_pos, z_pos));
        }
    }
}

void AnimationOrientation::readLinear(){
    qDebug() << Q_FUNC_INFO << "reading an orientation animation linear motion";
    //sectionLength = file->fileData->readInt();
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    qDebug() << Q_FUNC_INFO << "reading" << keyframeCount << "values at" << file->fileData->currentPosition;
    for (int frameData = 0; frameData < keyframeCount; frameData++) {
        keyframeTimes.push_back(file->fileData->readFloat());
        if (hasPackedSpline) {
            rotationList.push_back(file->fileData->readMiniQuaternion());
        } else {
            rotationList.push_back(file->fileData->readQuaternion());
        }
    }
}

void AnimationOrientation::readCubic(){
    //might need to add check for miniquat
    qDebug() << Q_FUNC_INFO << "reading an orientation animation cubic motion at" << file->fileData->currentPosition;
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    if(hasScalar){
        qDebug() << Q_FUNC_INFO << "animation has a scalar component";
        //sectionLength = file->fileData->readInt();
        for (int frameData = 0; frameData < keyframeCount; frameData++) {
            keyframeTimes.push_back(file->fileData->readFloat());
            rotationList.push_back(file->fileData->readMiniQuaternion());
        }
    } else {
        qDebug() << Q_FUNC_INFO << "animation does not have a scalar component";
        for (int frameData = 0; frameData < keyframeCount; frameData++) {
            keyframeTimes.push_back(file->fileData->readFloat());
            for(int i = 0; i < 3; i++){
                rotationList.push_back(file->fileData->readMiniQuaternion());
            }
        }
    }

    qDebug() << Q_FUNC_INFO << "finished reading cubic orientation at" << file->fileData->currentPosition;
}

void AnimationOrientation::readCubicC2(){
    //might need to add check for miniquat
    qDebug() << Q_FUNC_INFO << "reading an orientation animation cubic2 motion";
    //sectionLength = file->fileData->readInt();
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    qDebug() << Q_FUNC_INFO << "reading" << keyframeCount << "values";
    for (int frameData = 0; frameData < keyframeCount; frameData++) {
        keyframeTimes.push_back(file->fileData->readFloat());
        rotationList.push_back(file->fileData->readMiniQuaternion());
        velocityList2.push_back(file->fileData->readMiniQuaternion());
        qDebug() << Q_FUNC_INFO << "keyframe" << keyframeTimes[frameData] << "has rotation" << rotationList[frameData] << "and velocity" << velocityList2[frameData];
    }
}

void AnimationTranslation::readCubicC2(){
    qDebug() << Q_FUNC_INFO << "reading an translation animation cubic2 motion";
    //sectionLength = file->fileData->readInt();
    version3 = file->fileData->readInt();
    keyframeCount = file->fileData->readInt();
    subVersion = file->fileData->readInt();
    qDebug() << Q_FUNC_INFO << "reading" << keyframeCount << "values";
    for (int frameData = 0; frameData < keyframeCount; frameData++) {
        keyframeTimes.push_back(file->fileData->readFloat());
        vectorList.push_back(file->fileData->read3DVector());
        velocityList.push_back(file->fileData->read3DVector());
        qDebug() << Q_FUNC_INFO << "keyframe" << keyframeTimes[frameData] << "has offset" << vectorList[frameData];
    }
}

float AnimationBase::findSplineAtTime(float time){
    for (int i = 0; i < keyframeTimes.size()-1; i++) {
        if(time > keyframeTimes[i] && time < keyframeTimes[i+1]){
            return (time - keyframeTimes[i]) / (keyframeTimes[i+1] - keyframeTimes[i]);
        }
    }
    return 0;
}

void AnimationBase::readLinear(){
    file->m_Debug->MessageError("THIS SHOULD NOT RUN");
}

void AnimationBase::readCubic(){
    file->m_Debug->MessageError("THIS SHOULD NOT RUN");
}

void AnimationBase::readCubicC2(){
    file->m_Debug->MessageError("THIS SHOULD NOT RUN");
}

void AnimationSourceSet::writeAnimationsDAE(QTextStream &fileOut){
    for(int animations = 0; animations < streamArray.size(); animations++){
        streamArray[animations]->writeAnimationStreamDAE(fileOut);
    }
}

void AnimationStream::writeAnimationStreamDAE(QTextStream &fileOut){
    //<animation id="action_container-SENTRYDRONE-Gun" name="SENTRYDRONE-Gun">
    fileOut << "    <animation id=\"action_container-" + name +"\">" << Qt::endl;
    for(int channel = 0; channel < channelArray.size(); channel++){
        channelArray[channel]->writeAnimationChannelDAE(fileOut, name);
    }
    fileOut << "    </animation>" << Qt::endl;
}

void AnimationBase::writeAnimationChannelDAE(QTextStream &fileOut, QString animationName){
    qDebug() << Q_FUNC_INFO << "Virtual function overwrite was not called.";
}

void AnimationTranslation::writeAnimationChannelDAE(QTextStream &fileOut, QString animationName){
    //this one might need different versions for each channel type
    fileOut << "      <animation id=\"" + name + "_" + animationName + "_Translation\">" << Qt::endl;
    fileOut << "        <source id=\"" + name + "_" + animationName + "-input\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + name + "_" + animationName + "-input-array\" count=\"" + QString::number(keyframeCount) +"\">";
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        fileOut << QString::number(keyframeTimes[frame]) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-input-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"1\">" << Qt::endl;
    fileOut << "              <param name=\"TIME\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + name + "_" + animationName + "-output\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + name + "_" + animationName + "-output-array\" count=\"" + QString::number(keyframeCount*3) +"\">";
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        fileOut << QString::number(vectorList[frame].x()) << " ";
        fileOut << QString::number(vectorList[frame].y()) << " ";
        fileOut << QString::number(vectorList[frame].z()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-output-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"3\">" << Qt::endl;
    fileOut << "              <param name=\"X\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Y\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Z\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + name + "_" + animationName + "-interpolation\">" << Qt::endl;
    fileOut << "          <Name_array id=\"" + name + "_" + animationName + "-interpolation-array\" count=\"" + QString::number(keyframeCount) +"\">";
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        fileOut << motionType.toUpper() << " ";
    }
    fileOut << "</Name_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-interpolation-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"1\">" << Qt::endl;
    fileOut << "              <param name=\"INTERPOLATION\" type=\"name\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <sampler id=\"" + name + "_" + animationName + "-sampler\">" << Qt::endl;
    fileOut << "          <input semantic=\"INPUT\" source=\"#" + name + "_" + animationName + "-input\"/>" << Qt::endl;
    fileOut << "          <input semantic=\"OUTPUT\" source=\"#" + name + "_" + animationName + "-output\"/>" << Qt::endl;
    fileOut << "          <input semantic=\"INTERPOLATION\" source=\"#" + name + "_" + animationName + "-interpolation\"/>" << Qt::endl;
    fileOut << "        </sampler>" << Qt::endl;
    fileOut << "        <channel source=\"#" + name + "_" + animationName + "-sampler\" target=\"" + name + "/translate\"/>" << Qt::endl;
    fileOut << "      </animation>" << Qt::endl;
}

void AnimationOrientation::writeAnimationChannelDAE(QTextStream &fileOut, QString animationName){
    //this one might need different versions for each channel type
    fileOut << "      <animation id=\"" + name + "_" + animationName + "_orientation\">" << Qt::endl;
    fileOut << "        <source id=\"" + name + "_" + animationName + "-input\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + name + "_" + animationName + "-input-array\" count=\"" + QString::number(keyframeCount) +"\">";
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        fileOut << QString::number(keyframeTimes[frame]) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-input-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"1\">" << Qt::endl;
    fileOut << "              <param name=\"TIME\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + name + "_" + animationName + "-output\">" << Qt::endl;
    fileOut << "          <float_array id=\"" + name + "_" + animationName + "-output-array\" count=\"" + QString::number(keyframeCount*3) +"\">";
    QVector3D eulerRotation;
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        eulerRotation = rotationList[frame].toEulerAngles();
        fileOut << QString::number(eulerRotation.x()) << " ";
        fileOut << QString::number(eulerRotation.y()) << " ";
        fileOut << QString::number(eulerRotation.z()) << " ";
    }
    fileOut << "</float_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-output-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"3\">" << Qt::endl;
    fileOut << "              <param name=\"X\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Y\" type=\"float\"/>" << Qt::endl;
    fileOut << "              <param name=\"Z\" type=\"float\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <source id=\"" + name + "_" + animationName + "-interpolation\">" << Qt::endl;
    fileOut << "          <Name_array id=\"" + name + "_" + animationName + "-interpolation-array\" count=\"" + QString::number(keyframeCount) +"\">";
    for(int frame = 0; frame < keyframeTimes.size(); frame++){
        fileOut << motionType.toUpper() << " ";
    }
    fileOut << "</Name_array>" << Qt::endl;
    fileOut << "          <technique_common>" << Qt::endl;
    fileOut << "            <accessor source=\"#" + name + "_" + animationName + "-interpolation-array\" count=\"" + QString::number(keyframeCount) +"\" stride=\"1\">" << Qt::endl;
    fileOut << "              <param name=\"INTERPOLATION\" type=\"name\"/>" << Qt::endl;
    fileOut << "            </accessor>" << Qt::endl;
    fileOut << "          </technique_common>" << Qt::endl;
    fileOut << "        </source>" << Qt::endl;

    fileOut << "        <sampler id=\"" + name + "_" + animationName + "-sampler\">" << Qt::endl;
    fileOut << "          <input semantic=\"INPUT\" source=\"#" + name + "_" + animationName + "-input\"/>" << Qt::endl;
    fileOut << "          <input semantic=\"OUTPUT\" source=\"#" + name + "_" + animationName + "-output\"/>" << Qt::endl;
    fileOut << "          <input semantic=\"INTERPOLATION\" source=\"#" + name + "_" + animationName + "-interpolation\"/>" << Qt::endl;
    fileOut << "        </sampler>" << Qt::endl;
    fileOut << "        <channel source=\"#" + name + "_" + animationName + "-sampler\" target=\"" + name + "/translate\"/>" << Qt::endl;
    fileOut << "      </animation>" << Qt::endl;
}
