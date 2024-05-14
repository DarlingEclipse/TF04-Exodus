#ifndef ISOBUILDER_H
#define ISOBUILDER_H

#include <QString>
#include <QProcess>
#include <QDirIterator>

/*This will interface with 7zip, ImgBurn, and the python TF04 ISO Manager to unpack and repack modded versions of the game
This will require that the settings system is set up so users only have to find their 7zip and imgburn installations once*/

class ProgWindow;

class IsoBuilder {
public:
    ProgWindow *parent;
    QString sevenZipPath;
    QString imgBurnPath;
    QString inputISOPath;
    QString outputISOPath;
    QString randomizerInput = "";
    QString randomizerOutput = "";

    int unpackISO(); //calls TF04 ISO Manager to unpack and attempt to unzip TFA etc
    int unzipSpecial(); //calls 7zip to unzip TFA if the ISO Manager was unable to
    int rezipTFA_isoManager(bool removeFiles); //calls TF04 ISO Manager to repack TFA etc
    int rezipTFA_sevenZip(bool removeFiles); //calls 7zip to repack TFA etc
    int repackISO(bool removeFiles); //calls imgburn to rebuild a new ISO
    int packRandomizer(); //combines randomized files with an existing vanilla unpack, then packs that into an ISO
    int getFileCount(QDir dirToCount);

    void handleOutputCode(QString output);
};

#endif // ISOBUILDER_H
