#pragma once

#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/AfterAbilityCost.h"
#include "ScalableFloat.h"

#include "AfterAbilityCost_PlayerTagStack.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UAfterGameplayAbility;
class UObject;
struct FGameplayAbilityActorInfo;

/**
 * ���������, ������������� ���� � ����������� �� ��������� ������.
 */
UCLASS(meta = (DisplayName = "Palyer Tag Stack"))
class AFTER_API UAfterAbilityCost_PlayerTagStack : public UAfterAbilityCost
{
    GENERATED_BODY()

public:
    UAfterAbilityCost_PlayerTagStack();

    //~UAfterAbilityCost interface
    virtual bool CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRlevantTags) const override;
    virtual void ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    //~End of UAfterAbilityCost interface

protected:
    /** ��������� �������. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    FScalableFloat Quantity;

    /** ������������� ���. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    FGameplayTag Tag;
};
