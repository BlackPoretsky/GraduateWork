#include "Equipment/AfterGA_FromEquipment.h"

#include "Equipment/AfterEquipmentInstance.h"
#include "Inventory/AfterInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGA_FromEquipment)

UAfterGA_FromEquipment::UAfterGA_FromEquipment(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

UAfterEquipmentInstance* UAfterGA_FromEquipment::GetAssociatedEquipment() const
{
    if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
    {
        return Cast<UAfterEquipmentInstance>(Spec->SourceObject.Get());
    }

    return nullptr;
}

UAfterInventoryItemInstance* UAfterGA_FromEquipment::GetAssociatedItem() const
{
    if (UAfterEquipmentInstance* Equipment = GetAssociatedEquipment())
    {
        return Cast<UAfterInventoryItemInstance>(Equipment->GetInstigator());
    }

    return nullptr;
}
