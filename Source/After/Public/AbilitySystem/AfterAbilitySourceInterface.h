#pragma once

#include "UObject/Interface.h"

#include "AfterAbilitySourceInterface.generated.h"

class UObject;
class UPhysicalMaterial;
struct FGameplayTagContainer;

UINTERFACE(MinimalAPI)
class UAfterAbilitySourceInterface : public UInterface
{
    GENERATED_BODY()
};

class AFTER_API IAfterAbilitySourceInterface
{
    GENERATED_BODY()

public:
    virtual float GetDistanceAttenuation(
        float Diastance, const FGameplayTagContainer* SourceTag = nullptr, const FGameplayTagContainer* TargetTag = nullptr) const = 0;
};
