#include "Character/AfterHealthComponent.h"

#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/AfterAttributeSet.h"
#include "AbilitySystem/Attributes/AfterHealthSet.h"

#include "AfterLogChannels.h"
#include "AfterGameplayTags.h"
#include "GameFramework/PlayerState.h"

UAfterHealthComponent::UAfterHealthComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);

    AbilitySystemComponent = nullptr;

    HealthSet = nullptr;
    DeathState = EAfterDeathState::NotDead;
}

void UAfterHealthComponent::InitializeWithAbilitySystem(UAfterAbilitySystemComponent* InASC)
{
    AActor* Owner = GetOwner();
    check(Owner);

    if (AbilitySystemComponent)
    {
        UE_LOG(LogAfter, Error,
            TEXT("AfterHealthComponent: Health component for owner [%s] has already been initialized with an ability system."),
            *GetNameSafe(Owner));
        return;
    }

    AbilitySystemComponent = InASC;
    if (!AbilitySystemComponent)
    {
        UE_LOG(LogAfter, Error, TEXT("AfterHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."),
            *GetNameSafe(Owner));
        return;
    }

    HealthSet = AbilitySystemComponent->GetSet<UAfterHealthSet>();
    if (!HealthSet)
    {
        UE_LOG(LogAfter, Error,
            TEXT("AfterHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."),
            *GetNameSafe(Owner));
        return;
    }

    HealthSet->OnHealthChanged.AddUObject(this, &ThisClass::HandleHealthChanged);
    HealthSet->OnMaxHealthChanged.AddUObject(this, &ThisClass::HandleMaxHealthChanged);
    HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);

    AbilitySystemComponent->SetNumericAttributeBase(UAfterHealthSet::GetHealthAttribute(), HealthSet->GetMaxHealth());

    ClearGameplayTags();

    OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
    OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void UAfterHealthComponent::UninitializeFromAbilitySystem()
{
    ClearGameplayTags();

    if (HealthSet)
    {
        HealthSet->OnHealthChanged.RemoveAll(this);
        HealthSet->OnMaxHealthChanged.RemoveAll(this);
    }

    HealthSet = nullptr;
    AbilitySystemComponent = nullptr;
}

float UAfterHealthComponent::GetHealth() const
{
    return HealthSet ? HealthSet->GetHealth() : 0.0f;
}

float UAfterHealthComponent::GetMaxHealth() const
{
    return HealthSet ? HealthSet->GetMaxHealth() : 0.0f;
}

float UAfterHealthComponent::GetHealthNormalized() const
{
    if (!HealthSet) return 0.0f;

    const float Healthe = HealthSet->GetHealth();
    const float MaxHealthe = HealthSet->GetMaxHealth();

    return (MaxHealthe > 0.0f) ? (Healthe / MaxHealthe) : 0.0f;
}

void UAfterHealthComponent::StartDeath()
{
    if (DeathState != EAfterDeathState::NotDead)
    {
        return;
    }

    DeathState = EAfterDeathState::DeathStarted;

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->SetLooseGameplayTagCount(AfterGameplayTags::Status_Death_Dying, 1);
    }

    AActor* Owner = GetOwner();
    check(Owner);

    OnDeathStarted.Broadcast(Owner);
}

void UAfterHealthComponent::FinishDeath()
{
    if (DeathState != EAfterDeathState::DeathStarted)
    {
        return;
    }

    DeathState = EAfterDeathState::DeathFinished;

    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->SetLooseGameplayTagCount(AfterGameplayTags::Status_Death_Dead, 1);
    }

    AActor* Owner = GetOwner();
    check(Owner);

    OnDeathFinished.Broadcast(Owner);
}

void UAfterHealthComponent::OnUnregister()
{
    UninitializeFromAbilitySystem();

    Super::OnUnregister();
}

void UAfterHealthComponent::ClearGameplayTags()
{
    if (AbilitySystemComponent)
    {
        AbilitySystemComponent->SetLooseGameplayTagCount(AfterGameplayTags::Status_Death_Dying, 0);
        AbilitySystemComponent->SetLooseGameplayTagCount(AfterGameplayTags::Status_Death_Dead, 0);
    }
}

void UAfterHealthComponent::HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
    float DamageMagnitude, float OldValue, float NewValue)
{
    OnHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UAfterHealthComponent::HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser,
    const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue)
{
    OnMaxHealthChanged.Broadcast(this, OldValue, NewValue, DamageInstigator);
}

void UAfterHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
    float DamageMagnitude, float OldValue, float NewValue)
{
    if (AbilitySystemComponent && DamageEffectSpec)
    {
        {
            FGameplayEventData Payload;
            Payload.EventTag = AfterGameplayTags::GameplayEvent_Death;
            Payload.Instigator = DamageInstigator;
            Payload.Target = AbilitySystemComponent->GetAvatarActor();
            Payload.OptionalObject = DamageEffectSpec->Def;
            Payload.ContextHandle = DamageEffectSpec->GetEffectContext();
            Payload.InstigatorTags = *DamageEffectSpec->CapturedSourceTags.GetAggregatedTags();
            Payload.TargetTags = *DamageEffectSpec->CapturedTargetTags.GetAggregatedTags();
            Payload.EventMagnitude = DamageMagnitude;

            AbilitySystemComponent->HandleGameplayEvent(Payload.EventTag, &Payload);
        }
    }
}