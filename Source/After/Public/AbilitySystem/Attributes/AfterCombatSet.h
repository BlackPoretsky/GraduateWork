#pragma once

#include "AbilitySystem/Attributes/AfterAttributeSet.h"
#include "AbilitySystemComponent.h"

#include "AfterCombatSet.generated.h"

/**
 * UAfterCombatSet
 *
 */
UCLASS()
class UAfterCombatSet : public UAfterAttributeSet
{
    GENERATED_BODY()

public:
    UAfterCombatSet();

    ATTRIBUTE_ACCESSORS(UAfterCombatSet, BaseDamage);
    ATTRIBUTE_ACCESSORS(UAfterCombatSet, BaseHeal);

protected:
    UFUNCTION()
    void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_BaseHeal(const FGameplayAttributeData& OldValue);

private:
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "After|Combat", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData BaseDamage;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHeal, Category = "After|Combat", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData BaseHeal;
};
