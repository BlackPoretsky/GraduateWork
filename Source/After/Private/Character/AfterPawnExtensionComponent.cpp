#include "Character/AfterPawnExtensionComponent.h"

#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "Player/AfterPlayerState.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"

#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "AfterGameplayTags.h"
#include "AfterLogChannels.h"
#include "AfterPawnData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterPawnExtensionComponent)

const FName UAfterPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

UAfterPawnExtensionComponent::UAfterPawnExtensionComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bStartWithTickEnabled = false;
    PrimaryComponentTick.bCanEverTick = false;

    SetIsReplicatedByDefault(true);

    PawnData = nullptr;
    AbilitySystemComponent = nullptr;
}

bool UAfterPawnExtensionComponent::CanChangeInitState(
    UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    check(Manager);

    APawn* Pawn = GetPawn<APawn>();
    if (!CurrentState.IsValid() && DesiredState == AfterGameplayTags::InitState_Spawned)
    {
        if (Pawn)
        {
            return true;
        }
    }
    if (CurrentState == AfterGameplayTags::InitState_Spawned && DesiredState == AfterGameplayTags::InitState_DataAvailable)
    {
        if (!PawnData)
        {
            return false;
        }

        return true;
    }
    else if (CurrentState == AfterGameplayTags::InitState_DataAvailable && DesiredState == AfterGameplayTags::InitState_DataInitialized)
    {
        return Manager->HaveAllFeaturesReachedInitState(Pawn, AfterGameplayTags::InitState_DataAvailable);
    }
    else if (CurrentState == AfterGameplayTags::InitState_DataInitialized && DesiredState == AfterGameplayTags::InitState_GameplayReady)
    {
        return true;
    }

    return false;
}

void UAfterPawnExtensionComponent::HandleChangeInitState(
    UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    if (DesiredState == AfterGameplayTags::InitState_DataInitialized)
    {
    }
}

void UAfterPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    if (Params.FeatureName != NAME_ActorFeatureName)
    {
        if (Params.FeatureState == AfterGameplayTags::InitState_DataAvailable)
        {
            CheckDefaultInitialization();
        }
    }
}

void UAfterPawnExtensionComponent::CheckDefaultInitialization()
{
    CheckDefaultInitializationForImplementers();

    static const TArray<FGameplayTag> StateChain = {AfterGameplayTags::InitState_Spawned, AfterGameplayTags::InitState_DataAvailable,
        AfterGameplayTags::InitState_DataInitialized, AfterGameplayTags::InitState_GameplayReady};

    ContinueInitStateChain(StateChain);
}

void UAfterPawnExtensionComponent::SetPawnData(const UAfterPawnData* InPawnData)
{
    check(InPawnData);

    APawn* Pawn = GetPawnChecked<APawn>();

    if (Pawn->GetLocalRole() != ROLE_Authority)
    {
        return;
    }

    if (PawnData)
    {
        UE_LOG(LogAfter, Error, TEXT("Trying to set PawnData [%s] on pawn [%s] that already has valid PawnData [%s]."),
            *GetNameSafe(InPawnData), *GetNameSafe(Pawn), *GetNameSafe(PawnData));
        return;
    }

    PawnData = InPawnData;

    CheckDefaultInitialization();
}

