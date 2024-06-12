#include "AbilitySystem/Phases/AfterGamePhaseAbility.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "AbilitySystem/Phases/AfterGamePhaseSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGamePhaseAbility)

UAfterGamePhaseAbility::UAfterGamePhaseAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ServerOnly;
}

EDataValidationResult UAfterGamePhaseAbility::IsDataValid(FDataValidationContext& Context) const
{
    return EDataValidationResult();
}

void UAfterGamePhaseAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (ActorInfo->IsNetAuthority())
    {
        UWorld* World = ActorInfo->AbilitySystemComponent->GetWorld();
        UAfterGamePhaseSubsystem* PhaseSubsystem = UWorld::GetSubsystem<UAfterGamePhaseSubsystem>(World);
        PhaseSubsystem->OnBeginPhase(this, Handle);
    }

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAfterGamePhaseAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (ActorInfo->IsNetAuthority())
    {
        UWorld* World = ActorInfo->AbilitySystemComponent->GetWorld();
        UAfterGamePhaseSubsystem* PhaseSubsystem = UWorld::GetSubsystem<UAfterGamePhaseSubsystem>(World);
        PhaseSubsystem->OnBeginPhase(this, Handle);
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
