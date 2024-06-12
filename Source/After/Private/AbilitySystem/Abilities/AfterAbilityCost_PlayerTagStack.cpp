#include "AbilitySystem/Abilities/AfterAbilityCost_PlayerTagStack.h"

#include "GameFramework/Controller.h"
#include "AbilitySystem/Abilities/AfterGameplayAbility.h"
#include "Player/AfterPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilityCost_PlayerTagStack)

UAfterAbilityCost_PlayerTagStack::UAfterAbilityCost_PlayerTagStack()
{
    Quantity.SetValue(1.0f);
}

bool UAfterAbilityCost_PlayerTagStack::CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRlevantTags) const
{
    if (AController* PC = Ability->GetControllerFromActorInfo())
    {
        if (AAfterPlayerState* PS = Cast<AAfterPlayerState>(PC->PlayerState))
        {
            const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

            const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
            const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

            return PS->GetStatTagStackCount(Tag) >= NumStacks;
        }
    }
    return false;
}

void UAfterAbilityCost_PlayerTagStack::ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
    if (ActorInfo->IsNetAuthority())
    {
        if (AController* PC = Ability->GetControllerFromActorInfo())
        {
            if (AAfterPlayerState* PS = Cast<AAfterPlayerState>(PC->PlayerState))
            {
                const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

                const float NumStacksReal = Quantity.GetValueAtLevel(AbilityLevel);
                const int32 NumStacks = FMath::TruncToInt(NumStacksReal);

                PS->RemoveStatTagStack(Tag, NumStacks);
            }
        }
    }
}
