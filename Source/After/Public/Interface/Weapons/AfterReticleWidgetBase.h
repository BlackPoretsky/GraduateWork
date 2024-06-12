#pragma once

#include "CommonUserWidget.h"

#include "AfterReticleWidgetBase.generated.h"

class UAfterInventoryItemInstance;
class UAfterWeaponInstance;

UCLASS(Abstract)
class UAfterReticleWidgetBase : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UAfterReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintImplementableEvent)
    void OnWeaponInitialized();

    UFUNCTION(BlueprintCallable)
    void InitializeFromWeapon(UAfterWeaponInstance* InWeapon);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    float ComputeSpreadAngle() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    float ComputeMaxScreenspaceSpreadRadius() const;

    UFUNCTION(BlueprintCallable, BlueprintPure)
    bool HasFirstShotAccuracy() const;

protected:
    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAfterWeaponInstance> WeaponInstance;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<UAfterInventoryItemInstance> InventoryInstance;
};
