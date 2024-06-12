#include "Inventory/AfterInventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemFragment_SetStats.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterInventoryItemDefinition)

///////////////////////////////////
// UAfterInventoryItemDefinition //
///////////////////////////////////
UAfterInventoryItemDefinition::UAfterInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

const UAfterInventoryItemFragment* UAfterInventoryItemDefinition::FindFragmentByClass(
    TSubclassOf<UAfterInventoryItemFragment> FragmentClass) const
{
    if (FragmentClass)
    {
        for (UAfterInventoryItemFragment* Fragment : Fragments)
        {
            if (Fragment && Fragment->IsA(FragmentClass))
            {
                return Fragment;
            }
        }
    }

    return nullptr;
}

////////////////////////////////////
// UAfterInventoryFunctionLibrary //
////////////////////////////////////
const UAfterInventoryItemFragment* UAfterInventoryFunctionLibrary::FindItemDefinitionFragment(
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef, TSubclassOf<UAfterInventoryItemFragment> FragmentClass)
{
    if ((!ItemDef) && (FragmentClass != nullptr))
    {
        return GetDefault<UAfterInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
    }

    return nullptr;
}

int32 UAfterInventoryFunctionLibrary::GetDefaultStatFromItemDef(
    const TSubclassOf<UAfterInventoryItemDefinition> WeaponItemClass, FGameplayTag StatTag)
{
    if (WeaponItemClass != nullptr)
    {
        if (UAfterInventoryItemDefinition* WeaponItemCDO = WeaponItemClass->GetDefaultObject<UAfterInventoryItemDefinition>())
        {
            if (const UInventoryItemFragment_SetStats* ItemStatsFragment = Cast<UInventoryItemFragment_SetStats>(
                    WeaponItemCDO->FindFragmentByClass(UInventoryItemFragment_SetStats::StaticClass())))
            {
                return ItemStatsFragment->GetItemStatByTag(StatTag);
            }
        }
    }

    return 0;
}
