#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/Interface.h"

#include "Pickupable.generated.h"

template <typename InterfaceType>
class TScriptInterface;

class AActor;
class UAfterInventoryItemDefinition;
class UAfterInventoryItemInstance;
class UAfterInventoryManagerComponent;
class UObject;

USTRUCT(BlueprintType)
struct FPickupTemplate
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere)
    int32 StackCount = 1;

    UPROPERTY(EditAnywhere)
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct FPickupInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UAfterInventoryItemInstance> Item = nullptr;
};

USTRUCT(BlueprintType)
struct FInventoryPickup
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FPickupInstance> Instances;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TArray<FPickupTemplate> Templates;
};

UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class IPickupable
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    virtual FInventoryPickup GetPickupInventory() const = 0;
};

UCLASS()
class UPickupbleStatics : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UPickupbleStatics();

    UFUNCTION(BlueprintPure)
    static TScriptInterface<IPickupable> GetFirstPickupableFromActor(AActor* Actor);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
    static void AddPickupToInventory(UAfterInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup);
};