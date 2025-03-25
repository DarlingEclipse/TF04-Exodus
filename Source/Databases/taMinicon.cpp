/*Includes functions related to minicons from the taMinicon class and datahandler
taking this as a sign that datahandler is doing too much and that taminicon should be taking some of the load
or just that datahandler should be split up a bit in some way. but the lines are getting fuzzier
and my patience shorter.*/

#include "Headers/Databases/DataHandler.h"
#include "Headers/Databases/taMinicon.h"

taMinicon::taMinicon(){
    this->name = "UNLOADED";
    this->minicon = MiniconNone;
}

void taMinicon::SetValues(dictItem copyItem){
    this->activationType = copyItem.searchAttributes<int>("ActivationType");
    this->chargeDrainRate_Commander = copyItem.searchAttributes<float>("ChargeDrainMultiplier_Commander");
    this->chargeDrainRate_Veteran = copyItem.searchAttributes<float>("ChargeDrainMultiplier_Veteran");
    this->chargeDrainTime = copyItem.searchAttributes<float>("ChargeDrainTime");
    this->coolDownTime = copyItem.searchAttributes<float>("CoolDownTime");
    this->coolDownTimeDepleted = copyItem.searchAttributes<float>("CoolDownTimeDepleted");
    this->crosshairIndex = copyItem.searchAttributes<int>("CrosshairIndex");
    this->equipInHQ = copyItem.searchAttributes<bool>("EquipInHQ");
    this->icon = copyItem.searchAttributes<int>("Icon");
    this->minicon = copyItem.searchAttributes<int>("Minicon");
    this->minimumChargeToUse = copyItem.searchAttributes<float>("MinimumChargeToUse");
    this->minimumChargeToUsePerShot = copyItem.searchAttributes<float>("MinimumChargeToUsePerShot");
    this->name = copyItem.searchAttributes<QString>("Name");
    this->nodeToKeepIndex = copyItem.searchAttributes<int>("NodeToKeepIndex");
    this->paletteIndex = copyItem.searchAttributes<int>("PaletteIndex");
    this->powerCost = copyItem.searchAttributes<int>("PowerCost");
    this->rechargeTime = copyItem.searchAttributes<float>("RechargeTime");
    this->recoilType = copyItem.searchAttributes<int>("RecoilType");
    this->restrictToButton = copyItem.searchAttributes<int>("RestrictToButton");
    this->segments = copyItem.searchAttributes<int>("Segments");
    this->sidekickCoolDownTime = copyItem.searchAttributes<float>("SidekickCoolDownTime");
    this->sidekickRechargeTime = copyItem.searchAttributes<float>("SidekickRechargeTime");
    this->sidekickSegments = copyItem.searchAttributes<int>("SidekickSegments");
    this->slot = copyItem.searchAttributes<int>("Slot");
    this->team = copyItem.searchAttributes<int>("Team");
    this->toneLibrary = copyItem.searchAttributes<QString>("ToneLibrary");
}

taMinicon::taMinicon(dictItem copyItem){
    SetValues(copyItem);
}

/*taMiniconArmor::taMiniconArmor(dictItem copyItem){
    SetValues(copyItem);
    this->AbsorbPercentage = copyItem.searchAttributes<float>("AbsorbPercentage");
    this->MaxAbsorbPerHit = copyItem.searchAttributes<float>("MaxAbsorbPerHit");
    this->MaxDamageToAbsorb = copyItem.searchAttributes<float>("MaxDamageToAbsorb");
}

taMiniconDamageBonus::taMiniconDamageBonus(dictItem copyItem){
    SetValues(copyItem);
    this->MeleeDamageBonus = copyItem.searchAttributes<float>("MeleeDamageBonus");
}

taMiniconRangeBonus::taMiniconRangeBonus(dictItem copyItem){
    SetValues(copyItem);
    this->RangeBonus = copyItem.searchAttributes<float>("RangeBonus");
}

taMiniconRegeneration::taMiniconRegeneration(dictItem copyItem){
    SetValues(copyItem);
    this->HealthRegenPerSecond = copyItem.searchAttributes<float>("HealthRegenPerSecond");
    this->PowerlinkedHealthRegenPerSecond = copyItem.searchAttributes<float>("PowerlinkedHealthRegenPerSecond");
}*/

void DataHandler::LoadMiniconType(QString miniconType){
    std::vector<taMinicon> metagameMinicons;
    metagameMinicons = convertInstances<taMinicon>(gameData.metagameFile->sendInstances(miniconType));
    for(int i = 0; i < metagameMinicons.size(); i++){
        if(metagameMinicons[i].minicon != MiniconNone){
            gameData.miniconList.push_back(metagameMinicons[i]);
        }
    }
    qDebug() << Q_FUNC_INFO << "Minicon list now has:" << gameData.miniconList.size() << "loaded minicons";
}

