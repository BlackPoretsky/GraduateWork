#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "AfterInventoryItemDefinition.generated.h"

class UAfterInventoryItemInstance;

UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class AFTER_API UAfterInventoryItemFragment : public UObject
{
    GENERATED_BODY()

public:
    virtual void OnInstanceCreated(UAfterInventoryItemInstance* Instance) const {};
};

UCLASS(Blueprintable, Const, Abstract)
class AFTER_API UAfterInventoryItemDefinition : public UObject
{
    GENERATED_BODY()

public:
    UAfterInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    const UAfterInventoryItemFragment* FindFragmentByClass(TSubclassOf<UAfterInventoryItemFragment> FragmentClass) const;

    UFUNCTION(BlueprintCallable, Category = "Display")
    FText GetItemDisplayName() const { return DisplayName; }

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display")
    FText DisplayName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Display", Instanced)
    TArray<TObjectPtr<UAfterInventoryItemFragment>> Fragments;
};

UCLASS()
class UAfterInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

    UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
    static const UAfterInventoryItemFragment* FindItemDefinitionFragment(
        TSubclassOf<UAfterInventoryItemDefinition> ItemDef, TSubclassOf<UAfterInventoryItemFragment> FragmentClass);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "After|WeaponPickup")
    static int32 GetDefaultStatFromItemDef(const TSubclassOf<UAfterInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag);
};