void UAfterPawnExtensionComponent::InitializeAbilitySystem(UAfterAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
    check(InASC);
    check(InOwnerActor);

    if (AbilitySystemComponent == InASC)
    {
        return;
    }

    if (AbilitySystemComponent)
    {
        UninitializeAbilitySystem();
    }

    APawn* Pawn = GetPawnChecked<APawn>();
    AActor* ExistingAvatar = InASC->GetAvatarActor();

    UE_LOG(LogAfter, Verbose, TEXT("Setting up ASC [%s] on pawn [%s] owner [%s], existing [%s] "), *GetNameSafe(InASC), *GetNameSafe(Pawn),
        *GetNameSafe(InOwnerActor), *GetNameSafe(ExistingAvatar));

    if ((ExistingAvatar != nullptr) && (ExistingAvatar != Pawn))
    {
        UE_LOG(LogAfter, Log, TEXT("Existing avatar (authority=%d)"), ExistingAvatar->HasAuthority() ? 1 : 0);

        ensure(!ExistingAvatar->HasAuthority());

        if (UAfterPawnExtensionComponent* OtherExtensionComponent = FindPawnExtensionComponent(ExistingAvatar))
        {
            OtherExtensionComponent->UninitializeAbilitySystem();
        }
    }

    AbilitySystemComponent = InASC;
    AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);

    if (ensure(PawnData))
    {
        InASC->SetTagRelationshipMapping(PawnData->TagRelationshipMapping);
        AAfterPlayerState* AfterPS = GetPlayerState<AAfterPlayerState>();
        AfterPS->SetPawnData(PawnData);
    }

    OnAbilitySystemInitialized.Broadcast();
}

void UAfterPawnExtensionComponent::UninitializeAbilitySystem()
{
    if (!AbilitySystemComponent)
    {
        return;
    }

    if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
    {
        FGameplayTagContainer AbilityTypesToIgnore;
        AbilityTypesToIgnore.AddTag(AfterGameplayTags::Ability_Behavior_SurvivesDeath);

        AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
        AbilitySystemComponent->ClearAbilityInput();
        AbilitySystemComponent->RemoveAllGameplayCues();

        if (AbilitySystemComponent->GetOwnerActor() != nullptr)
        {
            AbilitySystemComponent->SetAvatarActor(nullptr);
        }
        else
        {
            AbilitySystemComponent->ClearActorInfo();
        }

        OnAbilitySystemUninitialized.Broadcast();
    }

    AbilitySystemComponent = nullptr;
}

void UAfterPawnExtensionComponent::HandleControllerChanged()
{
    if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
    {
        ensure(AbilitySystemComponent->AbilityActorInfo->OwnerActor == AbilitySystemComponent->GetOwnerActor());
        if (AbilitySystemComponent->GetOwnerActor() == nullptr)
        {
            UninitializeAbilitySystem();
        }
        else
        {
            AbilitySystemComponent->RefreshAbilityActorInfo();
        }
    }

    CheckDefaultInitialization();
}

void UAfterPawnExtensionComponent::HandlePlayerStateReplicated()
{
    CheckDefaultInitialization();
}

void UAfterPawnExtensionComponent::SetupPlayerInputComponent()
{
    CheckDefaultInitialization();
}

void UAfterPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
    if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
    {
        OnAbilitySystemInitialized.Add(Delegate);
    }

    if (AbilitySystemComponent)
    {
        Delegate.Execute();
    }
}

void UAfterPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
    if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
    {
        OnAbilitySystemUninitialized.Add(Delegate);
    }
}

void UAfterPawnExtensionComponent::OnRegister()
{
    Super::OnRegister();

    const APawn* Pawn = GetPawn<APawn>();
    ensureAlwaysMsgf(
        (Pawn != nullptr), TEXT("AfterPawnExtensionComponent on [%s] can only be added to Pawn actors."), *GetNameSafe(GetOwner()));

    TArray<UActorComponent*> PawnExtensionComponents;
    Pawn->GetComponents(UAfterPawnExtensionComponent::StaticClass(), PawnExtensionComponents);
    ensureAlwaysMsgf(
        (PawnExtensionComponents.Num() == 1), TEXT("Only one AfterPawnExtensionComponent should exist on [%s]."), *GetNameSafe(GetOwner()));

    RegisterInitStateFeature();
}

void UAfterPawnExtensionComponent::BeginPlay()
{
    Super::BeginPlay();

    BindOnActorInitStateChanged(NAME_None, FGameplayTag(), false);

    ensure(TryToChangeInitState(AfterGameplayTags::InitState_Spawned));
    CheckDefaultInitialization();
}

void UAfterPawnExtensionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UninitializeAbilitySystem();
    UnregisterInitStateFeature();

    Super::EndPlay(EndPlayReason);
}
