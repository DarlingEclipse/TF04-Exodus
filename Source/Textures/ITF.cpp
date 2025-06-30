
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

void exImageData::SetPixel(int index, QColor pixel){
    if(index > m_pixels.size()){
        qDebug() << Q_FUNC_INFO << "Pixel index" << index << "out of range";
        return;
    }
    m_pixels[index] = pixel;
}

void exImageData::SetPixel(int pixelIndex, int colorIndex){
    if(pixelIndex > m_indexedPixels.size()){
        qDebug() << Q_FUNC_INFO << "Pixel index" << pixelIndex << "out of range";
        return;
    }
    m_indexedPixels[pixelIndex] = colorIndex;
}

void exPaletteData::SetColor(int index, QColor color){
    if(index > m_colors.size()){
        qDebug() << Q_FUNC_INFO << "Pixel index" << index << "out of range";
        return;
    }
    m_colors[index] = color;
}

void ITF::load(QString fromType){
    int failedRead = 0;
    if(fromType == "ITF"){
        failedRead = readDataITF();
    } else {
        QImage input(inputPath);
        AdaptImage(input);
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
        QImage output = CreateImage(0);
        output.save(outputPath);
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

        paletteTable = new QTableWidget(pow(2, bytesPerPixel()), 7, m_UI->m_centralContainer);
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
    int maxMips = int(fmin(log(m_width)/log(2), log(m_height)/log(2)));
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
    QAbstractButton::connect(comboAlphaType, &QComboBox::currentIndexChanged, m_UI, [this](int index) {convertAlpha(index);});
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
    int mapOffset = 0;
    labelMipMaps.clear();
    for(int i = 0; i < mipmapCount; i++){
        QLabel *displayMipMap = m_UI->MakeImage(CreateImage(i));
        //QPixmap mipMapData = QPixmap::fromImage(CreateImage(i));
        //QLabel *displayMipMap = new QLabel(m_UI->m_centralContainer);
        //displayMipMap->setPixmap(mipMapData);
        qDebug() << Q_FUNC_INFO << "displaying mipmap at" << MapWidth(i)*mapOffset << "given factors width" << MapWidth(i) << "mapoffset" << mapOffset;
        displayMipMap->setGeometry(QRect(QPoint(50 + (MapWidth(i)*mapOffset), 600), QSize(MapWidth(i), MapHeight(i))));
        displayMipMap->show();
        labelMipMaps.push_back(displayMipMap);
        mapOffset += pow(2, i+1);
    }
}

void ITF::changeColorTable(bool input){
    std::vector<QColor> tempColorTable = m_palettes[0].m_colors;
    if(m_format == ITFProperties_8bpp && input){
        int k = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k+0]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k+8]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k-8]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k+0]);
                k++;
            }
        }
    } else if (m_format == ITFProperties_8bpp){
        int k = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k+0]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k+8, tempColorTable[k]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k-8, tempColorTable[k]);
                k++;
            }
            for(int j = 0; j < 8; j++){
                m_palettes[0].SetColor(k, tempColorTable[k+0]);
                k++;
            }
        }
    }
}

void ITF::changeSwizzleType(int index){
    if(index & !swizzled){
        swizzle();
    } else if (swizzled){
        //unswizzle();
        swizzle(true);
    }
    qDebug() << Q_FUNC_INFO << "attempting to update center";
    updatePreview();
}

void ITF::convertAlpha(int index){
    //opaque to blended: all alpha values need to be set to 255
    //punchthrough to blended: all < 255 alpha values need to be set to 0
    if(hasPalette){
        for(int i = 0; i < m_palettes.size(); i++){
            for(int j = 0; j < m_palettes[i].m_colors.size(); j++){
                QColor replacementColor = m_palettes[i].m_colors[j];
                switch(index){
                case Alpha_Gradient:
                    if(m_palettes[i].m_colors[j].alpha() < 255){
                        replacementColor.setAlpha(0);
                    } else {
                        replacementColor.setAlpha(255);
                        m_palettes[i].SetColor(j, replacementColor);
                    }
                    break;
                case Alpha_Opaque:
                    replacementColor.setAlpha(255);
                    break;
                case Alpha_Punchthrough:
                    if(m_palettes[i].m_colors[j].alpha() < 255){
                        replacementColor.setAlpha(0);
                    }
                    break;
                default:
                    qDebug() << Q_FUNC_INFO << "Undefined alpha type.";
                }
                m_palettes[i].SetColor(j, replacementColor);

            }
        }
    } else {
        for(int i = 0; i < m_mipMaps.size(); i++){
            for(int j = 0; j < m_mipMaps[i].m_pixels.size(); j++){
                QColor replacementColor = m_mipMaps[i].m_pixels[j];
                switch(alphaType){
                case Alpha_Gradient:
                    if(replacementColor.alpha() < 255){
                        replacementColor.setAlpha(0);
                    } else {
                        replacementColor.setAlpha(255);
                    }
                    break;
                case Alpha_Opaque:
                    replacementColor.setAlpha(255);
                    break;
                case Alpha_Punchthrough:
                    if(replacementColor.alpha() < 255){
                        replacementColor.setAlpha(0);
                    }
                    break;
                }
                m_mipMaps[i].SetPixel(j, replacementColor);

            }
        }
    }
    alphaType = index;
    updatePreview();
}

