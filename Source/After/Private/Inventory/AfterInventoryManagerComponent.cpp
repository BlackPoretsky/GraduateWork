#include "Inventory/AfterInventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "Engine/World.h"
#include "Inventory/AfterInventoryItemDefinition.h"
#include "Inventory/AfterInventoryItemInstance.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterInventoryManagerComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_After_Inventory_Message_StackChanged, "After.Inventory.Message.StackChanged")

/////////////////////////
// FAfterInventoryList //
/////////////////////////
TArray<UAfterInventoryItemInstance*> FAfterInventoryList::GetAllItems() const
{
    TArray<UAfterInventoryItemInstance*> Results;
    Results.Reserve(Entries.Num());
    for (UAfterInventoryItemInstance* Entry : Entries)
    {
        if (Entry != nullptr)
        {
            Results.Add(Entry);
        }
    }

    return Results;
}

UAfterInventoryItemInstance* FAfterInventoryList::AddEntry(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount)
{
    check(ItemDef != nullptr);
    check(OwnerComponent);

    const UAfterInventoryItemDefinition* DefaultItemDef = GetDefault<UAfterInventoryItemDefinition>(ItemDef);

    UAfterInventoryItemInstance* Result = Entries.AddDefaulted_GetRef();
    Result = NewObject<UAfterInventoryItemInstance>(OwnerComponent->GetOwner());
    Result->SetItemDef(ItemDef);
    Result->SetItemDisplayName(DefaultItemDef->GetItemDisplayName());

    for (UAfterInventoryItemFragment* Fragment : DefaultItemDef->Fragments)
    {
        if (Fragment != nullptr)
        {
            Fragment->OnInstanceCreated(Result);
        }
    }

    return Result;
}

void FAfterInventoryList::AddEntry(UAfterInventoryItemInstance* Instance)
{
    unimplemented();
}

void FAfterInventoryList::RemoveEntry(UAfterInventoryItemInstance* Instance)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        UAfterInventoryItemInstance* Entry = *EntryIt;
        if (Entry == Instance)
        {
            EntryIt.RemoveCurrent();
        }
    }
}

/////////////////////////////////////
// UAfterInventoryManagerComponent //
/////////////////////////////////////
UAfterInventoryManagerComponent::UAfterInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
      InventoryList(this)
{
}

bool UAfterInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount)
{
    return true;
}

UAfterInventoryItemInstance* UAfterInventoryManagerComponent::AddItemDefinition(
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 StackCount)
{
    UAfterInventoryItemInstance* Result = nullptr;
    if (ItemDef != nullptr)
    {
        Result = InventoryList.AddEntry(ItemDef, StackCount);

        if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && Result)
        {
            AddReplicatedSubObject(Result);
        }
    }
    return Result;
}

void UAfterInventoryManagerComponent::AddItemInstance(UAfterInventoryItemInstance* ItemInstance)
{
    InventoryList.AddEntry(ItemInstance);
}

void UAfterInventoryManagerComponent::RemoveItemInstance(UAfterInventoryItemInstance* ItemInstance)
{
    InventoryList.RemoveEntry(ItemInstance);
}

TArray<UAfterInventoryItemInstance*> UAfterInventoryManagerComponent::GetAllItems() const
{
    return InventoryList.GetAllItems();
}

UAfterInventoryItemInstance* UAfterInventoryManagerComponent::FindFirstItemStackByDefinition(
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef) const
{
    for (UAfterInventoryItemInstance* Entry : InventoryList.Entries)
    {

        if (IsValid(Entry))
        {
            if (Entry->GetItemDef() == ItemDef)
            {
                return Entry;
            }
        }
    }

    return nullptr;
}

int32 UAfterInventoryManagerComponent::GetTotatlItemCountByDefinition(
    TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
    int32 TotalCount = 0;
    for (const UAfterInventoryItemInstance* Entry : InventoryList.Entries)
    {
        if (IsValid(Entry))
        {
            if (Entry->GetItemDef() == ItemDef)
            {
                ++TotalCount;
            }
        }
    }

    return TotalCount;
}

bool UAfterInventoryManagerComponent::ConsumeItemByDefinition(TSubclassOf<UAfterInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
    AActor* OwningActor = GetOwner();
    if (!OwningActor || !OwningActor->HasAuthority()) return false;

    int32 TotalConsumed = 0;
    while (TotalConsumed < NumToConsume)
    {
        if (UAfterInventoryItemInstance* Instance = UAfterInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
        {
            InventoryList.RemoveEntry(Instance);
            ++TotalConsumed;
        }
        else
        {
            return false;
        }
    }

    return TotalConsumed == NumToConsume;
}