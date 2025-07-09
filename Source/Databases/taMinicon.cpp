/*Includes functions related to minicons from the taMinicon class and datahandler
taking this as a sign that datahandler is doing too much and that taminicon should be taking some of the load
or just that datahandler should be split up a bit in some way. but the lines are getting fuzzier
and my patience shorter.*/

#include "Databases/DataHandler.h"
#include "Databases/taMinicon.h"

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

taMiniconArmor::taMiniconArmor(dictItem copyItem){
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
}

taMiniconEmergencyWarpgate::taMiniconEmergencyWarpgate(dictItem copyItem){
    SetValues(copyItem);
    this->TimeActive = copyItem.searchAttributes<float>("TimeActive");
}

void DataHandler::LoadMiniconType(QString miniconType){
    /*this is awful but it's 3am and it compiles and it works*/
    std::vector<std::shared_ptr<taMinicon>> metagameMinicons;
    std::vector<std::shared_ptr<taMiniconDamageBonus>> metagameMiniconDamageBonus;
    std::vector<std::shared_ptr<taMiniconArmor>> metagameMiniconArmor;
    std::vector<std::shared_ptr<taMiniconEmergencyWarpgate>> metagameMiniconEmergencyWarpgate;
    std::vector<std::shared_ptr<taMiniconRangeBonus>> metagameMiniconRangeBonus;
    std::vector<std::shared_ptr<taMiniconRegeneration>> metagameMiniconRegeneration;
    switch(m_miniconTypes.indexOf(miniconType)){
    case 0: //Minicon
        convertInstances<taMinicon>(gameData.metagameFile->sendInstances(miniconType), &metagameMinicons);
        break;
    case 1: //MiniconDamageBonus
        convertInstances<taMiniconDamageBonus>(gameData.metagameFile->sendInstances(miniconType), &metagameMiniconDamageBonus);
        for(int i = 0; i < metagameMiniconDamageBonus.size(); i++){
            metagameMinicons.push_back(metagameMiniconDamageBonus[i]);
        }
        break;
    case 2: //MiniconArmor
        convertInstances<taMiniconArmor>(gameData.metagameFile->sendInstances(miniconType), &metagameMiniconArmor);
        for(int i = 0; i < metagameMiniconArmor.size(); i++){
            metagameMinicons.push_back(metagameMiniconArmor[i]);
        }
        break;
    case 3: //MiniconEmergencyWarpgate
        convertInstances<taMiniconEmergencyWarpgate>(gameData.metagameFile->sendInstances(miniconType), &metagameMiniconEmergencyWarpgate);
        for(int i = 0; i < metagameMiniconEmergencyWarpgate.size(); i++){
            metagameMinicons.push_back(metagameMiniconEmergencyWarpgate[i]);
        }
        break;
    case 4: //MiniconRangeBonus
        convertInstances<taMiniconRangeBonus>(gameData.metagameFile->sendInstances(miniconType), &metagameMiniconRangeBonus);
        for(int i = 0; i < metagameMiniconRangeBonus.size(); i++){
            metagameMinicons.push_back(metagameMiniconRangeBonus[i]);
        }
        break;
    case 5: //MiniconRegeneration
        convertInstances<taMiniconRegeneration>(gameData.metagameFile->sendInstances(miniconType), &metagameMiniconRegeneration);
        for(int i = 0; i < metagameMiniconRegeneration.size(); i++){
            metagameMinicons.push_back(metagameMiniconRegeneration[i]);
        }
        break;
    default:
        convertInstances<taMinicon>(gameData.metagameFile->sendInstances(miniconType), &metagameMinicons);
        break;
    }

    for(int i = 0; i < metagameMinicons.size(); i++){
        if(metagameMinicons[i]->minicon != MiniconNone){
            gameData.miniconList.push_back(metagameMinicons[i]);
        }
    }
    qDebug() << Q_FUNC_INFO << "Minicon list now has:" << gameData.miniconList.size() << "loaded minicons";
}

