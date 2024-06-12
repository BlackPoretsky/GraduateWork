#pragma once

#include "AfterGameplayAbility.h"

#include "AfterGameplayAbility_Jump.generated.h"

struct FGameplayAbilityActorInfo;
struct FGameplaTagContainer;

/**
 * UAfterGameplayAbility_Jump
 *
 * Способность используемая персонажем для прыжка.
 */
UCLASS(Abstract)
class UAfterGameplayAbility_Jump : public UAfterGameplayAbility
{
    GENERATED_BODY()

public:
    UAfterGameplayAbility_Jump(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTag, const FGameplayTagContainer* TargetTags,
        FGameplayTagContainer* OptionalRelevanTags) const override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void CharacterJumpStart();

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    void CharacterJumpStop();
};
