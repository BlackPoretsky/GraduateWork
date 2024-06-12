#include "AbilitySystem/Phases/AfterGamePhaseSubsystem.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"
#include "Phases/AfterGamePhaseAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGamePhaseSubsystem)

DEFINE_LOG_CATEGORY_STATIC(LogAfterGamePhase, All, All)

UAfterGamePhaseSubsystem::UAfterGamePhaseSubsystem() {}

void UAfterGamePhaseSubsystem::PostInitialize()
{
    Super::PostInitialize();
}

bool UAfterGamePhaseSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
    if (Super::ShouldCreateSubsystem(Outer)) return true;

    return false;
}

void UAfterGamePhaseSubsystem::StartPhase(TSubclassOf<UAfterGamePhaseAbility> PhaseAbility, FAfterGamePhaseDelegate PhaseEndedCallback)
{
    UWorld* World = GetWorld();
    UAfterAbilitySystemComponent* GameState_ASC = World->GetGameState()->FindComponentByClass<UAfterAbilitySystemComponent>();
    if (ensure(GameState_ASC))
    {
        FGameplayAbilitySpec PhaseSpec(PhaseAbility, 1, 0, this);
        FGameplayAbilitySpecHandle SpecHandle = GameState_ASC->GiveAbilityAndActivateOnce(PhaseSpec);
        FGameplayAbilitySpec* FoundSpec = GameState_ASC->FindAbilitySpecFromHandle(SpecHandle);

        if (FoundSpec && FoundSpec->IsActive())
        {
            FAfterGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(SpecHandle);
            Entry.PhaseEndedCallback = PhaseEndedCallback;
        }
        else
        {
            PhaseEndedCallback.ExecuteIfBound(nullptr);
        }
    }
}

void UAfterGamePhaseSubsystem::WhenPhaseStartsOrIsActive(
    FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FAfterGamePhaseTagDelegate& WhenPhaseActive)
{
    FPhaseObserver Observer;
    Observer.PhaseTag = PhaseTag;
    Observer.MatchType = MatchType;
    Observer.PhaseCallback = WhenPhaseActive;
    PhaseStartObservers.Add(Observer);

    if (IsPhaseActive(PhaseTag))
    {
        WhenPhaseActive.ExecuteIfBound(PhaseTag);
    }
}

void UAfterGamePhaseSubsystem::WhenPhaseEnds(
    FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FAfterGamePhaseTagDelegate& WhenPhaseEnd)
{
    FPhaseObserver Observer;
    Observer.PhaseTag = PhaseTag;
    Observer.MatchType = MatchType;
    Observer.PhaseCallback = WhenPhaseEnd;
    PhaseEndObservers.Add(Observer);
}

bool UAfterGamePhaseSubsystem::IsPhaseActive(const FGameplayTag& PhaseTag) const
{
    for (const auto& KVP : ActivePhaseMap)
    {
        const FAfterGamePhaseEntry& PhaseEntry = KVP.Value;
        if (PhaseEntry.PhaseTag.MatchesTag(PhaseTag))
        {
            return true;
        }
    }

    return false;
}

bool UAfterGamePhaseSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

void UAfterGamePhaseSubsystem::K2_StartPhase(
    TSubclassOf<UAfterGamePhaseAbility> PhaseAbility, const FAfterGamePhaseDynamicDelegate& PhaseEndedDelegate)
{
    const FAfterGamePhaseDelegate EndedDelegate =
        FAfterGamePhaseDelegate::CreateWeakLambda(const_cast<UObject*>(PhaseEndedDelegate.GetUObject()),
            [PhaseEndedDelegate](const UAfterGamePhaseAbility* PhaseAbility) { PhaseEndedDelegate.ExecuteIfBound(PhaseAbility); });

    StartPhase(PhaseAbility, EndedDelegate);
}

void UAfterGamePhaseSubsystem::K2_WhenPhaseStartsOrISActive(
    FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FAfterGamePhaseTagDynamicDelegate WhenPhaseActive)
{
    const FAfterGamePhaseTagDelegate ActiveDelegate = FAfterGamePhaseTagDelegate::CreateWeakLambda(
        WhenPhaseActive.GetUObject(), [WhenPhaseActive](const FGameplayTag& PhaseTag) { WhenPhaseActive.ExecuteIfBound(PhaseTag); });

    WhenPhaseStartsOrIsActive(PhaseTag, MatchType, ActiveDelegate);
}

