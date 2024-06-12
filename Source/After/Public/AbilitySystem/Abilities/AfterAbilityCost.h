#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/GameplayAbility.h"
#include "AfterAbilityCost.generated.h"

class UAfterGameplayAbility;

/**
 * UAfterAbilityCost
 *
 * Базовый клас для затрат на использование способностей
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class AFTER_API UAfterAbilityCost : public UObject
{
    GENERATED_BODY()

public:
    UAfterAbilityCost() {}

    /**
     * Проверяет, можем ли мы позволить себе использовать способность.
     */
    virtual bool CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
    {
        return true;
    }

    /**
     * Применяет стоимость способности к целевому параметру.
     */
    virtual void ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
    {
    }

    bool ShouldOnlyApplyCostOnHit() const { return bOnlyApplyCostOnHit; }

protected:
    /** Если значение true, то затраты срабатывает только вслучае успешного применения способности. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    bool bOnlyApplyCostOnHit = false;
};
