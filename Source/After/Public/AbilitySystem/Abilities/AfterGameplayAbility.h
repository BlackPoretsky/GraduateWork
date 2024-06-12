#pragma once

#include "Abilities/GameplayAbility.h"

#include "AfterGameplayAbility.generated.h"

class AActor;
class AController;
class AAfterPlayerController;
class APlayerController;
class AAfterCharacter;
class FText;
class IAfterAbilitySourceInterface;
class UanimMontage;
class UAfterAbilityCost;
class UAfterAbilitySystemComponent;
class UAfterCameraMode;
class UAfterHeroComponent;

struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;
struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

/**
 * EAfterAbilityActivationPolice
 *
 * ���������� ������ ��������� �����������.
 */
UENUM(BlueprintType)
enum class EAfterAbilityActivationPolicy : uint8
{
    OnInputTriggered,
    WhileInputActive,
    OnSpawn
};

/**
 * UAfterAbilityActivationGroup
 *
 * ����������, ��� ����������� ������������ �� ��������� � ������ ������������.
 */
UENUM(BlueprintType)
enum class EAfterAbilityActivationGroup : uint8
{
    Independent,
    Exclusive_Replaceable,
    Exclusive_Blocking,

    MAX UMETA(Hidden)
};

/** ������� ����, ������� ����� ���� ������������ ��� ��������������� ������������� ������� ��� ������������� ���� */
USTRUCT(BlueprintType)
struct FAfterAbilityMontageFailureMessage
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<APlayerController> PlayerController = nullptr;

    UPROPERTY(BlueprintReadWrite)
    FGameplayTagContainer FailureTags;

    UPROPERTY(BlueprintReadWrite)
    TObjectPtr<UAnimMontage> FailureMontage = nullptr;
};

/**
 * UAfterGameplayAbility
 *
 * ������� ����� ������������ ������������ � �������.
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class AFTER_API UAfterGameplayAbility : public UGameplayAbility
{
    GENERATED_BODY()
    friend class UAfterAbilitySystemComponent;

public:
    UAfterGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponentFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    AAfterPlayerController* GetAfterPlayerControllerFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    AController* GetControllerFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    AAfterCharacter* GetAfterCharacterFromActorInfo() const;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    UAfterHeroComponent* GetHeroComponentFromActorInfo() const;

    EAfterAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
    EAfterAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

    void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

    /**���������� �������� true, ���� ����������� ������ ��������� �������� ���������� ���������. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "After|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
    bool CanChangeActivationGroup(EAfterAbilityActivationGroup NewGroup) const;

    /** �������� �������� ������ ���������. ���������� �������� true, ���� ��� ���� ������� ��������. */
    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "After|Ability", Meta = (ExpandBoolAsExecs = "ReturnValue"))
    bool ChangeActivationGroup(EAfterAbilityActivationGroup NewGroup);

    /** ������������� ����� ������*/
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void SetCameraMode(TSubclassOf<UAfterCameraMode> CameraMode);

    /** ���������� ����� ������*/
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void ClearCameraMode();

    void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
    {
        NativeOnAbilityFailedToActivate(FailedReason);
        ScriptOnAbilityFailedToActivate(FailedReason);
    }

protected:
    virtual void NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const;

    UFUNCTION(BlueprintImplementableEvent)
    void ScriptOnAbilityFailedToActivate(const FGameplayTagContainer& FailReason) const;

    //~UGameplayAbility interface
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevantTags) const override;
    virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo) const override;
    virtual FGameplayEffectContextHandle MakeEffectContext(
        const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
    virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
    virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
        const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    //~End of UGameplayAbility interface

    virtual void OnPawnAvatarSet();

    virtual void GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& OutSourceLevel,
        const IAfterAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const;

    /** ����������, ����� ����������� ��������������� AbilitySystemComponent */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityAdded")
    void K2_OnAbilityAdded();

    /** ����������, ����� ����������� ��������� �� AbilitySystemComponent */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnAbilityRemoved")
    void K2_OnAbilityRemoved();

    /** ����������, ����� ����������� ���������������� � ������� ������� �����. */
    UFUNCTION(BlueprintImplementableEvent, Category = Ability, DisplayName = "OnPawnAvatarSet")
    void K2_OnPawnAvatarSet();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Ability Activation")
    EAfterAbilityActivationPolicy ActivationPolicy;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Ability Activation")
    EAfterAbilityActivationGroup ActivationGroup;

    UPROPERTY(EditDefaultsOnly, Instanced, Category = "Costs")
    TArray<TObjectPtr<UAfterAbilityCost>> AdditionalCosts;

    UPROPERTY(EditDefaultsOnly, Category = "Advanced")
    TMap<FGameplayTag, TObjectPtr<UAnimMontage>> FailureTagToAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "Advanced")
    bool bLogCancelation;

    TSubclassOf<UAfterCameraMode> ActiveCameraMode;
};
