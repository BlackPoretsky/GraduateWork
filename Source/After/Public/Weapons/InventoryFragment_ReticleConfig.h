#pragma once

#include "Inventory/AfterInventoryItemDefinition.h"

#include "InventoryFragment_ReticleConfig.generated.h"

class UAfterReticleWidgetBase;

UCLASS()
class UInventoryFragment_ReticleConfig : public UAfterInventoryItemFragment
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reticle")
    TArray<TSubclassOf<UAfterReticleWidgetBase>> ReticleWidgets;
};
