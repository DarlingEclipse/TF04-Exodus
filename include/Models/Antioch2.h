#ifndef ANTIOCH2_H
#define ANTIOCH2_H

#include <QVector>
#include <QQuaternion>

class VBIN;

class NeutralData{
public:
    int minimumRenderLOD;
};

class AnimationBase{
public:
    VBIN *file;
    QString name; //mesh name
    long sectionLength;
    long sectionEnd;
    QString animationType;
    QString motionType; //Linear, cubic, scalar, etc
    //DAE supported interpolation types: Constant, Linear, Bezier

    int version1;
    int minimumLOD;
    int version2;
    int version3;
    int keyframeCount;
    int subVersion;
    bool has4DSpline;
    bool hasPackedSpline;

    float maxTime;

    std::vector<float> keyframeTimes;

    std::vector<QVector3D> vectorList;
    std::vector<QQuaternion> rotationList;
    std::vector<QVector3D> velocityList;
    std::vector<QQuaternion> velocityList2;
    float scale;
    bool hasScalar;

    static AnimationBase *makeAnimation(QString animationSignature);
    float findSplineAtTime(float time);
    virtual void readLinear();
    virtual void readCubic();
    virtual void readCubicC2();
    virtual void writeAnimationChannelDAE(QTextStream &fileOut, QString animationName);


    void readScalar();

};

class AnimationTranslation : public AnimationBase {
public:
    void readLinear();
    void readCubic();
    void readCubicC2();
    void writeAnimationChannelDAE(QTextStream &fileOut, QString animationName);
};

class AnimationOrientation : public AnimationBase {
    void readLinear();
    void readCubic();
    void readCubicC2();
    void writeAnimationChannelDAE(QTextStream &fileOut, QString animationName);
};

class AnimationScale : public AnimationBase {
};

class AnimationStream{
public:
    VBIN* file;
    long fileLocation;
    QString name; //animation name
    long sectionLength;
    long sectionEnd;

    int version1;
    int minimumLOD;
    int version2;
    int animationPlaybackType;
    float defaultPlaybackSpeed;
    int channelCount;

    std::vector<AnimationBase*> channelArray;

    void readAnimationStream();
    void writeAnimationStreamDAE(QTextStream &fileOut);
};

class AnimationSourceSet{
  public:
    VBIN* file;
    long fileLocation;
    QString name;
    long sectionLength;
    long sectionEnd;
    int m_selectedAnimation;
    int m_selectedFrame; //this should be moved to the animation itself

    int version;
    int sourceCount;

    std::vector<AnimationStream*> streamArray;

    void readAnimationSet();
    void writeAnimationsDAE(QTextStream &fileOut);
};

#endif // ANTIOCH2_H