void ITF::promptColorToIndex(){
    m_UI->ClearWindow();
    QImage indexedImage = CreateImage(0).convertToFormat(QImage::Format_Indexed8); //no way to do 4bpp, as far as I can find
    //aside from writing a whole algorithm for it on my own, and, uh. I'll pass on that for now.
    QPixmap indexedData = QPixmap::fromImage(indexedImage);
    QPixmap originalData = QPixmap::fromImage(CreateImage(0));

    QLabel *originalDisplay = new QLabel(m_UI->m_centralContainer);
    originalDisplay->setPixmap(originalData);
    originalDisplay->setGeometry(QRect(QPoint(50, 300), QSize(m_width, m_height)));
    originalDisplay->show();
    m_UI->m_currentWidgets.push_back(originalDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(originalDisplay->geometry(), "Current Image:", m_UI->m_centralContainer));

    QLabel *indexedDisplay = new QLabel(m_UI->m_centralContainer);
    indexedDisplay->setPixmap(indexedData);
    indexedDisplay->setGeometry(QRect(QPoint(100 + m_width, 300), QSize(m_width, m_height)));
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
    m_Debug->MessageError("This is currently not supported.");
    /*if(!cancelled){
        hasPalette = true;
        paletteCount = 1;
        m_format = ITFProperties_8bpp;
        for(int i = 0; i < m_mipMaps.size(); i++){
            mipMaps[i] = mipMaps[i].convertToFormat(QImage::Format_Indexed8);
        }
    }*/
    updateCenter();
}

