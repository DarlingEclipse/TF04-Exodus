
#include <QPushButton>
#include <QByteArrayMatcher>

/*Find a way to remove the file dialog requirement*/
#include <QFileDialog>

/*This should be handled by exWidnow instead*/
#include <QHeaderView>

#include "Headers/Textures/itf.h"
#include "Headers/Main/exDebugger.h"
#include "Headers/UI/exWindow.h"
#include "Headers/Main/BinChanger.h"

//https://ps2linux.no-ip.info/playstation2-linux.com/docs/howto/display_docef7c.html?docid=75

void ITF::load(QString fromType){
    int failedRead = 0;
    if(fromType == "ITF"){
        failedRead = readDataITF();
    } else {
        mipMaps.resize(1);
        failedRead = !mipMaps[0].load(inputPath);
        adaptProperties();
    }
    if(failedRead){
        m_Debug->MessageError("There was an error reading " + fileName);
        return;
    }
}

void ITF::save(QString toType){
    if(toType == "ITF"){
        writeITF();
    } else {
        //writeBMP();
        //only saves base image, currently no support for lower mipmap exports
        //changeColorTable(false);
        mipMaps[0].save(outputPath);
        //changeColorTable(true);
    }
}

void ITF::updateCenter(){
    m_UI->ClearWindow();

    currentPalette = 0;
    if(hasPalette){
        comboPalettes = new QComboBox(m_UI->m_centralContainer);
        comboPalettes -> setGeometry(QRect(QPoint(250,50), QSize(150,30)));
        if (paletteCount <= 0){
            comboPalettes->insertItem(0, "1");
        } else {
            for(int i=0; i<paletteCount; ++i){
                comboPalettes->insertItem(i, QString::number(i+1));
            }
        }
        QAbstractButton::connect(comboPalettes, &QComboBox::currentIndexChanged, m_UI, [this](int index) {selectPalette(index);});
        //QAbstractButton::connect(comboPalettes, &QComboBox::currentIndexChanged, parent, [this] {populatePalette();});
        comboPalettes->show();
        m_UI->m_currentWidgets.push_back(comboPalettes);
        m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(comboPalettes->geometry(), "Choose palette:", m_UI->m_centralContainer));

        paletteTable = new QTableWidget(mipMaps[0].colorCount()/paletteCount, 7, m_UI->m_centralContainer);
        paletteTable->setGeometry(QRect(QPoint(50,250), QSize(125*7,300)));
        QAbstractButton::connect(paletteTable, &QTableWidget::cellChanged, m_UI, [this](int row, int column) {editPalette(row, column);});
        paletteTable->show();
        m_UI->m_currentWidgets.push_back(paletteTable);
        m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(paletteTable->geometry(), "Current palette:", m_UI->m_centralContainer));
        populatePalette();

        QPushButton* buttonRemovePalette = new QPushButton("Convert to Color", m_UI->m_centralContainer);
        buttonRemovePalette->setGeometry(QRect(QPoint(450,50), QSize(150,30)));
        QAbstractButton::connect(buttonRemovePalette, &QPushButton::released, m_UI, [this]{promptIndexToColor();});
        buttonRemovePalette->show();
        m_UI->m_currentWidgets.push_back(buttonRemovePalette);

        QPushButton* buttonImportPalette = new QPushButton("Import new palette", m_UI->m_centralContainer);
        buttonImportPalette->setGeometry(QRect(QPoint(50,50), QSize(150,30)));
        QAbstractButton::connect(buttonImportPalette, &QPushButton::released, m_UI, [this]{importPalette();});
        buttonImportPalette->show();
        m_UI->m_currentWidgets.push_back(buttonImportPalette);
    } else {
        QPushButton* buttonAddPalette = new QPushButton("Convert to 8bpp", m_UI->m_centralContainer);
        buttonAddPalette->setGeometry(QRect(QPoint(250,50), QSize(150,30)));
        QAbstractButton::connect(buttonAddPalette, &QPushButton::released, m_UI, [this]{promptColorToIndex();});
        buttonAddPalette->show();
        m_UI->m_currentWidgets.push_back(buttonAddPalette);
    }

    //allow the user to preview the swizzled image (useful for debugging)
    QComboBox *comboSwizzle = new QComboBox(m_UI->m_centralContainer);
    comboSwizzle -> setGeometry(QRect(QPoint(250,120), QSize(150,30)));
    comboSwizzle->insertItem(0, "Unswizzled");
    comboSwizzle->insertItem(1, "Swizzled");
    comboSwizzle->setCurrentIndex(swizzled);
    QAbstractButton::connect(comboSwizzle, &QComboBox::currentIndexChanged, m_UI, [this](int index) {changeSwizzleType(index);});
    comboSwizzle->show();
    m_UI->m_currentWidgets.push_back(comboSwizzle);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(comboSwizzle->geometry(), "Preview image as:", m_UI->m_centralContainer));

    //display dropdown to select # of maps, button to generate maps
    QComboBox *userMipMaps = new QComboBox(m_UI->m_centralContainer);
    userMipMaps -> setGeometry(QRect(QPoint(650,50), QSize(150,30)));
    int maxMips = int(fmin(log(mipMaps[0].width())/log(2), log(mipMaps[0].height())/log(2)));
    for(int i=0; i<maxMips; ++i){
        userMipMaps->insertItem(i, QString::number(i+1));
    }
    qDebug() << Q_FUNC_INFO << "selected texture has mipmaps?" << hasMipmaps << "if true, how many:" << mipmapCount << "with potential for" << maxMips << "maps. dropdown contains" << userMipMaps->count() << "items";
    QAbstractButton::connect(userMipMaps, &QComboBox::currentIndexChanged, m_UI, [this](int index) {selectMipMap(index+1);});
    userMipMaps->show();
    userMipMaps->setCurrentIndex(mipmapCount - 1);
    m_UI->m_currentWidgets.push_back(userMipMaps);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(userMipMaps->geometry(), "# of MipMaps to add:", m_UI->m_centralContainer));

    QPushButton* buttonAddMipmaps = new QPushButton("Add MipMaps", m_UI->m_centralContainer);
    buttonAddMipmaps->setGeometry(QRect(QPoint(850,50), QSize(150,30)));
    QAbstractButton::connect(buttonAddMipmaps, &QPushButton::released, m_UI, [this]{createMipMaps(currentMipMaps);});
    buttonAddMipmaps->show();
    m_UI->m_currentWidgets.push_back(buttonAddMipmaps);


    updatePreview();
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(QRect(QPoint(50, 600), QSize(0, 0)), "Preview image:", m_UI->m_centralContainer));

    QComboBox *comboAlphaType = new QComboBox(m_UI->m_centralContainer);
    comboAlphaType -> setGeometry(QRect(QPoint(650,120), QSize(150,30)));
    for(int i=0; i<alphaTypes.size(); ++i){
        comboAlphaType->insertItem(i, alphaTypes[i]);
    }
    comboAlphaType->setCurrentIndex(alphaType);
    QAbstractButton::connect(comboAlphaType, &QComboBox::currentIndexChanged, m_UI, [this](int index) {convertAlphaType(index);});
    comboAlphaType->show();
    m_UI->m_currentWidgets.push_back(comboAlphaType);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(comboAlphaType->geometry(), "Current alpha type:", m_UI->m_centralContainer));
    qDebug() << Q_FUNC_INFO << "reached the end up updatecenter";


    // pixmapImageData = QPixmap::fromImage(mipMaps[0]);

    // labelImageDisplay = new QLabel(m_UI->m_centralContainer);
    // labelImageDisplay->setPixmap(pixmapImageData);
    // labelImageDisplay->setGeometry(QRect(QPoint(50, 600), QSize(mipMaps[0].width(), mipMaps[0].height())));
    // labelImageDisplay->show();
    // m_UI->m_currentWidgets.push_back(labelImageDisplay);

}

