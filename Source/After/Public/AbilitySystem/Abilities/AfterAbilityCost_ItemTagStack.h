#pragma once

#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/AfterAbilityCost.h"
#include "ScalableFloat.h"

#include "AfterAbilityCost_ItemTagStack.generated.h"

class UAfterGameplayAbility;

struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

/**
 * Затраты, которые требуют определённые стек тегов для экземляра предмета.
 */
UCLASS(meta = (DisplayName = "Item Tag Stack"))
class AFTER_API UAfterAbilityCost_ItemTagStack : public UAfterAbilityCost
{
    GENERATED_BODY()

public:
    UAfterAbilityCost_ItemTagStack();

    //~UAfterAbilityCost interface
    virtual bool CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
    virtual void ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    //~End of UAfterAbilityCost interface

protected:
    /** Требуемые затраты. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    FScalableFloat Quantity;

    /** Затрачиваемый тег. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    FGameplayTag Tag;

    /** Тег ошибки если способность неможет быть применена. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Costs")
    FGameplayTag FailureTag;
};