void ITF::promptIndexToColor(){
    m_UI->ClearWindow();
    QImage colorImage = CreateImage(0).convertToFormat(QImage::Format_ARGB32); //going straight to 32
    QPixmap colorData = QPixmap::fromImage(colorImage);
    QPixmap originalData = QPixmap::fromImage(CreateImage(0));

    QLabel *originalDisplay = new QLabel(m_UI->m_centralContainer);
    originalDisplay->setPixmap(originalData);
    originalDisplay->setGeometry(QRect(QPoint(50, 300), QSize(m_width, m_height)));
    originalDisplay->show();
    m_UI->m_currentWidgets.push_back(originalDisplay);
    m_UI->m_currentWidgets.push_back(CustomLabel::addLabel(originalDisplay->geometry(), "Current Image:", m_UI->m_centralContainer));

    QLabel *colorDisplay = new QLabel(m_UI->m_centralContainer);
    colorDisplay->setPixmap(colorData);
    colorDisplay->setGeometry(QRect(QPoint(100 + m_width, 300), QSize(m_width, m_height)));
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

QImage ITF::CreateImage(int mipMapLevel){
    int imageWidth = MapWidth(mipMapLevel);
    int imageHeight = MapHeight(mipMapLevel);
    QImage createdImage;
    qDebug() << Q_FUNC_INFO << "creating image that is" << imageWidth << "x" << imageHeight << "from level" << mipMapLevel;
    switch(m_format){
    case ITFProperties_4bpp:{
        qDebug() << Q_FUNC_INFO << "image contains" << m_mipMaps[mipMapLevel].m_indexedPixels.size() << "pixels" ;
        createdImage = QImage(imageWidth, imageHeight, QImage::Format_Indexed8);
        createdImage.setColorCount(16);
        for(int i = 0; i < m_palettes[currentPalette].m_colors.size(); i++){
            createdImage.setColor(i, m_palettes[currentPalette].m_colors[i].rgba());
        }
        for(int i = 0; i < m_mipMaps[mipMapLevel].m_indexedPixels.size(); i++){
            createdImage.setPixel(i % imageWidth, i / imageWidth, m_mipMaps[mipMapLevel].m_indexedPixels[i]);
        }
        break;
    }
    case ITFProperties_8bpp:{
        createdImage = QImage(imageWidth, imageHeight, QImage::Format_Indexed8);
        createdImage.setColorCount(256);
        for(int i = 0; i < m_palettes[currentPalette].m_colors.size(); i++){
            createdImage.setColor(i, m_palettes[currentPalette].m_colors[i].rgba());
        }
        for(int i = 0; i < m_mipMaps[mipMapLevel].m_indexedPixels.size(); i++){
            createdImage.setPixel(i % imageWidth, i / imageWidth, m_mipMaps[mipMapLevel].m_indexedPixels[i]);
        }
        break;
    }
    case ITFProperties_16bpp:{
        createdImage = QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
        for(int i = 0; i < m_mipMaps[mipMapLevel].m_pixels.size(); i++){
            createdImage.setPixel(i % imageWidth, i / imageWidth, m_mipMaps[mipMapLevel].m_pixels[i].rgba());
        }
        break;
    }
    case ITFProperties_24bpp:{
        createdImage = QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
        for(int i = 0; i < m_mipMaps[mipMapLevel].m_pixels.size(); i++){
            createdImage.setPixel(i % imageWidth, i / imageWidth, m_mipMaps[mipMapLevel].m_pixels[i].rgba());
        }
        break;
    }
    case ITFProperties_32bpp:{
        createdImage = QImage(imageWidth, imageHeight, QImage::Format_ARGB32);
        for(int i = 0; i < m_mipMaps[mipMapLevel].m_pixels.size(); i++){
            createdImage.setPixel(i % imageWidth, i / imageWidth, m_mipMaps[mipMapLevel].m_pixels[i].rgba());
        }
        break;
    }
    default:
        qDebug() << Q_FUNC_INFO << "Undefined image format. How did this happen?";
        break;
    }
    return createdImage;
}

void ITF::convertIndexToColor(bool cancelled){
    m_UI->ClearWindow();
    if(!cancelled){
        hasPalette = false;
        for(int i = 0; i < m_mipMaps.size(); i++){
            for(int j = 0; j < m_mipMaps[i].m_indexedPixels.size(); j++){
                m_mipMaps[i].m_pixels.push_back(m_palettes[currentPalette].m_colors[m_mipMaps[i].m_indexedPixels[j]]);
            }
            m_mipMaps[i].m_indexedPixels.clear();
        }
    }
    updateCenter();
}

void ITF::selectMipMap(int mipmapCount){
    currentMipMaps = mipmapCount;
    qDebug() << Q_FUNC_INFO << "mipmap count changed to:" << currentMipMaps;
}

void ITF::selectPalette(int palette){
    currentPalette = palette;
    qDebug() << Q_FUNC_INFO << "new palette selected. Index:" << palette << "setting current palette to" << currentPalette;

    updatePreview();

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

    int colorsPerPalette = pow(2, bytesPerPixel());
    int currentColors = colorsPerPalette * paletteCount;
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

    QList<QRgb> tempColorTable = importData.colorTable();
    exPaletteData addedPalette;
    for(int i = 0; i < addedColors; i++){
        addedPalette.m_colors.push_back(tempColorTable[i]);
    }
    if(!hasPalette){
        hasPalette = true;
    }
    paletteCount += 1;
    m_palettes.push_back(addedPalette);
    comboPalettes->addItem(QString::number(paletteCount));
    comboPalettes->setCurrentIndex(paletteCount-1);

    return 0;
}

int ITF::bytesPerPixel(){
    //int checkProperties = 0;
    /*if(swizzled){
        checkProperties = propertyByte - 128;
    } else {
        checkProperties = propertyByte;
    }*/
    //checkProperties = m_format ^ 0x10000000;
    //qDebug() << Q_FUNC_INFO << "values:" << propertyByte << checkProperties << (checkProperties & 12) << (checkProperties & 11) << (checkProperties & 10) << (checkProperties & 7) << (checkProperties & 2);

    if (m_format == ITFProperties_24bpp){
        qDebug() << Q_FUNC_INFO << "24 bpp";
        return 24;
    } else if (m_format == ITFProperties_8bpp){
        qDebug() << Q_FUNC_INFO << "8 bpp";
        return 8;
    } else if (m_format == ITFProperties_4bpp){
        qDebug() << Q_FUNC_INFO << "4 bpp";
        return 4;
    } else if (m_format == ITFProperties_16bpp){
        qDebug() << Q_FUNC_INFO << "16 bpp";
        return 16;
    } else if(m_format == ITFProperties_32bpp){
        qDebug() << Q_FUNC_INFO << "32 bpp";
        return 32;
    }

    return 0;
}

void ITF::createMipMaps(int mipmapLevels){
//    qDebug() << Q_FUNC_INFO << "width: " << mipMaps[0].width() << "is the" << log(mipMaps[0].width())/log(2) <<"th power of 2";
//    qDebug() << Q_FUNC_INFO << "maximum number of mipmaps is" << int(fmin(log(mipMaps[0].width())/log(2), log(mipMaps[0].height())/log(2)));
    qDebug() << Q_FUNC_INFO << "requested maps:" << mipmapLevels;
    int maxLevels = int(fmin(log(m_width)/log(2), log(m_height)/log(2)));
    if(mipmapLevels > maxLevels){
        m_Debug->MessageError("Too many mipmaps. Maximum maps for this texture: " + QString::number(maxLevels) + ". Entered maps: " + QString::number(mipmapLevels));
    }
    exImageData storeImage = m_mipMaps[0];

    m_mipMaps.clear(); //if the image already has maps, they need to be cleaned out to make room for the new ones.
    m_mipMaps.push_back(storeImage); //this feels gross though, find a better way to clear the extras.
    if(swizzled){
        m_Debug->MessageError("Texture is currently swizzled. Unswizzling before generating new mipmaps.");
        //unswizzle();
        swizzle(true);
    }
    if(mipmapLevels == 1){
        qDebug() << Q_FUNC_INFO << "Only one mipmap level selected - this is the base texture. Existing maps are being cleared, new maps will not be generated.";
        hasMipmaps = false;
        mipmapCount = mipmapLevels;
        updateCenter();
        return;
    }
    for(int i = 1; i < mipmapLevels; i++){
        m_mipMaps.push_back(ScaleBase(i));
    }

    hasMipmaps = true;
    mipmapCount = mipmapLevels;

    updateCenter();
}

exImageData ITF::ScaleBase(int mipMapLevel){
    /*Scales the image to the given level. Mipmaps must be in powers of 2, so we can use that to our advantage
     For any given level, we know we will have an image 2^x times smaller. So, we can treat each pixel as a square
    that is 2^x times larger than it is, take the average, and use the result as the new pixel. Then we skip over
    all other pixels in that square (because they've already been accounted for).
    This might also just make the image grey, but we'll see.*/

    exImageData baseImage = m_mipMaps[0];
    if(mipMapLevel == 0){
        return baseImage;
    }
    exImageData scaledImage;
    int column = 0;
    int row = 0;
    int scaleFactor = int(pow(2, mipMapLevel));
    int currentPixel = 0;
    //qDebug() << Q_FUNC_INFO << "scaling image from" << m_width << "x" << m_height << "to" << m_width/scaleFactor << "x" << m_height/scaleFactor;
    if(m_format == ITFProperties_4bpp || m_format == ITFProperties_8bpp){
        /*not sure if we can really do this for indexed images? I'm sure there's a way
        for now, just take the most common index among the available options
        I think whatever function is made to convert non-indexed images into indexed ones may help here?
        whenever that gets written, at least
        */
        for(int i = 0; i < baseImage.m_indexedPixels.size(); i++){
            std::vector<int> indexList;
            column = i / m_width;
            row = i % m_width;
            if(row % scaleFactor != 0 || column % scaleFactor != 0){
                continue;
            }
            //qDebug() << Q_FUNC_INFO << "Checking scaling at pixel" << i;
            for(int x = 0; x < scaleFactor; x++){
                for(int y = 0; y < scaleFactor; y++){
                    currentPixel = i + x + y*m_width;
                    indexList.push_back(baseImage.m_indexedPixels[currentPixel]);
                }
            }
            //qDebug() << Q_FUNC_INFO << "index list has" << indexList.size() << "items. should contain" << pow(2, mipMapLevel+1);
            int mode = 0;
            int modeCount = 0;
            int currentCount = 0;
            for(int j = 0; j < indexList.size(); j++){
                for(int k = 0; k < indexList.size(); k++){
                    if(indexList[j] == indexList[k]){
                        currentCount++;
                    }
                }
                if(currentCount > modeCount){
                    mode = indexList[j];
                    modeCount = currentCount;
                }
                currentCount = 0;
            }
            //qDebug() << Q_FUNC_INFO << "mode for pixel was" << mode;
            scaledImage.m_indexedPixels.push_back(mode);
        }
        //qDebug() << Q_FUNC_INFO << "scaled image has" << scaledImage.m_indexedPixels.size() << "pixels. Should contain" << m_mipMaps[0].m_indexedPixels.size()/pow(2, mipMapLevel+1);
    } else {
        for(int i = 0; i < baseImage.m_pixels.size(); i++){
            std::vector<int> redList;
            std::vector<int> greenList;
            std::vector<int> blueList;
            std::vector<int> alphaList;
            column = i / m_width;
            row = i % m_width;
            if(row % scaleFactor != 0 || column % scaleFactor != 0){
                continue;
            }
            for(int x = 0; x < scaleFactor; x++){
                for(int y = 0; y < scaleFactor; y++){
                    currentPixel = i + x + y*m_width;
                    redList.push_back(baseImage.m_pixels[currentPixel].red());
                    greenList.push_back(baseImage.m_pixels[currentPixel].green());
                    blueList.push_back(baseImage.m_pixels[currentPixel].blue());
                    alphaList.push_back(baseImage.m_pixels[currentPixel].alpha());
                }
            }
            int avgRed = int(std::accumulate(redList.begin(), redList.end(), 0.0) / redList.size());
            int avgGreen = int(std::accumulate(greenList.begin(), greenList.end(), 0.0) / greenList.size());
            int avgBlue = int(std::accumulate(blueList.begin(), blueList.end(), 0.0) / blueList.size());
            int avgAlpha = int(std::accumulate(alphaList.begin(), alphaList.end(), 0.0) / alphaList.size());
            QColor avgColor = QColor(avgRed, avgGreen, avgBlue, avgAlpha);
            scaledImage.m_pixels.push_back(avgColor);
        }
    }
    return scaledImage;
}

void ITF::readPalette(){
    int colorsPerPalette = std::pow(2, bytesPerPixel());
    qDebug() << Q_FUNC_INFO << "Color count: " << colorsPerPalette << "palette count:" << paletteCount;
    //mipMaps[0].setColorCount(colorsPerPalette*paletteCount);
    for(int j = 0; j < paletteCount; j++){
        exPaletteData addedPalette;
        qDebug() << Q_FUNC_INFO << "color count before:" << addedPalette.m_colors.size() << ". should be set to" << colorsPerPalette*paletteCount;
        for(int i = 0; i < colorsPerPalette; i++){
            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;
            uint8_t alpha = 0;
            fileData->process(red);
            fileData->process(green);
            fileData->process(blue);
            fileData->process(alpha);
            if(alphaType == Alpha_Opaque){
                //opaque texture, no alpha
                alpha = 255;
            } else if (alphaType == Alpha_Punchthrough){
                //punchthrough texture, either full alpha or no alpha
                if (alpha < 128){
                    alpha = 0;
                } else {
                    alpha = 255;
                }
            }

            addedPalette.m_colors.push_back(QColor(red, green, blue, alpha));
            //forceTable.push_back(nextColor);
        }
        m_palettes.push_back(addedPalette);
    }

    changeColorTable(true);

}

void ITF::readIndexedData(){
    std::tuple <exUInt8, exUInt8> nibTup;
    for(int m = 0; m < mipmapCount; m++){
        int imageWidth = MapWidth(m);
        int imageHeight = MapHeight(m);
        exImageData addedMap;
        if (m_format == ITFProperties_8bpp){
            //8bpp, 256 palette case. nice and easy since each pixel uses 1 byte to refer to the palette
            for (int i = 0; i < imageWidth * imageHeight; i++){
                //qDebug() << Q_FUNC_INFO << "current pixel " << i << "x" << i%width << "y" << i/width;
                exUInt8 pixelIndex;
                fileData->process(pixelIndex);
                addedMap.m_indexedPixels.resize(i+1);
                addedMap.SetPixel(i, pixelIndex);
            }

        } else {
            //4bpp case, 16 color palette. this is tougher since each pixel is only half a byte (nibble?) and we can only refer to whole bytes.
            //however every image should be an even number of pixels so we can just grab them in pairs.
            int pixelIndex = 0;
            addedMap.m_indexedPixels.resize(imageWidth * imageHeight);
            for (int i = 0; i < (imageWidth * imageHeight)/2; i++){
                //qDebug() << Q_FUNC_INFO << "current pixel pair" << i << "x" << pixelIndex%width << "y" << pixelIndex/width;
                fileData->process(nibTup);
                addedMap.SetPixel(pixelIndex, std::get<0>(nibTup));
                pixelIndex += 1;
                addedMap.SetPixel(pixelIndex, std::get<1>(nibTup));
                pixelIndex += 1;
            }
        }
        m_mipMaps.push_back(addedMap);
    }

}

void ITF::readImageData(){
    //these all store their color values directly instead of referring to a palette
    //int contentLength = 0;
    exUInt16 combinedIntensity = 0;
    int currentWidth = 0;
    int currentHeight = 0;

    for(int m = 0; m < mipmapCount; m++){
        exImageData addedMap;
        currentHeight = m_height/pow(2,m);
        currentWidth = m_width/pow(2,m);
        switch(m_format){
        case ITFProperties_16bpp:
        qDebug() << Q_FUNC_INFO << "16bpp";
        //16bpp, each pixel has its r,g,b, and a values stored as 4 integers packed into 2 bytes
        //contentLength = dataLength / 2;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            fileData->process(combinedIntensity);
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
            addedMap.m_pixels.push_back(currentPixel);
        }
        break;

        case ITFProperties_24bpp:
        qDebug() << Q_FUNC_INFO << "24bpp";
        //24bpp, each pixel has its r,g, and b values stored as single-byte integers
        //contentLength = dataLength / 3;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            QColor currentPixel;
            fileData->process(currentPixel, ColorType_RGB_Char); //don't have to worry about alpha type with this - alpha is always 0*/
            //qDebug() << Q_FUNC_INFO << "setting color at x" << i % currentWidth << "y" << i / currentWidth << "to" << currentPixel;
            addedMap.m_pixels.push_back(currentPixel);
        }
        break;

        case ITFProperties_32bpp:
        qDebug() << Q_FUNC_INFO << "32bpp";
        //32bpp, each pixel has its r,g,b, and a values stored as single-byte integers
        //contentLength = dataLength / 4;
        for (int i = 0; i < currentWidth * currentHeight; i++){
            QColor currentPixel;
            fileData->process(currentPixel, ColorType_RGBA_Char);
            if(alphaType == 1){
                //opaque texture, no alpha
                currentPixel.setAlpha(255);
            } else if (alphaType == 2){
                //punchthrough texture, either full alpha or no alpha
                if (currentPixel.alpha() < 128){
                    currentPixel.setAlpha(0);
                } else {
                    currentPixel.setAlpha(255);
                }
            }
            //currentPixel.setAlpha(alpha);
            //qDebug() << Q_FUNC_INFO << "setting color at x" << i % currentWidth << "y" << i / currentWidth << "to" << currentPixel;
            addedMap.m_pixels.push_back(currentPixel);
        }
        break;

        default:
        m_Debug->MessageError("Unknown bit depth");
        }
        m_mipMaps.push_back(addedMap);
    }
}

