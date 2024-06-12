#pragma once

#include "Inventory/AfterInventoryItemDefinition.h"
#include "Styling/SlateBrush.h"

#include "InventoryFragment_QuickBarIcon.generated.h"

class UObject;

UCLASS()
class UInventoryFragment_QuickBarIcon : public UAfterInventoryItemFragment
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    FSlateBrush Brush;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    FSlateBrush AmmoBrush;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    FText DisplayNameWhenEquipped;
};