void ITF::updatePreview(){
    //this is far from ideal - if this is called too often, currentModeWidgets will be filled with junk pointers

    //skipping 1 - mipmap of 1 is the base image
    int mapOffset = 0;
    labelMipMaps.clear();
    for(int i = 0; i < mipmapCount; i++){
        QPixmap mipMapData = QPixmap::fromImage(mipMaps[i]);
        QLabel *displayMipMap = new QLabel(m_UI->m_centralContainer);
        displayMipMap->setPixmap(mipMapData);
        qDebug() << Q_FUNC_INFO << "displaying mipmap at" << mipMaps[i].width()*mapOffset << "given factors width" << mipMaps[i].width() << "mapoffset" << mapOffset;
        displayMipMap->setGeometry(QRect(QPoint(50 + (mipMaps[i].width()*mapOffset), 600), QSize(mipMaps[i].width(), mipMaps[i].height())));
        displayMipMap->show();
        m_UI->m_currentWidgets.push_back(displayMipMap);
        labelMipMaps.push_back(displayMipMap);
        mapOffset += pow(2, i+1);
    }
}

void ITF::changeColorTable(bool input){
    QList<QRgb> tempColorTable = mipMaps[0].colorTable();
    if(bytesPerPixel() == 8 and input){
        int k = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k+0]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k+8]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k-8]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k+0]);
                k++;
            }
        }
    } else if (bytesPerPixel() == 8){
        int k = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k+0]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k+8, tempColorTable[k]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k-8, tempColorTable[k]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                mipMaps[0].setColor(k, tempColorTable[k+0]);
                k++;
            }
        }
    }
}

void ITF::changeSwizzleType(int index){
    if(index & !swizzled){
        swizzle();
    } else if (swizzled){
        unswizzle();
    }
    qDebug() << Q_FUNC_INFO << "attempting to update center";
    updatePreview();
}

void ITF::convertGradientAlpha(QImage *imageData){
    //opaque to blended: all alpha values need to be set to 255
    //punchthrough to blended: all < 255 alpha values need to be set to 0
    if(hasPalette){
        QList<QRgb> tempColorTable = imageData->colorTable();
        for(int i = 0; i < imageData->colorCount(); i++){
            if(alphaType == 2 and qAlpha(tempColorTable[i]) < 255){
                imageData->setColor(i, qRgba(qRed(tempColorTable[i]), qGreen(tempColorTable[i]),qBlue(tempColorTable[i]),0));
            } else {
                imageData->setColor(i, qRgba(qRed(tempColorTable[i]), qGreen(tempColorTable[i]),qBlue(tempColorTable[i]),255));
            }
            imageData->setColor(i, tempColorTable[i]);
        }
    } else {
        for(int i = 0; i < imageData->width(); i++){
            for(int j = 0; j<imageData->height(); j++){
                QColor tempColor = imageData->pixelColor(i,j);
                if(alphaType == 2 and tempColor.alpha() < 255){
                    tempColor.setAlpha(0);
                } else {
                    tempColor.setAlpha(255);
                }
                imageData->setPixelColor(i,j, tempColor);
            }
        }
    }
}
void ITF::convertOpaqueAlpha(QImage *imageData){
    //alpha values just need to be removed (set to 255)
    if(hasPalette){
        QList<QRgb> tempColorTable = imageData->colorTable();
        for(int i = 0; i < imageData->colorCount(); i++){
            imageData->setColor(i, qRgba(qRed(tempColorTable[i]), qGreen(tempColorTable[i]),qBlue(tempColorTable[i]),255));
        }
    } else {
        for(int i = 0; i < imageData->width(); i++){
            for(int j = 0; j<imageData->height(); j++){
                QColor tempColor = imageData->pixelColor(i,j);
                tempColor.setAlpha(255);
                imageData->setPixelColor(i,j, tempColor);
            }
        }
    }
}
void ITF::convertPunchthroughAlpha(QImage *imageData){
    //not sure what the best way to handle this is
    //maybe an alpha threshold goes to 0? maybe a specific color?
    //for now, all <255 alpha values become 0
    if(hasPalette){
        QList<QRgb> tempColorTable = imageData->colorTable();
        for(int i = 0; i < imageData->colorCount(); i++){
            if(qAlpha(tempColorTable[i]) < 255){
                imageData->setColor(i, qRgba(qRed(tempColorTable[i]), qGreen(tempColorTable[i]),qBlue(tempColorTable[i]),0));
            }
            imageData->setColor(i, tempColorTable[i]);
        }
    } else {
        for(int i = 0; i < imageData->width(); i++){
            for(int j = 0; j<imageData->height(); j++){
                QColor tempColor = imageData->pixelColor(i,j);
                if(tempColor.alpha() < 255){
                    tempColor.setAlpha(0);
                }
                imageData->setPixelColor(i,j, tempColor);
            }
        }
    }
}

void ITF::convertAlphaType(int index){
    //so much nesting. See if there's a better way.
    //a note: alpha value runs from 0 to 255, with 0 being more transparent and 255 being more opaque
    switch(index){
        case 0:
        //changing to blended alpha
        for(int m = 0; m < mipmapCount; m++){
            convertGradientAlpha(&mipMaps[m]);
        }
        break;

        case 1:
        //changing to opaque
        for(int m = 0; m < mipmapCount; m++){
            convertOpaqueAlpha(&mipMaps[m]);
        }
        break;

        case 2:
        //change to punchthrough
        for(int m = 0; m < mipmapCount; m++){
            convertPunchthroughAlpha(&mipMaps[m]);
        }
        break;
    }
    alphaType = index;
    updatePreview();
    //pixmapImageData = QPixmap::fromImage(mipMaps[0]);
    //labelImageDisplay->setPixmap(pixmapImageData);
}