void UAfterGamePhaseSubsystem::K2_WhenPhaseEnds(
    FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FAfterGamePhaseTagDynamicDelegate WhenPhaseEnd)
{
    const FAfterGamePhaseTagDelegate EndedDelegate = FAfterGamePhaseTagDelegate::CreateWeakLambda(
        WhenPhaseEnd.GetUObject(), [WhenPhaseEnd](const FGameplayTag& PhaseTag) { WhenPhaseEnd.ExecuteIfBound(PhaseTag); });

    WhenPhaseEnds(PhaseTag, MatchType, EndedDelegate);
}

void UAfterGamePhaseSubsystem::OnBeginPhase(const UAfterGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
    const FGameplayTag IncomingPhaseTag = PhaseAbility->GetGamePhaseTag();

    UE_LOG(LogAfterGamePhase, Log, TEXT("Beginning Phase '%s' (%s)"), *IncomingPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

    const UWorld* World = GetWorld();
    UAfterAbilitySystemComponent* GameState_ASC = World->GetGameState()->FindComponentByClass<UAfterAbilitySystemComponent>();
    if (ensure(GameState_ASC))
    {
        TArray<FGameplayAbilitySpec*> ActivePhases;
        for (const auto& KVP : ActivePhaseMap)
        {
            const FGameplayAbilitySpecHandle ActiveAbilityHandle = KVP.Key;
            if (FGameplayAbilitySpec* Spec = GameState_ASC->FindAbilitySpecFromHandle(ActiveAbilityHandle))
            {
                ActivePhases.Add(Spec);
            }
        }

        for (const FGameplayAbilitySpec* ActivePhase : ActivePhases)
        {
            const UAfterGamePhaseAbility* ActivePhaseAbility = CastChecked<UAfterGamePhaseAbility>(ActivePhase->Ability);
            const FGameplayTag ActivePhaseTag = ActivePhaseAbility->GetGamePhaseTag();

            if (!IncomingPhaseTag.MatchesTag(ActivePhaseTag))
            {
                UE_LOG(
                    LogAfterGamePhase, Log, TEXT("\tEnding Phase '%s' (%s)"), *ActivePhaseTag.ToString(), *GetNameSafe(ActivePhaseAbility));

                FGameplayAbilitySpecHandle HandleToEnd = ActivePhase->Handle;
                GameState_ASC->CancelAbilitiesByFunc([HandleToEnd](const UAfterGameplayAbility* LyraAbility,
                                                         FGameplayAbilitySpecHandle Handle) { return Handle == HandleToEnd; },
                    true);
            }
        }

        FAfterGamePhaseEntry& Entry = ActivePhaseMap.FindOrAdd(PhaseAbilityHandle);
        Entry.PhaseTag = IncomingPhaseTag;

        // Notify all observers of this phase that it has started.
        for (const FPhaseObserver& Observer : PhaseStartObservers)
        {
            if (Observer.IsMatch(IncomingPhaseTag))
            {
                Observer.PhaseCallback.ExecuteIfBound(IncomingPhaseTag);
            }
        }
    }
}

void UAfterGamePhaseSubsystem::OnEndPhase(const UAfterGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle)
{
    const FGameplayTag EndedPhaseTag = PhaseAbility->GetGamePhaseTag();
    UE_LOG(LogAfterGamePhase, Log, TEXT("Ended Phase '%s' (%s)"), *EndedPhaseTag.ToString(), *GetNameSafe(PhaseAbility));

    const FAfterGamePhaseEntry& Entry = ActivePhaseMap.FindChecked(PhaseAbilityHandle);
    Entry.PhaseEndedCallback.ExecuteIfBound(PhaseAbility);

    ActivePhaseMap.Remove(PhaseAbilityHandle);

    for (const FPhaseObserver& Observer : PhaseEndObservers)
    {
        if (Observer.IsMatch(EndedPhaseTag))
        {
            Observer.PhaseCallback.ExecuteIfBound(EndedPhaseTag);
        }
    }
}

bool UAfterGamePhaseSubsystem::FPhaseObserver::IsMatch(const FGameplayTag& ComparePhaseTag) const
{
    switch (MatchType)
    {
        case EPhaseTagMatchType::ExactMatch:
            return ComparePhaseTag == PhaseTag;
        case EPhaseTagMatchType::PartialMatch:
            return ComparePhaseTag.MatchesTag(PhaseTag);
    }

    return false;
}
