#ifndef TAMINICON_H
#define TAMINICON_H

#include "Headers/Databases/Database.h"

enum ActivationType{ActivationTypeInvalid = -1, ActivationTypeHoldDown, ActivationTypeAlwaysOn, ActivationTypePressInstant, ActivationTypeReleaseInstant, ActivationTypePressAndRelease, ActivationTypeChargeRelease
                      , ActivationTypeChargeReleasePartial, ActivationTypeToggle, ActivationTypeToggleDrain, ActivationTypeHoldDownDrain};
enum CrosshairIndex{CrosshairIndexNone, CrosshairIndexAssaultBlaster, CrosshairIndexLockon, CrosshairIndexSlingshot, CrosshairIndexClaymore, CrosshairIndexTractor, CrosshairIndexFirefight, CrosshairIndexHailstorm
                      , CrosshairIndexLandslide, CrosshairIndexSmackdown, CrosshairIndexWatchdog, CrosshairIndexLookout, CrosshairIndexSparkjump, CrosshairIndexSkirmish, CrosshairIndexAirburst
                      , CrosshairIndexSandstorm, CrosshairIndexTwister, CrosshairIndexDiscord, CrosshairIndexCorona, CrosshairIndexAftershock, CrosshairIndexAurora, CrosshairIndexFailsafe, CrosshairIndexJumpstart
                      , CrosshairIndexEndgame, CrosshairIndexUltimatePrimary, CrosshairIndexUltimateSecondary};
enum Icon{IconArmor, IconBeam, IconBlaster, IconExotic, IconLobbed, IconMelee, IconMissile, IconMovement, IconRepair, IconShield, IconStealth, IconVision, IconNone};
enum Minicon{MiniconNone, MiniconAssaultBlaster, MiniconHeavyAssaultBlaster, MiniconSuperHeavyAssaultBlaster, MiniconSuperHeavyCannon, MiniconHeavyElectricArcGun, MiniconSuperHeavyElectricArcGun
            , MiniconImpactGun, MiniconHeavyRibbonBeam, MiniconSuperHeavyRibbonBeam, MiniconUltimatePrimary, MiniconClusterRocket, MiniconHeavyClusterRocket, MiniconDarkEnergonBlaster
            , MiniconDecoyLauncher, MiniconEMPBlast, MiniconFlakBurst, MiniconGrenadeLauncher, MiniconHomingMissile, MiniconLimpetMine, MiniconDropMine, MiniconSniperRifle, MiniconVortexCannon, MiniconUltimateSecondary
            , MiniconElectroField, MiniconGlide, MiniconUltimateGlide, MiniconAreaEffectScrambler, MiniconArmor, MiniconHeavyArmor, MiniconSuperHeavyArmor, MiniconDash, MiniconEmergencyWarpGate, MiniconEnergonMeleeWeapon
            , EnergonScreen, MiniconEnergonVision, MiniconEnergonShield, MiniconEnhancedDash, MiniconMeleeDamageEnhance, MiniconMiniconSensor, MiniconWeaponDistanceEnhance, MiniconReflectionShield
            , MiniconStealth, MiniconTractorBeam, MiniconUltimateShield, MiniconAutoRepair, MiniconRevive, MiniconBoostJump, MiniconHeavyHomingMissile};
enum RecoilType{RecoilTypeNone, RecoilTypeSmall, RecoilTypeLarge};
enum RestrictToButton{RestrictToButtonDefault, RestrictToButtonL2, RestrictToButtonR2, RestrictToButtonL1, RestrictToButtonR1};
enum Slot{SlotPrimaryWeapon, SlotSecondaryWeapon, SlotMovement, SlotAbility};
enum Team{TeamGreen, TeamBlue, TeamRed, TeamPurple, TeamGold, TeamNone};
enum UI_DamagePri{UI_DamagePriNA, UI_DamagePriUseFloat, UI_DamagePriUseFloatX2, UI_DamagePriUseFloatX4, UI_DamagePriVariable, UI_DamagePriAutobotMeleexFloat, UI_DamagePriplusFloat, UI_DamagePriFloatPerSec};
enum UI_DamageSec{UI_DamageSecNA, UI_DamageSecUseFloat, UI_DamageSecUseFloatx4, UI_DamageSecVariable, UI_DamageSecSpecial, UI_DamageSecFloatPerSec};
enum UI_Defence{UI_DefenceNA, UI_DefenceFloat, UI_DefenceAllBeamAndBlaster, UI_DefenceAllBeamAndMelee, UI_DefenceAllMissileAndMelee};
enum UI_Duration{UI_DurationNA, UI_DurationConstant, UI_DurationFloatBoosts, UI_DurationFloatSec, UI_DurationFloat, UI_DurationInstant};
enum UI_Range{UI_RangeNA, UI_RangeUseRangeFloat, UI_RangeMelee, UI_RangeLineOfSight, UI_RangePlusPercent, UI_RangeDrop};
enum UI_Recharge{UI_RechargeNA, UI_RechargeFloatPtsPerSec, UI_RechargeFloatSec, UI_RechargeFloatSecOrHQ, UI_RechargeHQ};
enum UI_Rounds{UI_RoundsNA, UI_RoundsUseInt, UI_RoundsVariable};
class taMinicon : public taDatabaseInstance{
    /*From METAGAME.TMD*/
public:
    int activationType;
    float chargeDrainRate_Commander;
    float chargeDrainRate_Veteran;
    float chargeDrainTime;
    float coolDownTime;
    float coolDownTimeDepleted;
    int crosshairIndex;
    QString description;
    bool equipInHQ;
    int icon;
    int minicon;
    float minimumChargeToUse;
    float minimumChargeToUsePerShot;
    QString name;
    int nodeToKeepIndex;
    int paletteIndex;
    int powerCost;
    float rechargeTime;
    int recoilType;
    int restrictToButton;
    int segments;
    float sidekickChargeDrainTime;
    float sidekickCoolDownTime;
    float sidekickCoolDownTimeDepleted;
    float sidekickMinimumChargeToUse;
    float sidekickRechargeTime;
    int sidekickSegments;
    int slot;
    int team;
    QString toneLibrary;
    int uiDamagePri;
    float UI_DamagePriFloat;
    int uiDamageSec;
    float UI_DamageSecFloat;
    int uiDefence;
    int uiDuration;
    float UI_DurationFloat;
    int uiRange;
    float UI_RangeDist;
    int UI_Recharge;
    int uiRounds;
    int UI_RoundsInt;

    taMinicon();
    taMinicon(dictItem copyItem);
    void SetValues(dictItem copyItem);
};

class taMiniconArmor : public taMinicon {
public:
    float AbsorbPercentage;
    float MaxAbsorbPerHit;
    float MaxDamageToAbsorb;
    //taMiniconArmor(dictItem copyItem);
};

class taMiniconDamageBonus : public taMinicon {
public:
    float MeleeDamageBonus;
    //taMiniconDamageBonus(dictItem copyItem);
};

class taMiniconEmergencyWarpgate : public taMinicon {
public:
    float TimeActive;
    //taMiniconEmergencyWarpgate(dictItem copyItem);
};

class taMiniconRangeBonus : public taMinicon {
public:
    float RangeBonus;
    //taMiniconRangeBonus(dictItem copyItem);
};

class taMiniconRegeneration : public taMinicon {
public:
    float HealthRegenPerSecond;
    float PowerlinkedHealthRegenPerSecond;
    //taMiniconRegeneration(dictItem copyItem);
};

#endif // TAMINICON_H