void ITF::promptColorToIndex(){
    m_UI->ClearWindow();
    QImage indexedImage = mipMaps[0].convertToFormat(QImage::Format_Indexed8); //no way to do 4bpp, as far as I can find
    //aside from writing a whole algorithm for it on my own, and, uh. I'll pass on that for now.
    QPixmap indexedData = QPixmap::fromImage(indexedImage);
    QPixmap originalData = QPixmap::fromImage(mipMaps[0]);

    QLabel *originalDisplay = new QLabel(m_UI->m_centralContainer);
    originalDisplay->setPixmap(originalData);
    originalDisplay->setGeometry(QRect(QPoint(50, 300), QSize(mipMaps[0].width(), mipMaps[0].height())));
    originalDisplay->show();
    m_UI->m_currentWidgets.push_back(originalDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(originalDisplay->geometry(), "Current Image:", m_UI->m_centralContainer));

    QLabel *indexedDisplay = new QLabel(m_UI->m_centralContainer);
    indexedDisplay->setPixmap(indexedData);
    indexedDisplay->setGeometry(QRect(QPoint(100 + mipMaps[0].width(), 300), QSize(mipMaps[0].width(), mipMaps[0].height())));
    indexedDisplay->show();
    m_UI->m_currentWidgets.push_back(indexedDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(indexedDisplay->geometry(), "8bpp Image:", m_UI->m_centralContainer));

    QPushButton* buttonConfirm = new QPushButton("Convert Image", m_UI->m_centralContainer);
    buttonConfirm->setGeometry(QRect(QPoint(250,50), QSize(150,30)));
    QAbstractButton::connect(buttonConfirm, &QPushButton::released, m_UI, [this]{convertColorToIndex(false);});
    buttonConfirm->show();
    m_UI->m_currentWidgets.push_back(buttonConfirm);

    QPushButton* buttonCancel = new QPushButton("Cancel", m_UI->m_centralContainer);
    buttonCancel->setGeometry(QRect(QPoint(450,50), QSize(150,30)));
    QAbstractButton::connect(buttonCancel, &QPushButton::released, m_UI, [this]{convertColorToIndex(true);});
    buttonCancel->show();
    m_UI->m_currentWidgets.push_back(buttonCancel);

}

void ITF::convertColorToIndex(bool cancelled){
    m_UI->ClearWindow();
    if(!cancelled){
        mipMaps[0] = mipMaps[0].convertToFormat(QImage::Format_Indexed8);
        hasPalette = true;
        paletteCount = 1;
        propertyByte |= 11;
    }
    updateCenter();
}

void ITF::promptIndexToColor(){
    m_UI->ClearWindow();
    QImage colorImage = mipMaps[0].convertToFormat(QImage::Format_ARGB32); //going straight to 32
    QPixmap colorData = QPixmap::fromImage(colorImage);
    QPixmap originalData = QPixmap::fromImage(mipMaps[0]);

    QLabel *originalDisplay = new QLabel(m_UI->m_centralContainer);
    originalDisplay->setPixmap(originalData);
    originalDisplay->setGeometry(QRect(QPoint(50, 300), QSize(mipMaps[0].width(), mipMaps[0].height())));
    originalDisplay->show();
    m_UI->m_currentWidgets.push_back(originalDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(originalDisplay->geometry(), "Current Image:", m_UI->m_centralContainer));

    QLabel *colorDisplay = new QLabel(m_UI->m_centralContainer);
    colorDisplay->setPixmap(colorData);
    colorDisplay->setGeometry(QRect(QPoint(100 + mipMaps[0].width(), 300), QSize(mipMaps[0].width(), mipMaps[0].height())));
    colorDisplay->show();
    m_UI->m_currentWidgets.push_back(colorDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(colorDisplay->geometry(), "Color (non-indexed) Image:", m_UI->m_centralContainer));

    QPushButton* buttonConfirm = new QPushButton("Convert Image", m_UI->m_centralContainer);
    buttonConfirm->setGeometry(QRect(QPoint(250,50), QSize(150,30)));
    QAbstractButton::connect(buttonConfirm, &QPushButton::released, m_UI, [this]{convertIndexToColor(false);});
    buttonConfirm->show();
    m_UI->m_currentWidgets.push_back(buttonConfirm);

    QPushButton* buttonCancel = new QPushButton("Cancel", m_UI->m_centralContainer);
    buttonCancel->setGeometry(QRect(QPoint(450,50), QSize(150,30)));
    QAbstractButton::connect(buttonCancel, &QPushButton::released, m_UI, [this]{convertIndexToColor(true);});
    buttonCancel->show();
    m_UI->m_currentWidgets.push_back(buttonCancel);

}

void ITF::convertIndexToColor(bool cancelled){
    m_UI->ClearWindow();
    if(!cancelled){
        mipMaps[0] = mipMaps[0].convertToFormat(QImage::Format_ARGB32);
        hasPalette = false;
    }
    updateCenter();
}

void ITF::selectMipMap(int mipmapCount){
    currentMipMaps = mipmapCount;
    qDebug() << Q_FUNC_INFO << "mipmap count changed to:" << currentMipMaps;
}

void ITF::selectPalette(int palette){
    int paletteChange = palette - currentPalette;
    currentPalette = palette;
    qDebug() << Q_FUNC_INFO << "new palette selected. Index:" << palette << "setting current palette to" << currentPalette;
    int indexOffset = paletteChange * mipMaps[0].colorCount()/paletteCount;
    //QImage changedPalette = QImage(mipMaps[0]);

    for(int i = 0; i < mipMaps[0].height(); i++){
        for(int j = 0; j < mipMaps[0].width(); j++){
            mipMaps[0].setPixel(j, i, mipMaps[0].pixelIndex(j,i)+indexOffset);
        }
    }



    updatePreview();
    //pixmapImageData = QPixmap::fromImage(changedPalette);
    //labelImageDisplay->setPixmap(pixmapImageData);

    populatePalette();
}

