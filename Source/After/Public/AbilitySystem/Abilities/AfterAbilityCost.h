#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "AfterAbilityCost.generated.h"

class UAfterGameplayAbility;

/**
 * UAfterAbilityCost
 *
 * ������� ���� ��� ������ �� ������������� ������������
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class AFTER_API UAfterAbilityCost : public UObject
{
    GENERATED_BODY()

public:
    UAfterAbilityCost() {}

    /**
     * ���������, ����� �� �� ��������� ���� ������������ �����������.
     */
    virtual bool CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
    {
        return true;
    }

    /**
     * ��������� ��������� ����������� � �������� ���������.
     */
    virtual void ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
    {
    }

    bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
    /** ���� �������� true, �� ������� ����������� ������ ������� ��������� ���������� �����������. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    bool bOnlyApplyCostOnHit = false;
};