void ITF::AdaptImage(QImage input){
    versionNum = 3; //most recent known version - we can experiment to see if the game recognizes a version 4 later
    headerLength = 32;
    m_format = 0; //this will be the hard part
    alphaType = 0; //blended alpha by default
    m_width = input.width();
    m_height = input.height();
    mipmapCount = 1; //base image
    unknown4Byte3 = 0; //this value is 0 in all recorded files. Might be reseved values for later file versions?
    unknown4Byte4 = 0;
    swizzled = false; //assuming imported images are not swizzled

    qDebug() << Q_FUNC_INFO << "Image format:" << input.format();

    if(input.format() == QImage::Format_Indexed8){
        if(input.colorCount() < 15){
            m_format = ITFProperties_4bpp;
            m_Debug->Log("Image bit depth: 4bpp");
        } else {
            m_format = ITFProperties_8bpp;
            m_Debug->Log("Image bit depth: 8bpp");
        }
        paletteCount = 1; //additional palettes can be imported
        hasPalette = true;

    } else if (input.format() == QImage::Format_ARGB32 || input.format() == QImage::Format_RGB32){
        //there are probably other formats that need to be included here.
        m_format = ITFProperties_32bpp;
        m_Debug->Log("Image bit depth: 32bpp");
        hasPalette = false;
        //need to find a way later to compress these down
    } else {
        qDebug() << Q_FUNC_INFO << "Image format:" << input.format();
        m_Debug->Log(&"Image format read as: " [ input.format()]);
        m_Debug->MessageError("Invalid image format.");
    }

    if(int(log(m_width)/log(2)) != log(m_width)/log(2)){
        m_Debug->MessageError("Image width is not a factor of 2");
    }

    if(int(log(m_height)/log(2)) != log(m_height)/log(2)){
        m_Debug->MessageError("Image height is not a factor of 2");
    }

}

