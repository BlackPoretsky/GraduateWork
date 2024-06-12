#pragma once

#include "System/GameplayTagStack.h"
#include "AfterInventoryItemInstance.generated.h"

class FLifetimeProperty;

class UAfterInventoryItemDefinition;
class UAfterInventoryItemFragment;
struct FGameplaytag;

UCLASS(BlueprintType)
class AFTER_API UAfterInventoryItemInstance : public UObject
{
    GENERATED_BODY()

public:
    UAfterInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~UObject() interface
    virtual bool IsSupportedForNetworking() const override { return true; }
    //~End of UObject() interface

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
    void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    int32 GetStatTagStackCount(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    FText GetItemDisplayName() const { return ItemDisplayName; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void SetItemDisplayName(FText InItemDisplayName) { ItemDisplayName = InItemDisplayName; }

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool HasStatTag(FGameplayTag Tag) const;

    TSubclassOf<UAfterInventoryItemDefinition> GetItemDef() const { return ItemDef; }

    UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = "FragmentClass"))
    const UAfterInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAfterInventoryItemFragment> FragmentClass) const;

    template <typename ResultClass>
    const ResultClass* FindFragmentByClass() const
    {
        return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
    }

private:
    void SetItemDef(TSubclassOf<UAfterInventoryItemDefinition> InDef);

    friend struct FAfterInventoryList;

private:
    FGameplayTagStackContainer StatTags;

    FText ItemDisplayName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef;
};
