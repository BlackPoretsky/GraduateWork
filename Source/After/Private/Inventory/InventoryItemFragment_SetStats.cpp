#include "Inventory/InventoryItemFragment_SetStats.h"

#include "Inventory/AfterInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemFragment_SetStats)

void UInventoryItemFragment_SetStats::OnInstanceCreated(UAfterInventoryItemInstance* Instance) const
{
    for (const auto& KVP : InitialItemStats)
    {
        Instance->AddStatTagStack(KVP.Key, KVP.Value);
    }
}

int32 UInventoryItemFragment_SetStats::GetItemStatByTag(FGameplayTag Tag) const
{
    if (const int32* StatPtr = InitialItemStats.Find(Tag)) return *StatPtr;

    return 0;
}
