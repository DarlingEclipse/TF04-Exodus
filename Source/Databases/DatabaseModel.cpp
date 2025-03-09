#include <QDateTime>

#include "Headers/Main/exDebugger.h"
#include "Headers/Models/vbin.h"
#include "Headers/Databases/Database.h"
#include "Headers/FileManagement/Zebrafish.h"

/*This file handles the exporting of Database files to DAE format*/

void DictionaryFile::writeDAE(){
    qDebug() << Q_FUNC_INFO << "Can't export a definition file to DAE";
}

void DatabaseFile::writeDAE(){
    /*for each instance:
     *  check the instance's inherited file's dictionary base for the following named attributes
     *      position - placement within the level
     *      orientation - orientation
     *      prototype - name of the VBIN file needed
     *  if this value exists in both the instance and dictionary, use the instance value
     *  else, use dictionary default
     *
     *then write the positions to a DAE file
     *there's most likely a way to include a single instance of a VBIN's data as one model within the DAE
     *then reference it multiple times throughout the file
     *this will be useful here as well as for Instances - interesting - in the VBIN files
    */
    QStringList uniquePrototypes;
    //need to use instances here for minicons and datacons
    for(int i = 0; i < instances.size(); i++){
        if(instances[i].prototype != "" && !uniquePrototypes.contains(instances[i].prototype)){
            uniquePrototypes.push_back(instances[i].prototype);
        }
    }

    std::shared_ptr<taFile> testLoaded;
    std::vector<std::shared_ptr<taFile>> loadedModels;
    for(int i = 0; i < uniquePrototypes.size(); i++){
        testLoaded = m_zlManager->matchFile(uniquePrototypes[i] + ".VBIN");
        while(testLoaded == nullptr){
            m_Debug->MessageError("Please load a file " + uniquePrototypes[i]+".VBIN");
            m_zlManager->openFile("VBIN");
            testLoaded = m_zlManager->matchFile(uniquePrototypes[i] + ".VBIN");
        }
        testLoaded->outputPath = outputPath;
        loadedModels.push_back(testLoaded);
    }


    QFile stlOut(outputPath);
    QFile file(outputPath);
    file.open(QFile::WriteOnly|QFile::Truncate);
    file.close();

    if(!stlOut.open(QIODevice::ReadWrite)){
        m_Debug->MessageError("DAE export failed, could not open output file.");
        return;
    }
    QTextStream stream(&stlOut);
    //return;

    stream << "<?xml version=\"1.0\" encoding=\"utf-8\"?>" << Qt::endl;
    stream << "<COLLADA xmlns=\"http://www.collada.org/2005/11/COLLADASchema\" version=\"1.4.1\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">" << Qt::endl;

    stream << "  <asset>" << Qt::endl;
    stream << "    <contributor>" << Qt::endl;
    stream << "      <author>Everett Darling</author>" << Qt::endl;
    stream << "      <authoring_tool>ExodusVBIN v" << VBIN::version << "</authoring_tool>" << Qt::endl;
    stream << "    </contributor>" << Qt::endl;
    stream << "    <created>" << QDateTime::currentDateTime().toString("yyyy-MM-dd") + "T" + QDateTime::currentDateTime().toString("hh:mm:ss") << "</created>" << Qt::endl;
    stream << "    <modified>" << QDateTime::currentDateTime().toString("yyyy-MM-dd") + "T" + QDateTime::currentDateTime().toString("hh:mm:ss") << "</modified>" << Qt::endl;
    stream << "    <unit name=\"meter\" meter=\"1\"/>" << Qt::endl;
    stream << "    <up_axis>Z_UP</up_axis>" << Qt::endl;
    stream << "  </asset>" << Qt::endl;

    stream << "  <library_effects>" << Qt::endl;
    for(int i = 0; i < uniquePrototypes.size(); i++){
        loadedModels[i]->save("Effect", stream);
    }
    stream << "  </library_effects>" << Qt::endl;

    stream << "  <library_images>" << Qt::endl;
    for(int i = 0; i < uniquePrototypes.size(); i++){
        loadedModels[i]->save("Image", stream);
    }
    stream << "  </library_images>" << Qt::endl;

    stream << "  <library_materials>" << Qt::endl;
    for(int i = 0; i < uniquePrototypes.size(); i++){
        loadedModels[i]->save("Material", stream);
    }
    stream << "  </library_materials>" << Qt::endl;

    stream << "  <library_geometries>" << Qt::endl;
    for(int i = 0; i < uniquePrototypes.size(); i++){
        loadedModels[i]->save("Geometry", stream);
    }
    stream << "  </library_geometries>" << Qt::endl;

    stream << "  <library_animations>" << Qt::endl;
//    for(int i = 0; i < loadedModels.size(); i++){
//        loadedModels[i]->save("Animation", stream);
//    }
    stream << "  </library_animations>" << Qt::endl;

    stream << "  <library_nodes>" << Qt::endl;
//    for(int i = 0; i < uniquePrototypes.size(); i++){
//        stream << "      <node id=\"" + uniquePrototypes[i] + "\" name=\"" + uniquePrototypes[i] + "\" type=\"NODE\">" << Qt::endl;
//        loadedModels[i]->save("Node", stream);
//        stream << "      </node>" << Qt::endl;
//    }
    stream << "  </library_nodes>" << Qt::endl;

    QString instanceExportName;
    int prototypeIndex = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float angle = 0;
    stream << "  <library_visual_scenes>" << Qt::endl;
    stream << "    <visual_scene id=\"Scene\" name=\"Scene\">" << Qt::endl;
    for(int i = 0; i < instances.size(); i++){
        if(instances[i].prototype != ""){
            instances[i].orientation.getAxisAndAngle(&x, &y, &z, &angle);
            instanceExportName = instances[i].prototype + "_ID" + QString::number(instances[i].instanceIndex);
            stream << "      <node id=\"" + instanceExportName + "\" name=\"" + instanceExportName + "\" type=\"NODE\">" << Qt::endl;
            stream << "        <translate sid=\"translate\">" + QString::number(instances[i].position.x(), 'g', 3) + " " + QString::number(instances[i].position.y(), 'g', 3)
                       + " " + QString::number(instances[i].position.z(), 'g', 3) + "</translate>" << Qt::endl;

            //stream << "        <rotate sid=\"rotate\">" + QString::number(x, 'g', 3) + " " + QString::number(y, 'g', 3)
            //           + " " + QString::number(z, 'g', 3) + " " + QString::number(angle, 'g', 3) + "</rotate>" << Qt::endl;
            stream << "        <scale sid=\"scale\">1 1 1</scale>" << Qt::endl;
//            stream << "        <node id=\"" + instanceExportName + "_node\" name=\"" + instanceExportName + "_node\">" << Qt::endl;
//            stream << "          <instance_node url=\"#" + instances[i].prototype + "\"/>" << Qt::endl;
//            stream << "        </node>" << Qt::endl;
            prototypeIndex = uniquePrototypes.indexOf(instances[i].prototype);
            stream << "      <node id=\"" + instanceExportName + "_node\" name=\"" + instanceExportName + "_node\" type=\"NODE\">" << Qt::endl;
            loadedModels[prototypeIndex]->save("Node", stream);
            stream << "      </node>" << Qt::endl;
            stream << "      </node>" << Qt::endl;
        }
    }
    stream << "    </visual_scene>" << Qt::endl;
    stream << "  </library_visual_scenes>" << Qt::endl;

    stream << "  <scene>" << Qt::endl;
    stream << "    <instance_visual_scene url=\"#Scene\"/>" << Qt::endl;
    stream << "  </scene>" << Qt::endl;

    stream << "</COLLADA>" << Qt::endl;

}
