#include "AbilitySystem/Abilities/AfterAbilityCost_InventoryItem.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilityCost_InventoryItem)

UAfterAbilityCost_InventoryItem::UAfterAbilityCost_InventoryItem()
{
    Quantity.SetValue(1.0f);
}

bool UAfterAbilityCost_InventoryItem::CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
    return false;
}

void UAfterAbilityCost_InventoryItem::ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
}