int ITF::readDataITF(){

    QTableWidgetItem currentItem;
    currentPalette = 0;
    fileData->currentPosition += 4; //skip "FORM"
    qDebug() << Q_FUNC_INFO << "reading file" << fileName;
    fileData->process(fileLength);
    qDebug() << Q_FUNC_INFO << "file length:" << fileLength;

    /*Load header data*/
    fileData->currentPosition = 15;
    fileData->process(versionNum);
    fileData->process(headerLength);
    fileData->currentPosition += 3; //skip the "PS2" label
    exUInt8 propertyByte = 0;
    fileData->process(propertyByte);
    m_format = propertyByte & (ITFProperties_Swizzled ^ 0xFF); //this feels a bit hacky but it's just a bitmask? might be overthinking this one
    swizzled = propertyByte & ITFProperties_Swizzled;
    fileData->process(alphaType);
    fileData->process(m_width);
    fileData->process(m_height);
    fileData->process(mipmapCount);
    mipmapCount = std::max<exUInt32>(1, mipmapCount);
    fileData->process(paletteCount);
    paletteCount = std::max<exUInt32>(1, paletteCount); //some textures say 0 palettes, this catches those. possibly older ITF file version?
    fileData->process(unknown4Byte3);
    fileData->process(unknown4Byte4);
    /*End header data.*/
    qDebug() << Q_FUNC_INFO << "version:" << versionNum;
    qDebug() << Q_FUNC_INFO << "header length:" << headerLength;
    qDebug() << Q_FUNC_INFO << "property byte:" << propertyByte;
    qDebug() << Q_FUNC_INFO << "property byte:" << propertyByte << "format value:" << m_format << "swizzled?" << swizzled;
    qDebug() << Q_FUNC_INFO << "Alpha type:" << alphaTypes[alphaType];
    qDebug() << Q_FUNC_INFO << "width:" << m_width << "image height:" << m_height;
    qDebug() << Q_FUNC_INFO << "mipmap count:" << mipmapCount;
    qDebug() << Q_FUNC_INFO << "palette count:" << paletteCount;
    qDebug() << Q_FUNC_INFO << "position after header:" << fileData->currentPosition;


    if(paletteCount > 16){
        //this catch is for the Sarge textures, which claim to have 23 palettes (they don't).
        paletteCount = 1;
    }

    if(m_format == ITFProperties_4bpp || m_format == ITFProperties_8bpp){
        hasPalette = true;
    } else {
        hasPalette = false;
        paletteCount = 0;
    }

    if(mipmapCount > 0){
        hasMipmaps = true;
    } else {
        hasMipmaps = false;
    }

    qDebug() << Q_FUNC_INFO << "Possition after header data:" << fileData->currentPosition;

    //Why were we searching for TXTR instead of just reading through the file? what?
    //fileData->currentPosition = matcher.indexIn(fileData->dataBytes, 0)+4; //skip "TXTR"
    fileData->currentPosition += 4; //skip "TXTR"


    fileData->process(m_dataLength);
    //qDebug() << Q_FUNC_INFO << "bpp" << bytesPerPixel() << "has palette" << hasPalette;

    if(hasPalette){
        readPalette();
        readIndexedData();
    } else {
        readImageData();
    }

    if(swizzled){
        swizzle(true);
    }

    //createMipMaps(mipmapCount);

    return 0;
}

