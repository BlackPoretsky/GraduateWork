#pragma once

#include "Components/ActorComponent.h"

#include "AfterInventoryManagerComponent.generated.h"

class UAfterInventoryItemDefinition;
class UAfterInventoryItemInstance;
class UAfterManagerComponent;

struct FAfterInventoryList;

USTRUCT(BlueprintType)
struct FAfterInventoryList
{
    GENERATED_BODY()

    FAfterInventoryList()
        : OwnerComponent(nullptr)
    {
    }

    FAfterInventoryList(UActorComponent* InOwnerComponent)
        : OwnerComponent(InOwnerComponent)
    {
    }

    TArray<UAfterInventoryItemInstance*> GetAllItems() const;

public:
    UAfterInventoryItemInstance* AddEntry(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount);
    void AddEntry(UAfterInventoryItemInstance* Instance);

    void RemoveEntry(UAfterInventoryItemInstance* Instance);

private:
    friend UAfterInventoryManagerComponent;

private:
    UPROPERTY()
    TArray<UAfterInventoryItemInstance*> Entries;

    UPROPERTY()
    TObjectPtr<UActorComponent> OwnerComponent;
};

UCLASS(BlueprintType)
class AFTER_API UAfterInventoryManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAfterInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    bool CanAddItemDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount = 1);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    UAfterInventoryItemInstance* AddItemDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount = 1);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    void AddItemInstance(UAfterInventoryItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    void RemoveItemInstance(UAfterInventoryItemInstance* ItemInstance);

    UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintPure = false)
    TArray<UAfterInventoryItemInstance*> GetAllItems() const;

    UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintPure)
    UAfterInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef) const;

    int32 GetTotatlItemCountByDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 NumToConsume);
    bool ConsumeItemByDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 NumToConsume);

private:
    UPROPERTY()
    FAfterInventoryList InventoryList;
};