int ITF::importPalette(){
    QString filePath = QFileDialog::getOpenFileName(m_UI, m_UI->tr(QString("Select image file.").toStdString().c_str()), QDir::currentPath());
    QImage importData = QImage(filePath);
    qDebug() << Q_FUNC_INFO << "Importing image of format" << importData.format();
    if(importData.format() != QImage::Format_Indexed8){
        m_Debug->MessageError("Invalid image format. The image will be converted to the proper format, but this may cause unintended results.");
        importData = importData.convertToFormat(QImage::Format_Indexed8);
    }

    int currentColors = mipMaps[0].colorCount();
    int colorsPerPalette = mipMaps[0].colorCount()/paletteCount;
    int addedColors = importData.colorCount();

    qDebug() << Q_FUNC_INFO << "current color count" << currentColors << "plus imported color count" << addedColors;

    if(addedColors > colorsPerPalette){
        qDebug() << Q_FUNC_INFO << "Colors per palette:" << colorsPerPalette << "new texture has" << addedColors << "colors";
        m_Debug->MessageError("Imported image has more colors per palette than the current texture. This is not currently supported.");
        return 1;
        //maybe at some point the base color table can be expanded to allow for this compatibility, but I don't see much of a reason for that
        //the pixel indecies won't change, so only the original palette's size will be visible.
        //image editing should be done in another program anyway.
    } else if (addedColors < colorsPerPalette){
        m_Debug->MessageError("Imported image has fewer colors per palette than the current texture. This is the aforementioned unintended results.");
        importData.setColorCount(colorsPerPalette);
        addedColors = importData.colorCount();
    }
    if(addedColors > 256){
        qDebug() << Q_FUNC_INFO << "imported palette color count" << addedColors;
        m_Debug->MessageError("The imported image's color palette has too many colors (let Everett know, this shouldn't be possible).");
    }

    mipMaps[0].setColorCount(currentColors + addedColors);
    QList<QRgb> tempColorTable = importData.colorTable();
    for(int i = 0; i < addedColors; i++){
        mipMaps[0].setColor(currentColors+i, tempColorTable[i]);
    }
    if(!hasPalette){
        hasPalette = true;
    }
    paletteCount += 1;
    comboPalettes->addItem(QString::number(paletteCount));
    comboPalettes->setCurrentIndex(paletteCount-1);

    return 0;
}

int ITF::bytesPerPixel(){
    int checkProperties = 0;
    /*if(swizzled){
        checkProperties = propertyByte - 128;
    } else {
        checkProperties = propertyByte;
    }*/
    checkProperties = propertyByte ^ 0x10000000;
    //qDebug() << Q_FUNC_INFO << "values:" << propertyByte << checkProperties << (checkProperties & 12) << (checkProperties & 11) << (checkProperties & 10) << (checkProperties & 7) << (checkProperties & 2);

    if ((checkProperties & 12) == 12){
        qDebug() << Q_FUNC_INFO << "24 bpp";
        return 24;
    } else if ((checkProperties & 11) == 11){
        qDebug() << Q_FUNC_INFO << "8 bpp";
        return 8;
    } else if ((checkProperties & 10) == 10){
        qDebug() << Q_FUNC_INFO << "4 bpp";
        return 4;
    } else if ((checkProperties & 7) == 7){
        qDebug() << Q_FUNC_INFO << "16 bpp";
        return 16;
    } else if((checkProperties & 2) == 2){
        qDebug() << Q_FUNC_INFO << "32 bpp";
        return 32;
    }

    return 0;
}

void ITF::createMipMaps(int mipmapLevels){
//    qDebug() << Q_FUNC_INFO << "width: " << mipMaps[0].width() << "is the" << log(mipMaps[0].width())/log(2) <<"th power of 2";
//    qDebug() << Q_FUNC_INFO << "maximum number of mipmaps is" << int(fmin(log(mipMaps[0].width())/log(2), log(mipMaps[0].height())/log(2)));
    qDebug() << Q_FUNC_INFO << "requested maps:" << mipmapLevels;
    int maxLevels = int(fmin(log(mipMaps[0].width())/log(2), log(mipMaps[0].height())/log(2)));
    if(mipmapLevels > maxLevels){
        m_Debug->MessageError("Too many mipmaps. Maximum maps for this texture: " + QString::number(maxLevels) + ". Entered maps: " + QString::number(mipmapLevels));
    }
    QImage storeImage = QImage(mipMaps[0]);

    mipMaps.clear(); //if the image already has maps, they need to be cleaned out to make room for the new ones.
    mipMaps.push_back(storeImage); //this feels gross though, find a better way to clear the extras.
    if(swizzled){
        m_Debug->MessageError("Texture is currently swizzled. Unswizzling before generating new mipmaps.");
        unswizzle();
    }
    if(mipmapLevels == 1){
        qDebug() << Q_FUNC_INFO << "Only one mipmap level selected - this is the base texture. Existing maps are being cleared, new maps will not be generated.";
        hasMipmaps = false;
        mipmapCount = mipmapLevels;
        updateCenter();
        return;
    }

    for(int i = 1; i < mipmapLevels; i++){
        qDebug() << Q_FUNC_INFO << "scaling image from" << mipMaps[0].width() << "x" << mipMaps[0].height() << "to" << mipMaps[0].width()/pow(2,i) << "x" << mipMaps[0].height()/pow(2,i);
        mipMaps.push_back(QImage(mipMaps[0].scaled(mipMaps[0].width()/pow(2,i), mipMaps[0].height()/pow(2,i), Qt::KeepAspectRatio)));
    }

    hasMipmaps = true;
    mipmapCount = mipmapLevels;

    updateCenter();

}

void ITF::readPalette(){
    int colorsPerPalette = 0;
    QList<QRgb> forceTable;
    if (bytesPerPixel() < 16){
        hasPalette = true;
        colorsPerPalette = std::pow(2, bytesPerPixel());
    } else {
        hasPalette = false;
    }

    //qDebug() << Q_FUNC_INFO << "Color count: " << colorsPerPalette << "palette count:" << paletteCount;
    //qDebug() << Q_FUNC_INFO << "color count before:" << mipMaps[0].colorCount() << ". should be set to" << colorsPerPalette*paletteCount;
    mipMaps[0].setColorCount(colorsPerPalette*paletteCount);
    for(int i = 0; i < mipMaps[0].colorCount(); i++){
        int red = fileData->readUInt(1);
        int green = fileData->readUInt(1);
        int blue = fileData->readUInt(1);
        int alpha = fileData->readUInt(1);
        if(alphaType == 1){
            //opaque texture, no alpha
            alpha = 255;
        } else if (alphaType == 2){
            //punchthrough texture, either full alpha or no alpha
            if (alpha < 128){
                alpha = 0;
            } else {
                alpha = 255;
            }
        }

        QRgb nextColor = qRgba(red, green, blue, alpha);
        mipMaps[0].setColor(i, nextColor);
        //forceTable.push_back(nextColor);
    }

    changeColorTable(true);

}