int ITF::MapHeight(int level){
    return m_height / pow(2, level);
}

int ITF::MapWidth(int level){
    return m_width / pow(2, level);
}

void ITF::populatePalette(){
    qDebug() << Q_FUNC_INFO << "Function called. Palette index: " << currentPalette+1 << "out of" << paletteCount;
    QStringList columnNames = {"Palette Index", "Red", "Green", "Blue", "Alpha", "Original", "Current"};
    paletteTable->setHorizontalHeaderLabels(columnNames);
    paletteTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    //QList<QRgb> tempColorTable = mipMaps[0].colorTable();
    for(int i = 0; i < m_palettes[currentPalette].m_colors.size(); i++){
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
        cellText->setText(QString::number(m_palettes[currentPalette].m_colors[i].red()));
        QTableWidgetItem *cellText2 = paletteTable->item(i,2);
        if (!cellText2){
            cellText2 = new QTableWidgetItem;
            paletteTable->setItem(i,2,cellText2);
        }
        cellText2->setText(QString::number(m_palettes[currentPalette].m_colors[i].green()));
        QTableWidgetItem *cellText3 = paletteTable->item(i,3);
        if (!cellText3){
            cellText3 = new QTableWidgetItem;
            paletteTable->setItem(i,3,cellText3);
        }
        cellText3->setText(QString::number(m_palettes[currentPalette].m_colors[i].blue()));
        QTableWidgetItem *cellText4 = paletteTable->item(i,4);
        if (!cellText4){
            cellText4 = new QTableWidgetItem;
            paletteTable->setItem(i,4,cellText4);
        }
        cellText4->setText(QString::number(m_palettes[currentPalette].m_colors[i].alpha()));
        QTableWidgetItem *cellText5 = paletteTable->item(i,5);
        if (!cellText5){
            cellText5 = new QTableWidgetItem;
            paletteTable->setItem(i,5,cellText5);
        }
        cellText5->setBackground(m_palettes[currentPalette].m_colors[i]);
        QTableWidgetItem *cellText6 = paletteTable->item(i,6);
        if (!cellText6){
            cellText6 = new QTableWidgetItem;
            paletteTable->setItem(i,6,cellText6);
        }
        cellText6->setBackground(m_palettes[currentPalette].m_colors[i]);
        paletteTable->blockSignals(0);
    }
}

