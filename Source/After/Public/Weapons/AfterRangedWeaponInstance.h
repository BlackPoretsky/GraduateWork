#pragma once

#include "Curves/CurveFloat.h"

#include "Weapons/AfterWeaponInstance.h"
#include "AbilitySystem/AfterAbilitySourceInterface.h"

#include "AfterRangedWeaponInstance.generated.h"

UCLASS()
class UAfterRangedWeaponInstance : public UAfterWeaponInstance, public IAfterAbilitySourceInterface
{
    GENERATED_BODY()

public:
    UAfterRangedWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    int32 GetBulletsPerCartridge() const { return BulletsPerCartridge; }

    float GetCalculatedSpreadAngleMultiplier() const { return bHasFirstShotAccuracy ? 0.0f : CurrentSpreadAngleMultiplier; }

    bool HasFirstShotAccuracy() const { return bHasFirstShotAccuracy; }

    float GetSpreadExponent() const { return SpreadExponent; }

    float GetMaxDamageRange() const { return MaxDamageRange; }

    float GetBulletTraceSweepRadius() const { return BulletTraceSweepRadius; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.1), Category = "Spread|Fire Params")
    float SpreadExponent = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Spread|Fire Params", meta = (ForceUnits = s))
    float SpreadRecoveryCooldownDelay = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Spread|Fire Params")
    bool bAllowFirstShotAccuracy = false;

    UPROPERTY(EditAnywhere, Category = "Spread|Player Params", meta = (ForceUnits = x))
    float SpreadAngleMultiplier_Aiming = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
    float SpreadAngleMultiplier_StandingStill = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
    float TransitionRate_StandingStill = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
    float StandingStillSpeedThreshold = 80.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = "cm/s"))
    float StandingStillToMovingSpeedRange = 20.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
    float SpreadAngleMultiplier_Crouching = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
    float TransitionRate_Crouching = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params", meta = (ForceUnits = x))
    float SpreadAngleMultiplier_JumpingOrFalling = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spread|Player Params")
    float TransitionRate_JumpingOrFalling = 5.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config")
    int32 BulletsPerCartridge = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
    float MaxDamageRange = 25000.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Config", meta = (ForceUnits = cm))
    float BulletTraceSweepRadius = 0.0f;

    UPROPERTY(EditAnywhere, Category = "Weapon Config")
    FRuntimeFloatCurve DistanceDamageFalloff;

private:
    double LastFireTime = 0.0;

    bool bHasFirstShotAccuracy = false;

    float CurrentSpreadAngleMultiplier = 1.0f;

    float StandingStillMultiplier = 1.0f;

    float JumpFallMultiplier = 1.0f;

    float CrouchingMultiplier = 1.0f;

public:
    void Tick(float DeltaSeconds);

    //~UAfterEquipmentInstance interface
    virtual void OnEquipped();
    virtual void OnUnequipped();
    //~End of UAfterEquipmentInstance interface

    //~IAfterAbilitySourceInterface interface
    virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr,
        const FGameplayTagContainer* TargetTags = nullptr) const override;
    //~End of IAfterAbilitySourceInterface interface

private:
    bool UpdateMultipliers(float DeltaSeconds);
};
