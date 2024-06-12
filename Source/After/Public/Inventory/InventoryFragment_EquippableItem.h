#pragma once

#include "Inventory/AfterInventoryItemDefinition.h"
#include "InventoryFragment_EquippableItem.generated.h"

class UAfterEquipmentDefinition;
class UObject;

UCLASS()
class AFTER_API UInventoryFragment_EquippableItem : public UAfterInventoryItemFragment
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = "After")
    TSubclassOf<UAfterEquipmentDefinition> EquipmentDefinition;
};
