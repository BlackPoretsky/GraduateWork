#pragma once

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"
#include "AfterGamePhaseAbility.generated.h"

/**
 * UAfterGamePhaseAbility
 *
 * Базовая игровая способность для способностей меняющих активную фазу игры.
 */
UCLASS(Abstract, HideCategories = Input)
class AFTER_API UAfterGamePhaseAbility : public UAfterGameplayAbility
{
    GENERATED_BODY()

public:
    UAfterGamePhaseAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    const FGameplayTag& GetGamePhaseTag() const { return GamePhaseTag; }

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Game Phase")
    FGameplayTag GamePhaseTag;
};
