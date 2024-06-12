#pragma once

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"

#include "AfterGA_FromEquipment.generated.h"

class UAfterEquipmentInstance;
class UAfterInventoryItemInstance;

UCLASS()
class AFTER_API UAfterGA_FromEquipment : public UAfterGameplayAbility
{
    GENERATED_BODY()

public:
    UAfterGA_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    UAfterEquipmentInstance* GetAssociatedEquipment() const;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    UAfterInventoryItemInstance* GetAssociatedItem() const;
};
