#include "Inventory/AfterInventoryItemInstance.h"
#include "Inventory/AfterInventoryItemDefinition.h"

#include "System/GameplayTagStack.h"
#include "GameplayTagContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterInventoryItemInstance)

UAfterInventoryItemInstance::UAfterInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
    StatTags.AddStack(Tag, StackCount);
}

void UAfterInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
    StatTags.RemoveStack(Tag, StackCount);
}

int32 UAfterInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
    return StatTags.GetStackCount(Tag);
}

bool UAfterInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
    return StatTags.ContainsTag(Tag);
}

const UAfterInventoryItemFragment* UAfterInventoryItemInstance::FindFragmentByClass(
    TSubclassOf<UAfterInventoryItemFragment> FragmentClass) const
{
    if ((ItemDef != nullptr) && (FragmentClass != nullptr))
    {
        return GetDefault<UAfterInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
    }

    return nullptr;
}

void UAfterInventoryItemInstance::SetItemDef(TSubclassOf<UAfterInventoryItemDefinition> InDef)
{
    ItemDef = InDef;
}
