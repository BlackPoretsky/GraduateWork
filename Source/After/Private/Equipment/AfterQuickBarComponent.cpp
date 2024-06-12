#include "Equipment/AfterQuickBarComponent.h"

#include "Equipment/AfterEquipmentDefinition.h"
#include "Equipment/AfterEquipmentInstance.h"
#include "Equipment/AfterEquipmentManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "NativeGameplayTags.h"

#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterQuickBarComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_After_QuickBar_Message_SlotsChanged, "After.QuickBar.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_After_QuickBar_Message_ActiveIndexChanged, "After.QuickBar.Message.ActiveIndexChanged");

UAfterQuickBarComponent::UAfterQuickBarComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterQuickBarComponent::BeginPlay()
{
    if (Slots.Num() < NumSlots)
    {
        Slots.AddDefaulted(NumSlots - Slots.Num());
    }

    Super::BeginPlay();
}

void UAfterQuickBarComponent::CycleActiveSlotForward()
{
    if (Slots.Num() < 2) return;

    const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
    int32 NewIndex = ActiveSlotIndex;
    do
    {
        NewIndex = (NewIndex + 1) % Slots.Num();
        if (Slots[NewIndex] != nullptr)
        {
            SetActiveSlotIndex(NewIndex);
            return;
        }
    } while (NewIndex != OldIndex);
}

void UAfterQuickBarComponent::CycleActiveSlotBackward()
{
    if (Slots.Num() < 2) return;

    const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
    int32 NextIndex = ActiveSlotIndex;
    do
    {
        NextIndex = (NextIndex - 1 + Slots.Num()) % Slots.Num();
        if (Slots[NextIndex] != nullptr)
        {
            SetActiveSlotIndex(NextIndex);
            return;
        }
    } while (NextIndex != OldIndex);
}

void UAfterQuickBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
    if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
    {
        UnequipItemInSlot();

        ActiveSlotIndex = NewIndex;

        EquipItemInSlot();

        OnRep_ActiveSlotIndex();
    }
}

UAfterInventoryItemInstance* UAfterQuickBarComponent::GetActiveSlotItem() const
{
    return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UAfterQuickBarComponent::GetNextFreeItemSlot() const
{
    int32 SlotIndex = 0;
    for (TObjectPtr<UAfterInventoryItemInstance> ItemPtr : Slots)
    {
        if (ItemPtr == nullptr)
        {
            return SlotIndex;
        }

        ++SlotIndex;
    }

    return INDEX_NONE;
}

void UAfterQuickBarComponent::AddItemToSlot(int32 SlotIndex, UAfterInventoryItemInstance* Item)
{
    if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
    {
        if (Slots[SlotIndex] == nullptr)
        {
            Slots[SlotIndex] = Item;
        }
    }
}

UAfterInventoryItemInstance* UAfterQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
    UAfterInventoryItemInstance* Result = nullptr;

    if (ActiveSlotIndex == SlotIndex)
    {
        UnequipItemInSlot();
        ActiveSlotIndex = -1;
    }

    if (Slots.IsValidIndex(SlotIndex))
    {
        Result = Slots[SlotIndex];

        if (Result != nullptr)
        {
            Slots[SlotIndex] = nullptr;
        }
    }

    return Result;
}

void UAfterQuickBarComponent::EquipItemInSlot()
{
    check(Slots.IsValidIndex(ActiveSlotIndex));
    check(EquippedItem == nullptr);

    if (UAfterInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
    {
        if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
        {
            TSubclassOf<UAfterEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
            if (EquipDef != nullptr)
            {
                if (UAfterEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
                {
                    EquippedItem = EquipmentManager->EquipItem(EquipDef);
                    if (EquippedItem != nullptr)
                    {
                        EquippedItem->SetInstigator(SlotItem);
                    }
                }
            }
        }
    }
}

void UAfterQuickBarComponent::UnequipItemInSlot()
{
    if (UAfterEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
    {
        if (EquippedItem != nullptr)
        {
            EquipmentManager->UnequipItem(EquippedItem);
            EquippedItem = nullptr;
        }
    }
}

UAfterEquipmentManagerComponent* UAfterQuickBarComponent::FindEquipmentManager() const
{
    if (AController* OwnerController = Cast<AController>(GetOwner()))
    {
        if (APawn* Pawn = OwnerController->GetPawn())
        {
            return Pawn->FindComponentByClass<UAfterEquipmentManagerComponent>();
        }
    }

    return nullptr;
}

void UAfterQuickBarComponent::OnRep_ActiveSlotIndex()
{
    FAfterQuickBarActiveIndexChangedMessage Message;
    Message.Owner = GetOwner();
    Message.ActiveIndex = ActiveSlotIndex;

    UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
    MessageSystem.BroadcastMessage(TAG_After_QuickBar_Message_ActiveIndexChanged, Message);
}