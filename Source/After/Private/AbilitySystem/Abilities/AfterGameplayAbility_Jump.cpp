#include "AbilitySystem/Abilities/AfterGameplayAbility_Jump.h"

#include "Character/AfterCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameplayAbility_Jump)

UAfterGameplayAbility_Jump::UAfterGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UAfterGameplayAbility_Jump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTag, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!ActorInfo || !ActorInfo->AvatarActor.IsValid()) return false;

    const AAfterCharacter* AfterCharacter = Cast<AAfterCharacter>(ActorInfo->AvatarActor.Get());
    if (!AfterCharacter || !AfterCharacter->CanJump()) return false;

    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTag, TargetTags, OptionalRelevantTags)) return false;

    return true;
}

void UAfterGameplayAbility_Jump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    CharacterJumpStop();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAfterGameplayAbility_Jump::CharacterJumpStart()
{
    if (AAfterCharacter* AfterCharacter = GetAfterCharacterFromActorInfo())
    {
        if (AfterCharacter->IsLocallyControlled() && !AfterCharacter->bPressedJump)
        {
            AfterCharacter->UnCrouch();
            AfterCharacter->Jump();
        }
    }
}

void UAfterGameplayAbility_Jump::CharacterJumpStop()
{
    if (AAfterCharacter* AfterCharacter = GetAfterCharacterFromActorInfo())
    {
        if (AfterCharacter->IsLocallyControlled() && AfterCharacter->bPressedJump)
        {
            AfterCharacter->StopJumping();
        }
    }
}