void ITF::readIndexedData(){
    int pixelIndex = 0;
    //int contentLength = 0;
    long location = fileData->currentPosition;
    std::tuple <int8_t, int8_t> nibTup;
    for(int m = 0; m < mipmapCount; m++){
        if (bytesPerPixel() == 8){
            //8bpp, 256 palette case. nice and easy since each pixel uses 1 byte to refer to the palette
            for (int i = 0; i < mipMaps[m].width() * mipMaps[m].height(); i++){
                //qDebug() << Q_FUNC_INFO << "current pixel " << i << "x" << i%width << "y" << i/width;
                mipMaps[m].setPixel(i % mipMaps[m].width(), i / mipMaps[m].width(), fileData->readUInt(1));
            }

        } else {
            //4bpp case, 16 color palette. this is tougher since each pixel is only half a byte (nibble?) and we can only refer to whole bytes.
            //however every image should be an even number of pixels so we can just grab them in pairs.
            for (int i = 0; i < (mipMaps[m].width() * mipMaps[m].height())/2; i++){
                //qDebug() << Q_FUNC_INFO << "current pixel pair" << i << "x" << pixelIndex%width << "y" << pixelIndex/width;
                nibTup = BinChanger::byte_to_nib(fileData->mid(location+i, 1));
                mipMaps[m].setPixel(pixelIndex % mipMaps[m].width(), pixelIndex / mipMaps[m].width(), std::get<0>(nibTup));
                pixelIndex += 1;
                mipMaps[m].setPixel(pixelIndex % mipMaps[m].width(), pixelIndex / mipMaps[m].width(), std::get<1>(nibTup));
                pixelIndex += 1;
            }
        }
    }

}

void ITF::readImageData(){
    //these all store their color values directly instead of referring to a palette
    //int contentLength = 0;
    uint32_t combinedIntensity = 0;
    int currentWidth = 0;
    int currentHeight = 0;

    for(int m = 0; m < mipmapCount; m++){
        currentHeight = mipMaps[0].height()/pow(2,m);
        currentWidth = mipMaps[0].width()/pow(2,m);
        switch(bytesPerPixel()){
        case 16:
        qDebug() << Q_FUNC_INFO << "16bpp";
        //16bpp, each pixel has its r,g,b, and a values stored as 4 integers packed into 2 bytes
        //contentLength = dataLength / 2;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            combinedIntensity = fileData->readUInt(2);
            //combinedIntensity = fileData.readUInt(1) + fileData.readUInt(1); //this absolutely should not be necessary
            //int alpha = (combinedIntensity >> 15);
            int alpha = (combinedIntensity >> 15) & 1;
            if(alphaType == 1){
                //opaque texture, no alpha
                alpha = 255;
            } else if (alphaType == 2){
                //punchthrough texture, either full alpha or no alpha
                if (alpha < 128){
                    alpha = 0;
                } else {
                    alpha = 255;
                }
            }
            QColor currentPixel = qRgba(((combinedIntensity >> 0) & 31)*8, ((combinedIntensity >> 5) & 31)*8, ((combinedIntensity >> 10) & 31)*8, alpha);
            //qDebug() << Q_FUNC_INFO << "setting color at x" << i % currentWidth << "y" << i / currentWidth << "to" << currentPixel;
            mipMaps[m].setPixelColor(i % currentWidth, i / currentWidth, currentPixel);
        }
        break;

        case 24:
        qDebug() << Q_FUNC_INFO << "24bpp";
        //24bpp, each pixel has its r,g, and b values stored as single-byte integers
        //contentLength = dataLength / 3;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            QColor currentPixel;
            currentPixel.setRed(fileData->readUInt(1));
            currentPixel.setGreen(fileData->readUInt(1));
            currentPixel.setBlue(fileData->readUInt(1));
            currentPixel.setAlpha(255); //don't have to worry about alpha type with this - alpha is always 0
            //qDebug() << Q_FUNC_INFO << "setting color at x" << i % currentWidth << "y" << i / currentWidth << "to" << currentPixel;
            mipMaps[m].setPixelColor(i % currentWidth, i / currentWidth, currentPixel);
        }
        break;

        case 32:
        qDebug() << Q_FUNC_INFO << "32bpp";
        //32bpp, each pixel has its r,g,b, and a values stored as single-byte integers
        //contentLength = dataLength / 4;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            QColor currentPixel;
            currentPixel.setRed(fileData->readUInt(1));
            currentPixel.setGreen(fileData->readUInt(1));
            currentPixel.setBlue(fileData->readUInt(1));
            int alpha = fileData->readUInt(1);
            if(alphaType == 1){
                //opaque texture, no alpha
                alpha = 255;
            } else if (alphaType == 2){
                //punchthrough texture, either full alpha or no alpha
                if (alpha < 128){
                    alpha = 0;
                } else {
                    alpha = 255;
                }
            }
            currentPixel.setAlpha(alpha);
            //qDebug() << Q_FUNC_INFO << "setting color at x" << i % currentWidth << "y" << i / currentWidth << "to" << currentPixel;
            mipMaps[m].setPixelColor(i % currentWidth, i / currentWidth, currentPixel);
        }
        break;

        default:
        m_Debug->MessageError("Unknown bit depth");
        }
    }
}

void ITF::adaptProperties(){
    versionNum = 3; //most recent known version - we can experiment to see if the game recognizes a version 4 later
    headerLength = 32;
    propertyByte = 0; //this will be the hard part
    alphaType = 0; //blended alpha by default
    int width = mipMaps[0].width();
    int height = mipMaps[0].height();
    mipmapCount = 1; //base image
    unknown4Byte3 = 0; //this value is 0 in all recorded files. Might be reseved values for later file versions?
    unknown4Byte4 = 0;
    swizzled = false; //assuming imported images are not swizzled

    qDebug() << Q_FUNC_INFO << "Image format:" << mipMaps[0].format();

    if(mipMaps[0].format() == QImage::Format_Indexed8){
        if(mipMaps[0].colorCount() < 15){
            propertyByte = 10; //4bpp
            m_Debug->Log("Image bit depth: 4bpp");
        } else {
            propertyByte = 11; //8bpp
            m_Debug->Log("Image bit depth: 8bpp");
        }
        paletteCount = 1; //additional palettes can be imported
        hasPalette = true;

    } else if (mipMaps[0].format() == QImage::Format_ARGB32 || mipMaps[0].format() == QImage::Format_RGB32){
        //there are probably other formats that need to be included here.
        propertyByte = 2; //32bpp
        m_Debug->Log("Image bit depth: 32bpp");
        hasPalette = false;
        //need to find a way later to compress these down
    } else {
        qDebug() << Q_FUNC_INFO << "Image format:" << mipMaps[0].format();
        m_Debug->Log(&"Image format read as: " [ mipMaps[0].format()]);
        m_Debug->MessageError("Invalid image format.");
    }

    if(int(log(mipMaps[0].width())/log(2)) != log(mipMaps[0].width())/log(2)){
        m_Debug->MessageError("Image width is not a factor of 2");
    }

    if(int(log(mipMaps[0].height())/log(2)) != log(mipMaps[0].height())/log(2)){
        m_Debug->MessageError("Image height is not a factor of 2");
    }

}

