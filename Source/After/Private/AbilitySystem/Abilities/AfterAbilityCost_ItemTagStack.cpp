#include "AbilitySystem/Abilities/AfterAbilityCost_ItemTagStack.h"
#include "Equipment/AfterGA_FromEquipment.h"
#include "Inventory/AfterInventoryItemInstance.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilityCost_ItemTagStack)

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_FALL_COST, "Ability.ActivateFail.Cost")

UAfterAbilityCost_ItemTagStack::UAfterAbilityCost_ItemTagStack()
{
    Quantity.SetValue(1.0f);
    FailureTag = TAG_ABILITY_FALL_COST;
}

bool UAfterAbilityCost_ItemTagStack::CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (const UAfterGA_FromEquipment* EquipmentAbility = Cast<const UAfterGA_FromEquipment>(Ability))
    {
        if (UAfterInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
        {
            const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

            const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
            const int32 NumStacks = FMath::TruncToInt(NumStacksReal);
            const bool bCanApplyCost = ItemInstance->GetStatTagStackCount(Tag) >= NumStacks;

            if (!bCanApplyCost && OptionalRelevantTags && FailureTag.IsValid())
            {
                OptionalRelevantTags->AddTag(FailureTag);
            }
            return bCanApplyCost;
        }
    }
    return false;
}

void UAfterAbilityCost_ItemTagStack::ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (ActorInfo->IsNetAuthority())
    {
        if (const UAfterGA_FromEquipment* EquipmentAbility = Cast<const UAfterGA_FromEquipment>(Ability))
        {
            if (UAfterInventoryItemInstance* ItemInstance = EquipmentAbility->GetAssociatedItem())
            {
                const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

                const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
                const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

                ItemInstance->RemoveStatTagStack(Tag, NumStacks);
            }
        }
    }
}
