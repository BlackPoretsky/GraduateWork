#pragma once

#include "Equipment/AfterEquipmentInstance.h"

#include "AfterWeaponInstance.generated.h"

UCLASS()
class UAfterWeaponInstance : public UAfterEquipmentInstance
{
    GENERATED_BODY()

public:
    UAfterWeaponInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~UAfterEquipmentInstance interface
    virtual void OnEquipped() override;
    virtual void OnUnequipped() override;
    //~End of UAfterEquipmentInstance interface

    UFUNCTION(BlueprintCallable)
    void UpdateFiringTime();

    UFUNCTION(BlueprintPure)
    float GetTimeSinceLastInteractedWith() const;

private:
    double TimeLastEquipped = 0.0;
    double TimeLastFired = 0.0;
};
