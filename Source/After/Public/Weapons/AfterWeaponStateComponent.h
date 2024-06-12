#pragma once

#include "Components/ControllerComponent.h"
#include "GameplayTagContainer.h"

#include "AfterWeaponStateComponent.generated.h"

struct FGamepalyAbilityTargetDataHandle;
struct FGameplayEffectContextHandle;

struct FAfterScreenSpaceHitLocation
{
    FVector2D Location;
    FGameplayTag HitZone;
    bool bShowAsSuccess = false;
};

UCLASS()
class UAfterWeaponStateComponent : public UControllerComponent
{
    GENERATED_BODY()

public:
    UAfterWeaponStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void TickComponent(float DeltaTimem, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFinction) override;

    void UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext);

    void GetLastWeaponDamageScreenLocations(TArray<FAfterScreenSpaceHitLocation>& WeaponDamageScreenLocation)
    {
        WeaponDamageScreenLocation = LastWeaponDamageScreenLocations;
    }

    double GetTimeSinceLastHitNotification() const;

protected:
    virtual bool ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const;

    void ActuallyUpdateDamageInstigatedTime();

private:
    double LastWeaponDamageInstigatedTime = 0.0;

    TArray<FAfterScreenSpaceHitLocation> LastWeaponDamageScreenLocations;
};
