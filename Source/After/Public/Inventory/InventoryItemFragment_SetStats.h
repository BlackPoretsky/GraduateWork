#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/AfterInventoryItemDefinition.h"

#include "InventoryItemFragment_SetStats.generated.h"

class UAfterInventoryItemInstance;

UCLASS()
class UInventoryItemFragment_SetStats : public UAfterInventoryItemFragment
{
    GENERATED_BODY()

public:
    virtual void OnInstanceCreated(UAfterInventoryItemInstance* Instance) const override;

    int32 GetItemStatByTag(FGameplayTag Tag) const;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TMap<FGameplayTag, int32> InitialItemStats;
};
