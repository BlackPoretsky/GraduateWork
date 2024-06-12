#include "Player/AfterPlayerState.h"

#include "AbilitySystem/Attributes/AfterCombatSet.h"
#include "AbilitySystem/Attributes/AfterHealthSet.h"
#include "AbilitySystem/AfterAbilitySet.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"

#include "Character/AfterPawnData.h"
#include "Character/AfterPawnExtensionComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Player/AfterPlayerController.h"
#include "AfterLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterPlayerState)

const FName AAfterPlayerState::NAME_AfterAbilityReady("AfterAbilitiesReady");

AAfterPlayerState::AAfterPlayerState(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<UAfterAbilitySystemComponent>(this, "AbilitySystemComponent");

    HealthSet = CreateDefaultSubobject<UAfterHealthSet>("HealthSet");
    CombatSet = CreateDefaultSubobject<UAfterCombatSet>("CombatSet");
}

AAfterPlayerController* AAfterPlayerState::GetAfterPlayerController() const
{
    return Cast<AAfterPlayerController>(GetOwner());
}

UAbilitySystemComponent* AAfterPlayerState::GetAbilitySystemComponent() const
{
    return GetAfterAbilitySystemComponent();
}

void AAfterPlayerState::SetPawnData(const UAfterPawnData* InPawnData)
{
    check(InPawnData);

    if (GetLocalRole() != ROLE_Authority) return;

    if (PawnData)
    {
        UE_LOG(LogAfter, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."),
            *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
        return;
    }

    PawnData = InPawnData;

    for (const UAfterAbilitySet* AbilitySet : PawnData->AbilitySets)
    {
        if (AbilitySet)
        {
            AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
        }
    }

    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_AfterAbilityReady);
}

void AAfterPlayerState::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AAfterPlayerState::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    check(AbilitySystemComponent);
    AbilitySystemComponent->InitAbilityActorInfo(this, GetPawn());
}

void AAfterPlayerState::Reset()
{
    Super::Reset();
}

void AAfterPlayerState::ClientInitialize(AController* C)
{
    Super::ClientInitialize(C);

    if (UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(GetPawn()))
    {
        PawnExtComp->CheckDefaultInitialization();
    }
}

void AAfterPlayerState::CopyProperties(APlayerState* PlayerState)
{
    Super::CopyProperties(PlayerState);
}

void AAfterPlayerState::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
    StatTags.AddStack(Tag, StackCount);
}

void AAfterPlayerState::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
    StatTags.RemoveStack(Tag, StackCount);
}

int32 AAfterPlayerState::GetStatTagStackCount(FGameplayTag Tag) const
{
    return StatTags.GetStackCount(Tag);
}

bool AAfterPlayerState::HasStatTag(FGameplayTag Tag) const
{
    return StatTags.ContainsTag(Tag);
}
