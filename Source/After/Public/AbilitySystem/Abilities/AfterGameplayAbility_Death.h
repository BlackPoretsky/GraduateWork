#pragma once

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"

#include "AfterGameplayAbility_Death.generated.h"

struct FGameplayAbiltiyActorInfo;
struct FGameplayEventData;

/**
 * UAfterGameplayAbility_Death
 *
 * ������� ����������� ������������ ��� ��������� ������.
 * ����������� ������������� ������������ ����� ��� "GameplayEvent.Death".
 */
UCLASS(Abstract)
class UAfterGameplayAbility_Death : public UAfterGameplayAbility
{
    GENERATED_BODY()

public:
    UAfterGameplayAbility_Death(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    /** ��������� ������������������ ������*/
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void StartDeath();

    /** ��������� ������������������ ������. */
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void FinishDeath();

protected:
    /** ���� �������� true, �� ����������� ������������� �������� StartDeath. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Death")
    bool bAutoStartDeath;
};
