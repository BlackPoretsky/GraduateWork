#include "AbilitySystem/Abilities/AfterGameplayAbility_Death.h"

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "Character/AfterHealthComponent.h"
#include "AfterGameplayTags.h"
#include "AfterLogChannels.h"
#include "Trace/Trace.inl"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameplayAbility_Death)

UAfterGameplayAbility_Death::UAfterGameplayAbility_Death(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

    bAutoStartDeath = true;

    if (HasAnyFlags(RF_ClassDefaultObject))
    {
        FAbilityTriggerData TriggerData;
        TriggerData.TriggerTag = AfterGameplayTags::GameplayEvent_Death;
        TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
        AbilityTriggers.Add(TriggerData);
    }
}

void UAfterGameplayAbility_Death::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    check(ActorInfo);

    UAfterAbilitySystemComponent* AfterASC = CastChecked<UAfterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());

    FGameplayTagContainer AbilityTypesToIgnore;
    AbilityTypesToIgnore.AddTag(AfterGameplayTags::Ability_Behavior_SurvivesDeath);

    AfterASC->CancelAbilities(nullptr, &AbilityTypesToIgnore, this);

    SetCanBeCanceled(false);

    if (!ChangeActivationGroup(EAfterAbilityActivationGroup::Exclusive_Blocking))
    {
        UE_LOG(LogAfterAbilitySystem, Error,
            TEXT("UAfterGameplayAbility_Death::ActivateAbility: Ability [%s] failed to change activation group to blocking."), *GetName());
    }

    if (bAutoStartDeath)
    {
        StartDeath();
    }

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAfterGameplayAbility_Death::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    check(ActorInfo);

    FinishDeath();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAfterGameplayAbility_Death::StartDeath()
{
    if (UAfterHealthComponent* HealthComponent = UAfterHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
    {
        if (HealthComponent->GetDeathState() == EAfterDeathState::NotDead)
        {
            HealthComponent->StartDeath();
        }
    }
}

void UAfterGameplayAbility_Death::FinishDeath()
{
    if (UAfterHealthComponent* HealthComponent = UAfterHealthComponent::FindHealthComponent(GetAvatarActorFromActorInfo()))
    {
        if (HealthComponent->GetDeathState() == EAfterDeathState::DeathStarted)
        {
            HealthComponent->FinishDeath();
        }
    }
}