int ITF::readDataITF(){

    QByteArray txtrString = "TXTR";
    QByteArrayMatcher matcher(txtrString);
    QTableWidgetItem currentItem;
    long startLocation = 0;
    long contentLength = 0;
    currentPalette = 0;
    fileLength = fileData->readInt(4, 4);
    qDebug() << Q_FUNC_INFO << "reading file" << fileName;

    /*Load header data*/
    fileData->currentPosition = 15;
    versionNum = fileData->readUInt(1);
    headerLength = fileData->readUInt();
    fileData->currentPosition += 3; //skip the "PS2" label
    propertyByte = fileData->readUInt(1);
    alphaType = fileData->readUInt();
    qDebug() << Q_FUNC_INFO << "Alpha type:" << alphaTypes[alphaType];
    int width = fileData->readUInt();
    int height = fileData->readUInt();
    qDebug() << Q_FUNC_INFO << "image height:" << height << "width:" << width;
    mipmapCount = std::max(1, fileData->readUInt()); //some textures say 0 mipmaps. while this is accurate, the design of this program requires at least 1.
    qDebug() << Q_FUNC_INFO << "mipmap count:" << mipmapCount;
    paletteCount = std::max(1, fileData->readUInt()); //some textures say 0 palettes, this catches those. possibly older ITF file version?
    qDebug() << Q_FUNC_INFO << "palette count:" << paletteCount << " found at " << fileData->currentPosition;
    unknown4Byte3 = fileData->readUInt();
    unknown4Byte4 = fileData->readUInt();
    /*End header data.*/

    if(paletteCount > 16){
        //this catch is for the Sarge textures, which claim to have 23 palettes (they don't).
        paletteCount = 1;
    }

    if(bytesPerPixel() < 16){
        hasPalette = true;
    } else {
        hasPalette = false;
        paletteCount = 0;
    }

    if(mipmapCount > 1){
        hasMipmaps = true;
    } else {
        hasMipmaps = false;
    }
    mipMaps.resize(mipmapCount);

    //paletteList.resize(paletteCount);
    swizzled = propertyByte & 128;
    qDebug() << Q_FUNC_INFO << "Swizzled texture?" << swizzled;
    int checkProperties = 0;
    if(swizzled){
        checkProperties = propertyByte - 128;
    } else {
        checkProperties = propertyByte;
    }

    if ((checkProperties & 12) == 12 || (checkProperties & 7) == 7){
        qDebug() << Q_FUNC_INFO << "16 or 24 bpp. Setting format to Format_RGBA8888";
        for(int i = 0; i < mipmapCount; i++){
            mipMaps[i] = QImage(width/pow(2,i), height/pow(2,i), QImage::Format_RGBA8888);
        }
        //imageData = QImage(width, height, QImage::Format_RGBA8888);
    } else if ((checkProperties & 11) == 11 || (checkProperties & 10) == 10){
        qDebug() << Q_FUNC_INFO << "4 or 8 bpp. Setting format to Format_Indexed8";
        for(int i = 0; i < mipmapCount; i++){
            mipMaps[i] = QImage(width/pow(2,i), height/pow(2,i), QImage::Format_Indexed8);
        }
        //imageData = QImage(width, height, QImage::Format_Indexed8);
    } else if((checkProperties & 2) == 2){
        qDebug() << Q_FUNC_INFO << "32 bpp. Setting format to Format_ARGB32";
        for(int i = 0; i < mipmapCount; i++){
            mipMaps[i] = QImage(width/pow(2,i), height/pow(2,i), QImage::Format_ARGB32);
        }
        //imageData = QImage(width, height, QImage::Format_ARGB32);
    }

    fileData->currentPosition = matcher.indexIn(fileData->dataBytes, 0)+4;
    startLocation = fileData->currentPosition; //this will be used later to remove the palette from the content
    int readDataLength = fileData->readInt();
    contentLength = readDataLength + 4;
    qDebug() << Q_FUNC_INFO << "content length: " << contentLength;
    //qDebug() << Q_FUNC_INFO << "bpp" << bytesPerPixel() << "has palette" << hasPalette;

    if(hasPalette){
        readPalette();
        readIndexedData();
    } else {
        readImageData();
    }

    if(swizzled){
        unswizzle();
    }

    //createMipMaps(mipmapCount);

    return 0;
}

void ITF::populatePalette(){
    int paletteIndex = currentPalette;
    int paletteOffset = mipMaps[0].colorCount()/paletteCount * paletteIndex;
    qDebug() << Q_FUNC_INFO << "Function called. Palette index: " << paletteIndex+1 << "out of" << paletteCount;
    qDebug() << Q_FUNC_INFO << "Palette colors: " << paletteOffset << "out of total colors" << mipMaps[0].colorCount();
    QStringList columnNames = {"Palette Index", "Red", "Green", "Blue", "Alpha", "Original", "Current"};
    paletteTable->setHorizontalHeaderLabels(columnNames);
    paletteTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    QList<QRgb> tempColorTable = mipMaps[0].colorTable();
    for(int i = 0; i < mipMaps[0].colorCount()/paletteCount; i++){
        paletteTable->blockSignals(1);
        QTableWidgetItem *cellText0 = paletteTable->item(i,0);
        if (!cellText0){
            cellText0 = new QTableWidgetItem;
            paletteTable->setItem(i,0,cellText0);
        }
        cellText0->setText(QString::number(i));
        QTableWidgetItem *cellText = paletteTable->item(i,1);
        if (!cellText){
            cellText = new QTableWidgetItem;
            paletteTable->setItem(i,1,cellText);
        }
        cellText->setText(QString::number(qRed(tempColorTable[i+paletteOffset])));
        QTableWidgetItem *cellText2 = paletteTable->item(i,2);
        if (!cellText2){
            cellText2 = new QTableWidgetItem;
            paletteTable->setItem(i,2,cellText2);
        }
        cellText2->setText(QString::number(qGreen(tempColorTable[i+paletteOffset])));
        QTableWidgetItem *cellText3 = paletteTable->item(i,3);
        if (!cellText3){
            cellText3 = new QTableWidgetItem;
            paletteTable->setItem(i,3,cellText3);
        }
        cellText3->setText(QString::number(qBlue(tempColorTable[i+paletteOffset])));
        QTableWidgetItem *cellText4 = paletteTable->item(i,4);
        if (!cellText4){
            cellText4 = new QTableWidgetItem;
            paletteTable->setItem(i,4,cellText4);
        }
        cellText4->setText(QString::number(qAlpha(tempColorTable[i+paletteOffset])));
        QTableWidgetItem *cellText5 = paletteTable->item(i,5);
        if (!cellText5){
            cellText5 = new QTableWidgetItem;
            paletteTable->setItem(i,5,cellText5);
        }
        cellText5->setBackground(QColor::fromRgb(tempColorTable[i+paletteOffset]));
        QTableWidgetItem *cellText6 = paletteTable->item(i,6);
        if (!cellText6){
            cellText6 = new QTableWidgetItem;
            paletteTable->setItem(i,6,cellText6);
        }
        cellText6->setBackground(QColor::fromRgb(tempColorTable[i+paletteOffset]));
        paletteTable->blockSignals(0);
    }
}

