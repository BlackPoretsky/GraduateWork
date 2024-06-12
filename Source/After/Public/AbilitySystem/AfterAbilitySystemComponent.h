#pragma once

#include "Abilities/AfterGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "AfterAbilitySystemComponent.generated.h"

class AActor;
class UGameplayAbility;
class UAfterAbilityTagRelationshipMapping;
class UObject;

struct FFrame;
struct FGameplayAbilityTargetDataHandle;

AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_AbilityInputBlocked);

/**
 * UAfterAbilitySystemComponent
 *
 */
UCLASS()
class AFTER_API UAfterAbilitySystemComponent : public UAbilitySystemComponent
{
    GENERATED_BODY()

public:
    UAfterAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~UActorComponent interface
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent interface

    virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

    typedef TFunctionRef<bool(const UAfterGameplayAbility* AfterAbility, FGameplayAbilitySpecHandle Handle)> TShouldCancelAbilityFunc;
    void CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility);

    void CancelInputActivatedAbilities(bool bReplicateCancelAbility);

    void AbilityInputTagPressed(const FGameplayTag& InputTag);
    void AbilityInputTagReleased(const FGameplayTag& InputTag);

    void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
    void ClearAbilityInput();

    bool IsActivationGroupBlocked(EAfterAbilityActivationGroup Group) const;
    void AddAbilityToActivationGroup(EAfterAbilityActivationGroup Group, UAfterGameplayAbility* AfterAbility);
    void RemoveAbilityFromActivationGroup(EAfterAbilityActivationGroup Group, UAfterGameplayAbility* AfterAbility);
    void CancelActivationGroupAbilities(
        EAfterAbilityActivationGroup Group, UAfterGameplayAbility* IgnoreAfterAbility, bool bReplicateCancelAbility);

    void AddDynamicTagGameplayEffect(const FGameplayTag& Tag);

    void RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag);

    void GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo,
        FGameplayAbilityTargetDataHandle& OutTargetDataHandle);

    void SetTagRelationshipMapping(UAfterAbilityTagRelationshipMapping* NewMapping);

    void GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer& OutActivationRequired,
        FGameplayTagContainer& OutActivationBlocked) const;

protected:
    void TryActivateAbilitiesOnSpawn();

    virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;
    virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

    virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;
    virtual void NotifyAbilityFailed(
        const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason) override;
    virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;
    virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility,
        bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags,
        const FGameplayTagContainer& CancelTags) override;

    void HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason);

protected:
    UPROPERTY()
    TObjectPtr<UAfterAbilityTagRelationshipMapping> TagRelationshipMapping;

    TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;
    TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;
    TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

    int32 ActivationGroupCounts[(uint8)EAfterAbilityActivationGroup::MAX];
};
