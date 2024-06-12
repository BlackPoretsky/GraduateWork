#pragma once

#include "AbilitySystemGlobals.h"

#include "AfterAbilitySystemGlobals.generated.h"

class UObject;
struct FGameplayEffectContext;

UCLASS()
class UAfterAbilitySystemGlobals : public UAbilitySystemGlobals
{
    GENERATED_BODY()

public:
    UAfterAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer);

private:
    //~UAbilitySystemGlobals interface
    virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
    //~End of UAbilitySystemGlobals interface
};
