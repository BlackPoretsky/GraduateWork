#pragma once

#include "Inventory/AfterInventoryItemDefinition.h"
#include "InventoryFragment_PickupIcon.generated.h"

class UObject;
class USkeletalMesh;

UCLASS()
class AFTER_API UInventoryFragment_PickupIcon : public UAfterInventoryItemFragment
{
    GENERATED_BODY()

public:
    UInventoryFragment_PickupIcon();

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    TObjectPtr<USkeletalMesh> SkeletalMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    FText DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
    FLinearColor PadColor;
};