void ITF::editPalette(int row, int column){
    int changedValue = paletteTable->item(row, column)->text().toInt(nullptr, 10);
    int colorCount = 0;
    int paletteOffset = 0;
    if(m_format == ITFProperties_4bpp){
        colorCount = 16;
    } else if(m_format == ITFProperties_4bpp){
        colorCount = 256;
    }
    paletteOffset = colorCount/paletteCount * currentPalette;
    int colorIndex = paletteOffset + row;
    qDebug() << Q_FUNC_INFO << "Changed value: " << paletteTable->item(row, column)->text();
    qDebug() << Q_FUNC_INFO << "Row: " << row << " Column " << column;
    //QList<QRgb> tempColorTable = mipMaps[0].colorTable();
    QColor currentColor = m_palettes[currentPalette].m_colors[colorIndex];
    if (changedValue < 256 and changedValue >= 0 ){
        qDebug() << Q_FUNC_INFO << "Valid color value";
        switch (column){
        case 1: m_mipMaps[0].SetPixel(colorIndex, qRgba(changedValue, currentColor.green(), currentColor.blue(), currentColor.alpha())) ; break;
        case 2: m_mipMaps[0].SetPixel(colorIndex, qRgba(currentColor.red(), changedValue, currentColor.blue(), currentColor.alpha())) ; break;
        case 3: m_mipMaps[0].SetPixel(colorIndex, qRgba(currentColor.red(), currentColor.green(), changedValue, currentColor.alpha())) ; break;
        case 4: m_mipMaps[0].SetPixel(colorIndex, qRgba(currentColor.red(), currentColor.green(), currentColor.blue(), changedValue)) ; break;
        }
        QTableWidgetItem *cellText5 = paletteTable->item(row, 6);
        cellText5->setBackground(m_palettes[currentPalette].m_colors[colorIndex]);
        qDebug() << Q_FUNC_INFO << "cell text" << cellText5->text();

        selectPalette(currentPalette);

    } else {
        qDebug() << Q_FUNC_INFO << "Not a valid color value.";
        switch (column){
        case 1: paletteTable->item(row,column)->text() = QString::number(m_palettes[currentPalette].m_colors[colorIndex].red()); break;
        case 2: paletteTable->item(row,column)->text() = QString::number(m_palettes[currentPalette].m_colors[colorIndex].green()); break;
        case 3: paletteTable->item(row,column)->text() = QString::number(m_palettes[currentPalette].m_colors[colorIndex].blue()); break;
        case 4: paletteTable->item(row,column)->text() = QString::number(m_palettes[currentPalette].m_colors[colorIndex].alpha()); break;
        }
    }

}

int ITF::dataLength(){
    int dataLength = 0;
    dataLength = m_height * m_width * (bytesPerPixel()/8);
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
    uint8_t propertyByte = m_format + (swizzled<<7);

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
        BinChanger::intWrite(itfOut, m_width);
        BinChanger::intWrite(itfOut, m_height);
        BinChanger::intWrite(itfOut, mipmapCount);
        BinChanger::intWrite(itfOut, paletteCount);
        BinChanger::intWrite(itfOut, unknown4Byte3);
        BinChanger::intWrite(itfOut, unknown4Byte4);
        itfOut.write("TXTR");
        BinChanger::intWrite(itfOut, dataLength());
        //QList<QRgb> tempColorTable = mipMaps[0].colorTable();
        if(hasPalette){
            for(int i = 0; i < m_mipMaps[0].m_pixels.size(); i++){
                BinChanger::byteWrite(itfOut, m_mipMaps[0].m_pixels[i].red());
                BinChanger::byteWrite(itfOut, m_mipMaps[0].m_pixels[i].green());
                BinChanger::byteWrite(itfOut, m_mipMaps[0].m_pixels[i].blue());
                BinChanger::byteWrite(itfOut, m_mipMaps[0].m_pixels[i].alpha());
            }
            for(int i = 0; i < m_mipMaps.size(); i++){
                writeIndexedData(itfOut, &m_mipMaps[i]);
            }
        } else {
            for(int i = 0; i < m_mipMaps.size(); i++){
                writeImageData(itfOut, &m_mipMaps[i]);
            }
        }

        //and that should be it

    }
    changeColorTable(true);

}