void ITF::editPalette(int row, int column){
    int changedValue = paletteTable->item(row, column)->text().toInt(nullptr, 10);
    int paletteOffset = mipMaps[0].colorCount()/paletteCount * currentPalette;
    int colorIndex = paletteOffset + row;
    qDebug() << Q_FUNC_INFO << "Changed value: " << paletteTable->item(row, column)->text();
    qDebug() << Q_FUNC_INFO << "Row: " << row << " Column " << column;
    QList<QRgb> tempColorTable = mipMaps[0].colorTable();
    if (changedValue < 256 and changedValue >= 0 ){
        qDebug() << Q_FUNC_INFO << "Valid color value";
        switch (column){
        case 1: mipMaps[0].setColor(colorIndex, qRgba(changedValue, qGreen(tempColorTable[colorIndex]), qBlue(tempColorTable[colorIndex]), qAlpha(tempColorTable[colorIndex]))) ; break;
        case 2: mipMaps[0].setColor(colorIndex, qRgba(qRed(tempColorTable[colorIndex]), changedValue, qBlue(tempColorTable[colorIndex]), qAlpha(tempColorTable[colorIndex]))) ; break;
        case 3: mipMaps[0].setColor(colorIndex, qRgba(qRed(tempColorTable[colorIndex]), qGreen(tempColorTable[colorIndex]), changedValue, qAlpha(tempColorTable[colorIndex]))) ; break;
        case 4: mipMaps[0].setColor(colorIndex, qRgba(qRed(tempColorTable[colorIndex]), qGreen(tempColorTable[colorIndex]), qBlue(tempColorTable[colorIndex]), changedValue)) ; break;
        }
        QTableWidgetItem *cellText5 = paletteTable->item(row, 6);
        cellText5->setBackground(QColor::fromRgb(mipMaps[0].colorTable()[colorIndex]));
        qDebug() << Q_FUNC_INFO << "cell text" << cellText5->text();

        selectPalette(currentPalette);

    } else {
        qDebug() << Q_FUNC_INFO << "Not a valid color value.";
        switch (column){
        case 1: paletteTable->item(row,column)->text() = QString::number(qRed(tempColorTable[colorIndex])); break;
        case 2: paletteTable->item(row,column)->text() = QString::number(qGreen(tempColorTable[colorIndex])); break;
        case 3: paletteTable->item(row,column)->text() = QString::number(qBlue(tempColorTable[colorIndex])); break;
        case 4: paletteTable->item(row,column)->text() = QString::number(qAlpha(tempColorTable[colorIndex])); break;
        }
    }

}

int ITF::dataLength(){
    int dataLength = 0;
    dataLength = mipMaps[0].height() * mipMaps[0].width() * (bytesPerPixel()/8);
    qDebug() << Q_FUNC_INFO << "Data length calculated as:" << dataLength;

    return dataLength;
}

void ITF::writeITF(){
    QFile itfOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if(!swizzled){
        qDebug() << Q_FUNC_INFO << "texture not currently swizzled - re-swizzling before writing.";
        //swizzle();
    }

    changeColorTable(false);

    if (itfOut.open(QIODevice::ReadWrite)){
        QDataStream fileStream(&itfOut);

        qDebug() << Q_FUNC_INFO << "Writing ITF header info";
        itfOut.write("FORM");
        BinChanger::intWrite(itfOut, fileLength);
        itfOut.write("ITF0HDR");
        BinChanger::byteWrite(itfOut, versionNum);
        BinChanger::intWrite(itfOut, headerLength);
        itfOut.write("PS2");
        BinChanger::byteWrite(itfOut, propertyByte);
        BinChanger::intWrite(itfOut, alphaType);
        BinChanger::intWrite(itfOut, mipMaps[0].width());
        BinChanger::intWrite(itfOut, mipMaps[0].height());
        BinChanger::intWrite(itfOut, mipmapCount);
        BinChanger::intWrite(itfOut, paletteCount);
        BinChanger::intWrite(itfOut, unknown4Byte3);
        BinChanger::intWrite(itfOut, unknown4Byte4);
        itfOut.write("TXTR");
        BinChanger::intWrite(itfOut, dataLength());
        QList<QRgb> tempColorTable = mipMaps[0].colorTable();
        if(hasPalette){
            for(int i = 0; i < mipMaps[0].colorCount(); i++){
                BinChanger::byteWrite(itfOut, qRed(tempColorTable[i]));
                BinChanger::byteWrite(itfOut, qGreen(tempColorTable[i]));
                BinChanger::byteWrite(itfOut, qBlue(tempColorTable[i]));
                BinChanger::byteWrite(itfOut, qAlpha(tempColorTable[i]));
            }
            for(int i = 0; i < mipMaps.size(); i++){
                writeIndexedData(itfOut, &mipMaps[i]);
            }
        } else {
            for(int i = 0; i < mipMaps.size(); i++){
                writeImageData(itfOut, &mipMaps[i]);
            }
        }

        //and that should be it

    }
    changeColorTable(true);

}

void ITF::writeIndexedData(QFile& fileOut, QImage *writeData){
    QImage reverseImage = QImage(*writeData);
    std::tuple <int8_t, int8_t> nibTup;

    switch(bytesPerPixel()){
        case 8:
        m_Debug->Log("Exporting to ITF with bit depth: 8bpp");
        for(int i = 0; i < reverseImage.height(); i++){
            for(int j = 0; j < reverseImage.width(); j++){
                BinChanger::byteWrite(fileOut, reverseImage.pixelIndex(j,i));
            }
        }
        break;

        case 4:
        m_Debug->Log("Exporting to ITF with bit depth: 4bpp");
        for(int i = 0; i < reverseImage.height()*reverseImage.width(); i+=2){
            std::get<0>(nibTup) = reverseImage.pixelIndex(i % reverseImage.width(), i / reverseImage.width());
            std::get<1>(nibTup) = reverseImage.pixelIndex((i+1) % reverseImage.width(), (i+1) / reverseImage.width());
            BinChanger::byteWrite(fileOut, BinChanger::nib_to_byte(nibTup));
        }
        break;

        default:
        m_Debug->MessageError("Unknown byte depth");
    }
}

