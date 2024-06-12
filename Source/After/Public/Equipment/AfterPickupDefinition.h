#pragma once

#include "Engine/DataAsset.h"
#include "AfterPickupDefinition.generated.h"

class UAfterInventoryItemDefinition;
class UNiagaraSystem;
class USoundBase;
class UStaticMesh;

UCLASS(Blueprintable, BlueprintType, Const,
    Meta = (DisplayName = "After Pickup Data", ShortTooltip = "Data asset used to configure a pickup."))
class AFTER_API UAfterPickupDefinition : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pickup|Equipment")
    TSubclassOf<UAfterInventoryItemDefinition> InventoryItemDefinition;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pickup|Mesh")
    TObjectPtr<UStaticMesh> DisplayMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pickup")
    TObjectPtr<USoundBase> PickedUpSound;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pickup")
    TObjectPtr<UNiagaraSystem> PickedUpEffect;
};

UCLASS(Blueprintable, BlueprintType, Const,
    Meta = (DisplayName = "After Weapon Pickup Data", ShortTooltip = "Data asset used to configure a weapon pickup."))
class AFTER_API UAfterWeaponPickupDefinition : public UAfterPickupDefinition
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Afer|Pickup|Mesh")
    FVector WeaponMeshOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pickup|Mesh")
    FVector WeaponMeshScale = FVector(1.0f, 1.0f, 1.0f);
};
