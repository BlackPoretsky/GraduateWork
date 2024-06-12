#pragma once

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"

#include "AfterGameplayAbility_Death.generated.h"

struct FGameplayAbiltiyActorInfo;
struct FGameplayEventData;

/**
 * UAfterGameplayAbility_Death
 *
 * »грова€ способность использу€ма€ дл€ обработки смерти.
 * —пособность автоматически активируетс€ через тег "GameplayEvent.Death".
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

    /** «апускает последовательность смерти*/
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void StartDeath();

    /** «авершает последоваетльность смерти. */
    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void FinishDeath();

protected:
    /** ≈сли значение true, то способность автоматически запустит StartDeath. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Death")
    bool bAutoStartDeath;
};