void DataHandler::SetMiniconAttributes(const taMinicon *minicon, dictItem *itemToEdit){
    itemToEdit->setAttribute("ActivationType", QString::number(minicon->activationType));
    itemToEdit->setAttribute("ChargeDrainMultiplier_Commander", QString::number(minicon->chargeDrainRate_Commander));
    itemToEdit->setAttribute("ChargeDrainMultiplier_Veteran", QString::number(minicon->chargeDrainRate_Veteran));
    itemToEdit->setAttribute("ChargeDrainTime", QString::number(minicon->chargeDrainTime));
    itemToEdit->setAttribute("CoolDownTime", QString::number(minicon->coolDownTime));
    itemToEdit->setAttribute("CoolDownTimeDepleted", QString::number(minicon->coolDownTimeDepleted));
    itemToEdit->setAttribute("CrosshairIndex", QString::number(minicon->crosshairIndex));
    itemToEdit->setAttribute("EquipInHQ", QString::number(minicon->equipInHQ));
    itemToEdit->setAttribute("Icon", QString::number(minicon->icon));
    itemToEdit->setAttribute("Minicon", QString::number(minicon->minicon));
    itemToEdit->setAttribute("MinimumChargeToUse", QString::number(minicon->minimumChargeToUse));
    itemToEdit->setAttribute("MinimumChargeToUsePerShot", QString::number(minicon->minimumChargeToUsePerShot));
    itemToEdit->setAttribute("Name", minicon->name);
    itemToEdit->setAttribute("NodeToKeepIndex", QString::number(minicon->nodeToKeepIndex));
    itemToEdit->setAttribute("PaletteIndex", QString::number(minicon->paletteIndex));
    itemToEdit->setAttribute("PowerCost", QString::number(minicon->powerCost));
    itemToEdit->setAttribute("RechargeTime", QString::number(minicon->rechargeTime));
    itemToEdit->setAttribute("RecoilType", QString::number(minicon->recoilType));
    itemToEdit->setAttribute("RestrictToButton", QString::number(minicon->restrictToButton));
    itemToEdit->setAttribute("Segments", QString::number(minicon->segments));
    itemToEdit->setAttribute("SidekickCoolDownTime", QString::number(minicon->sidekickCoolDownTime));
    itemToEdit->setAttribute("SidekickRechargeTime", QString::number(minicon->sidekickRechargeTime));
    itemToEdit->setAttribute("SidekickSegments", QString::number(minicon->sidekickSegments));
    itemToEdit->setAttribute("Slot", QString::number(minicon->slot));
    itemToEdit->setAttribute("Team", QString::number(minicon->team));
    itemToEdit->setAttribute("ToneLibrary", minicon->toneLibrary);
}

dictItem DataHandler::createMetagameMinicon(const taMinicon *minicon){

    dictItem convertedData;
    convertedData.name = "Minicon";
    convertedData.attributes = gameData.metagameFile->generateAttributes("Minicon");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(const taMiniconArmor *minicon){

    dictItem convertedData;
    convertedData.name = "MiniconArmor";
    convertedData.attributes = gameData.metagameFile->generateAttributes("MiniconArmor");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);
    convertedData.setAttribute("AbsorbPercentage", QString::number(minicon->AbsorbPercentage));
    convertedData.setAttribute("MaxAbsorbPerHit", QString::number(minicon->MaxAbsorbPerHit));
    convertedData.setAttribute("MaxDamageToAbsorb", QString::number(minicon->MaxDamageToAbsorb));

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(const taMiniconDamageBonus *minicon){

    dictItem convertedData;
    convertedData.name = "MiniconDamageBonus";
    convertedData.attributes = gameData.metagameFile->generateAttributes("MiniconDamageBonus");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);
    convertedData.setAttribute("MeleeDamageBonus", QString::number(minicon->MeleeDamageBonus));

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(const taMiniconEmergencyWarpgate *minicon){

    dictItem convertedData;
    convertedData.name = "MiniconEmergencyWarpgate";
    convertedData.attributes = gameData.metagameFile->generateAttributes("MiniconEmergencyWarpgate");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);
    convertedData.setAttribute("TimeActive", QString::number(minicon->TimeActive));

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(const taMiniconRangeBonus *minicon){

    dictItem convertedData;
    convertedData.name = "MiniconRangeBonus";
    convertedData.attributes = gameData.metagameFile->generateAttributes("MiniconRangeBonus");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);
    convertedData.setAttribute("RangeBonus", QString::number(minicon->RangeBonus));

    return convertedData;
}

dictItem DataHandler::createMetagameMinicon(const taMiniconRegeneration *minicon){

    dictItem convertedData;
    convertedData.name = "MiniconRegeneration";
    convertedData.attributes = gameData.metagameFile->generateAttributes("MiniconRegeneration");
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    SetMiniconAttributes(minicon, &convertedData);
    convertedData.setAttribute("HealthRegenPerSecond", QString::number(minicon->HealthRegenPerSecond));
    convertedData.setAttribute("PowerlinkedHealthRegenPerSecond", QString::number(minicon->PowerlinkedHealthRegenPerSecond));

    return convertedData;
}
