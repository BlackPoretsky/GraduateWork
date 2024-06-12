#include "AbilitySystem/AfterAbilitySystemGlobals.h"

#include "AbilitySystem/AfterGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilitySystemGlobals)

UAfterAbilitySystemGlobals::UAfterAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

FGameplayEffectContext* UAfterAbilitySystemGlobals::AllocGameplayEffectContext() const
{
    return new FAfterGameplayEffectContext();
}