void ITF::writeImageData(QFile& fileOut, QImage *writeData){
    QImage reverseImage = QImage(*writeData);
    int bpp = bytesPerPixel();
    int alpha = 0;
    int combinedIntensities = 0;
    qDebug() << Q_FUNC_INFO << "width:" << reverseImage.width() << "height" << reverseImage.height() << "Total pixels:" << reverseImage.width() * reverseImage.height();
    m_Debug->Log("Exporting to ITF with bit depth: " + QString::number(bpp) + "bpp");
    for(int i = 0; i < reverseImage.height(); i++){
        for(int j = 0; j < reverseImage.width(); j++){
            QColor currentPixel = QColor(reverseImage.pixel(j,i));
            switch(bpp){
                case 16:
                //QColor currentPixel = qRgba(((combinedIntensity >> 0) & 31)*8, ((combinedIntensity >> 5) & 31)*8, ((combinedIntensity >> 10) & 31)*8, alpha);
                alpha = std::min(currentPixel.alpha(), 1);
                combinedIntensities = (currentPixel.red()/8) + ((currentPixel.green()/8) << 5) + ((currentPixel.blue()/8) << 10) + (alpha<<15);
                BinChanger::shortWrite(fileOut, combinedIntensities);
                break;

                case 24:
                BinChanger::byteWrite(fileOut, currentPixel.blue());
                BinChanger::byteWrite(fileOut, currentPixel.green());
                BinChanger::byteWrite(fileOut, currentPixel.red());
                break;

                case 32:
                BinChanger::byteWrite(fileOut, currentPixel.blue());
                BinChanger::byteWrite(fileOut, currentPixel.green());
                BinChanger::byteWrite(fileOut, currentPixel.red());
                BinChanger::byteWrite(fileOut, currentPixel.alpha());
                break;

                default:
                m_Debug->MessageError("Unknown byte depth.");
            }
        }
    }
}

void ITF::unswizzle(){

    //https://gist.github.com/Fireboyd78/1546f5c86ebce52ce05e7837c697dc72
    QImage unswizzledImage;
    int InterlaceMatrix[] = {
        0x00, 0x10, 0x02, 0x12,
        0x11, 0x01, 0x13, 0x03,
    };

    int Matrix[]        = { 0, 1, -1, 0 };
    int TileMatrix[]    = { 4, -4 };


    //to-do: this code seems to work, but the variables should be renamed to actually be useful.

    for(int m = 0; m < mipmapCount; m++){
        unswizzledImage = QImage(mipMaps[m]);
        for (int y = 0; y < mipMaps[m].height(); y++)
        {
            for (int x = 0; x < mipMaps[m].width(); x++)
            {
                int oddRow = ((y & 1) != 0);

                int num1 = ((y / 4) & 1);
                int num2 = ((x / 4) & 1);
                int num3 = (y % 4);

                int num4 = ((x / 4) % 4);

                if (oddRow){
                    num4 += 4;
                }

                int num5 = ((x * 4) % 16);
                int num6 = ((x / 16) * 32);

                int num7 = (oddRow) ? ((y - 1) * mipMaps[m].width()) : (y * mipMaps[m].width());

                int xx = x + num1 * TileMatrix[num2];
                int yy = y + Matrix[num3];

                int i = InterlaceMatrix[num4] + num5 + num6 + num7;
                int j = yy * mipMaps[m].width() + xx;

                //unswizzledImage[j] = pixelList[i];


                if(hasPalette){
                    unswizzledImage.setPixel(j % unswizzledImage.width(), j/unswizzledImage.width(), mipMaps[m].pixelIndex(i % mipMaps[m].width(), i/mipMaps[m].width()));
                } else {
                    unswizzledImage.setPixel(j % unswizzledImage.width(), j/unswizzledImage.width(), mipMaps[m].pixel(i % mipMaps[m].width(), i/mipMaps[m].width()));
                }
            }
        }
        mipMaps[m] = unswizzledImage;
    }
    swizzled = false;

}

void ITF::swizzle(){
    QImage swizzledImage;
    //https://gist.github.com/Fireboyd78/1546f5c86ebce52ce05e7837c697dc72

    //qDebug() << Q_FUNC_INFO << "unswizzled image size" << pixelList.size();
    int InterlaceMatrix[] = {
        0x00, 0x10, 0x02, 0x12,
        0x11, 0x01, 0x13, 0x03,
    };

    int Matrix[]        = { 0, 1, -1, 0 };
    int TileMatrix[]    = { 4, -4 };


    //to-do: this code seems to work, but the variables should be renamed to actually be useful.
    for(int m = 0; m < mipmapCount; m++){
        swizzledImage = QImage(mipMaps[m]);
        for (int y = 0; y < mipMaps[m].height(); y++)
        {
            for (int x = 0; x < mipMaps[m].width(); x++)
            {
                int oddRow = ((y & 1) != 0);

                int num1 = ((y / 4) & 1);
                int num2 = ((x / 4) & 1);
                int num3 = (y % 4);

                int num4 = ((x / 4) % 4);

                if (oddRow){
                    num4 += 4;
                }

                int num5 = ((x * 4) % 16);
                int num6 = ((x / 16) * 32);

                int num7 = (oddRow) ? ((y - 1) * mipMaps[m].width()) : (y * mipMaps[m].width());

                int xx = x + num1 * TileMatrix[num2];
                int yy = y + Matrix[num3];

                int i = InterlaceMatrix[num4] + num5 + num6 + num7;
                int j = yy * mipMaps[m].width() + xx;

                //qDebug() << Q_FUNC_INFO << "x" << x << "y" << y << "i" << i << "j" << j;

                //swizzledImage[i] = pixelList[j];
                if(hasPalette){
                    swizzledImage.setPixel(i % swizzledImage.width(), i/swizzledImage.width(), mipMaps[m].pixelIndex(j % mipMaps[m].width(), j/mipMaps[m].width()));
                } else {
                    swizzledImage.setPixel(i % swizzledImage.width(), i/swizzledImage.width(), mipMaps[m].pixel(j % mipMaps[m].width(), j/mipMaps[m].width()));
                }

            }
        }
        mipMaps[m] = swizzledImage;
    }

    swizzled = true;

}