const void taMinicon::SetBaseAttributes(dictItem *itemToEdit){
    itemToEdit->setAttribute("ActivationType", QString::number(activationType));
    itemToEdit->setAttribute("ChargeDrainMultiplier_Commander", QString::number(chargeDrainRate_Commander));
    itemToEdit->setAttribute("ChargeDrainMultiplier_Veteran", QString::number(chargeDrainRate_Veteran));
    itemToEdit->setAttribute("ChargeDrainTime", QString::number(chargeDrainTime));
    itemToEdit->setAttribute("CoolDownTime", QString::number(coolDownTime));
    itemToEdit->setAttribute("CoolDownTimeDepleted", QString::number(coolDownTimeDepleted));
    itemToEdit->setAttribute("CrosshairIndex", QString::number(crosshairIndex));
    itemToEdit->setAttribute("EquipInHQ", QString::number(equipInHQ));
    itemToEdit->setAttribute("Icon", QString::number(icon));
    itemToEdit->setAttribute("Minicon", QString::number(minicon));
    itemToEdit->setAttribute("MinimumChargeToUse", QString::number(minimumChargeToUse));
    itemToEdit->setAttribute("MinimumChargeToUsePerShot", QString::number(minimumChargeToUsePerShot));
    itemToEdit->setAttribute("Name", name);
    itemToEdit->setAttribute("NodeToKeepIndex", QString::number(nodeToKeepIndex));
    itemToEdit->setAttribute("PaletteIndex", QString::number(paletteIndex));
    itemToEdit->setAttribute("PowerCost", QString::number(powerCost));
    itemToEdit->setAttribute("RechargeTime", QString::number(rechargeTime));
    itemToEdit->setAttribute("RecoilType", QString::number(recoilType));
    itemToEdit->setAttribute("RestrictToButton", QString::number(restrictToButton));
    itemToEdit->setAttribute("Segments", QString::number(segments));
    itemToEdit->setAttribute("SidekickCoolDownTime", QString::number(sidekickCoolDownTime));
    itemToEdit->setAttribute("SidekickRechargeTime", QString::number(sidekickRechargeTime));
    itemToEdit->setAttribute("SidekickSegments", QString::number(sidekickSegments));
    itemToEdit->setAttribute("Slot", QString::number(slot));
    itemToEdit->setAttribute("Team", QString::number(team));
    itemToEdit->setAttribute("ToneLibrary", toneLibrary);
}

dictItem DataHandler::createMetagameMinicon(taMinicon* minicon){

    dictItem convertedData;
    convertedData.name = minicon->Type();
    convertedData.attributes = gameData.metagameFile->generateAttributes(minicon->Type());
    qDebug() << Q_FUNC_INFO << "new item has" << convertedData.attributes.size() << "attributes";

    minicon->SetBaseAttributes(&convertedData);
    minicon->SetSpecificAttributes(&convertedData);

    return convertedData;
}

void taMinicon::SetSpecificAttributes(dictItem *itemToEdit){
    qDebug() << Q_FUNC_INFO << "Called base function";
    return;
}

void taMiniconArmor::SetSpecificAttributes(dictItem *itemToEdit){
    qDebug() << Q_FUNC_INFO << "CALLED ARMOR FUNCTION CORRECTLY";

    itemToEdit->setAttribute("AbsorbPercentage", QString::number(AbsorbPercentage));
    itemToEdit->setAttribute("MaxAbsorbPerHit", QString::number(MaxAbsorbPerHit));
    itemToEdit->setAttribute("MaxDamageToAbsorb", QString::number(MaxDamageToAbsorb));
}

void taMiniconDamageBonus::SetSpecificAttributes(dictItem *itemToEdit){
    itemToEdit->setAttribute("MeleeDamageBonus", QString::number(MeleeDamageBonus));
}

void taMiniconEmergencyWarpgate::SetSpecificAttributes(dictItem *itemToEdit){
    itemToEdit->setAttribute("TimeActive", QString::number(TimeActive));
}

void taMiniconRangeBonus::SetSpecificAttributes(dictItem *itemToEdit){
    itemToEdit->setAttribute("RangeBonus", QString::number(RangeBonus));
}

void taMiniconRegeneration::SetSpecificAttributes(dictItem *itemToEdit){
    itemToEdit->setAttribute("HealthRegenPerSecond", QString::number(HealthRegenPerSecond));
    itemToEdit->setAttribute("PowerlinkedHealthRegenPerSecond", QString::number(PowerlinkedHealthRegenPerSecond));
}