void ITF::writeIndexedData(QFile& fileOut, exImageData *writeData){
    std::tuple <int8_t, int8_t> nibTup;

    switch(m_format){
        case ITFProperties_8bpp:
            m_Debug->Log("Exporting to ITF with bit depth: 8bpp");
            for(int i = 0; i < writeData->m_indexedPixels.size(); i++){
                BinChanger::byteWrite(fileOut, writeData->m_indexedPixels[i]);
            }
            break;

        case ITFProperties_4bpp:
        m_Debug->Log("Exporting to ITF with bit depth: 4bpp");
        for(int i = 0; i < m_height*m_width; i+=2){
            std::get<0>(nibTup) = writeData->m_indexedPixels[i];
            std::get<1>(nibTup) = writeData->m_indexedPixels[i+1];
            BinChanger::byteWrite(fileOut, BinChanger::nib_to_byte(nibTup));
        }
        break;

        default:
        m_Debug->MessageError("Unknown byte depth");
    }
}

void ITF::writeImageData(QFile& fileOut, exImageData *writeData){
    int alpha = 0;
    int combinedIntensities = 0;
    qDebug() << Q_FUNC_INFO << "width:" << m_width << "height" << m_height << "Total pixels:" << m_width * m_height;
    m_Debug->Log("Exporting to ITF with bit depth: " + QString::number(bytesPerPixel()) + "bpp");
    for(int i = 0; i < writeData->m_pixels.size(); i++){
        QColor currentPixel = writeData->m_pixels[i];
        switch(m_format){
        case ITFProperties_16bpp:
            //QColor currentPixel = qRgba(((combinedIntensity >> 0) & 31)*8, ((combinedIntensity >> 5) & 31)*8, ((combinedIntensity >> 10) & 31)*8, alpha);
            alpha = std::min(currentPixel.alpha(), 1);
            combinedIntensities = (currentPixel.red()/8) + ((currentPixel.green()/8) << 5) + ((currentPixel.blue()/8) << 10) + (alpha<<15);
            BinChanger::shortWrite(fileOut, combinedIntensities);
            break;

        case ITFProperties_24bpp:
            BinChanger::byteWrite(fileOut, currentPixel.blue());
            BinChanger::byteWrite(fileOut, currentPixel.green());
            BinChanger::byteWrite(fileOut, currentPixel.red());
            break;

        case ITFProperties_32bpp:
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

void ITF::swizzle(bool unswizzle){
    std::vector<QColor> swizzledImage;
    std::vector<int> swizzledIndex;
    //https://gist.github.com/Fireboyd78/1546f5c86ebce52ce05e7837c697dc72

    //qDebug() << Q_FUNC_INFO << "unswizzled image size" << pixelList.size();
    int InterlaceMatrix[] = {
        0x00, 0x10, 0x02, 0x12,
        0x11, 0x01, 0x13, 0x03,
    };

    int Matrix[]        = { 0, 1, -1, 0 };
    int TileMatrix[]    = { 4, -4 };


    int mapWidth = 0;
    //to-do: this code seems to work, but the variables should be renamed to actually be useful.
    for(int m = 0; m < mipmapCount; m++){
        //swizzledImage = QImage(mipMaps[m]);
        mapWidth = MapWidth(m);
        swizzledImage = m_mipMaps[m].m_pixels;
        swizzledIndex = m_mipMaps[m].m_indexedPixels;
        for (int y = 0; y < MapHeight(m); y++)
        {
            for (int x = 0; x < mapWidth; x++)
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

                int num7 = (oddRow) ? ((y - 1) * mapWidth) : (y * mapWidth);

                int xx = x + num1 * TileMatrix[num2];
                int yy = y + Matrix[num3];

                int i = 0;
                int j = 0;

                if(unswizzle){
                    j = InterlaceMatrix[num4] + num5 + num6 + num7;
                    i = yy * mapWidth + xx;
                } else {
                    i = InterlaceMatrix[num4] + num5 + num6 + num7;
                    j = yy * mapWidth + xx;
                }

                //qDebug() << Q_FUNC_INFO << "x" << x << "y" << y << "i" << i << "j" << j;

                if(hasPalette){
                    swizzledIndex[i] = m_mipMaps[m].m_indexedPixels[j];
                } else {
                    swizzledImage[i] = m_mipMaps[m].m_pixels[j];
                }
                /*if(hasPalette){
                    swizzledImage.setPixel(i % swizzledImage.width(), i/swizzledImage.width(), mipMaps[m].pixelIndex(j % mipMaps[m].width(), j/mipMaps[m].width()));
                } else {
                    swizzledImage.setPixel(i % swizzledImage.width(), i/swizzledImage.width(), mipMaps[m].pixel(j % mipMaps[m].width(), j/mipMaps[m].width()));
                }*/

            }
        }
        //mipMaps[m] = swizzledImage;
        if(hasPalette){
            m_mipMaps[m].m_indexedPixels = swizzledIndex;
        } else {
            m_mipMaps[m].m_pixels = swizzledImage;
        }
    }

    swizzled = !unswizzle;

}
