#pragma once

#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"

#include "AfterGamePhaseSubsystem.generated.h"

class UAfterGamePhaseAbility;
class UObject;
struct FGameplayAbilitySpecHandle;

DECLARE_DYNAMIC_DELEGATE_OneParam(FAfterGamePhaseDynamicDelegate, const UAfterGamePhaseAbility*, Phase);
DECLARE_DELEGATE_OneParam(FAfterGamePhaseDelegate, const UAfterGamePhaseAbility* Phase);

DECLARE_DYNAMIC_DELEGATE_OneParam(FAfterGamePhaseTagDynamicDelegate, const FGameplayTag&, PhaseTag);
DECLARE_DELEGATE_OneParam(FAfterGamePhaseTagDelegate, const FGameplayTag& PhaseTag);

UENUM(BlueprintType)
enum class EPhaseTagMatchType : uint8
{
    ExactMatch,
    PartialMatch
};

/**
 *
 */
UCLASS()
class UAfterGamePhaseSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UAfterGamePhaseSubsystem();

    virtual void PostInitialize() override;

    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void StartPhase(
        TSubclassOf<UAfterGamePhaseAbility> PhaseAbility, FAfterGamePhaseDelegate PhaseEndedCallback = FAfterGamePhaseDelegate());

    void WhenPhaseStartsOrIsActive(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FAfterGamePhaseTagDelegate& WhenPhaseActive);
    void WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, const FAfterGamePhaseTagDelegate& WhenPhaseEnd);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, BlueprintPure = false, meta = (AutoCreateRefTerm = "PhaseTag"))
    bool IsPhaseActive(const FGameplayTag& PhaseTag) const;

protected:
    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase",
        meta = (DisplayName = "Start Phase", AutoCreateRefTerm = "PhaseEnded"))
    void K2_StartPhase(TSubclassOf<UAfterGamePhaseAbility> PhaseAbility, const FAfterGamePhaseDynamicDelegate& PhaseEndedDelegate);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase",
        meta = (DisplayName = "When Phase Starts or Is Active", AutoCreateRefTerm = "WhenPhaseActive"))
    void K2_WhenPhaseStartsOrISActive(
        FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FAfterGamePhaseTagDynamicDelegate WhenPhaseActive);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Game Phase",
        meta = (DisplayName = "When Phase Ends", AutoCreateRefTerm = "WhenPhaseEnd"))
    void K2_WhenPhaseEnds(FGameplayTag PhaseTag, EPhaseTagMatchType MatchType, FAfterGamePhaseTagDynamicDelegate WhenPhaseEnd);

    void OnBeginPhase(const UAfterGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);
    void OnEndPhase(const UAfterGamePhaseAbility* PhaseAbility, const FGameplayAbilitySpecHandle PhaseAbilityHandle);

private:
    struct FAfterGamePhaseEntry
    {
    public:
        FGameplayTag PhaseTag;
        FAfterGamePhaseDelegate PhaseEndedCallback;
    };

    TMap<FGameplayAbilitySpecHandle, FAfterGamePhaseEntry> ActivePhaseMap;

    struct FPhaseObserver
    {
    public:
        bool IsMatch(const FGameplayTag& ComparePhaseTag) const;

        FGameplayTag PhaseTag;
        EPhaseTagMatchType MatchType = EPhaseTagMatchType::ExactMatch;
        FAfterGamePhaseTagDelegate PhaseCallback;
    };

    TArray<FPhaseObserver> PhaseStartObservers;
    TArray<FPhaseObserver> PhaseEndObservers;

    friend class UAfterGamePhaseAbility;
};
