#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"

#include "AfterGATData_SingleTargetHit.generated.h"

class FArchive;
struct FGameplayEffectContextHandle;

/**
 *
 */
USTRUCT()
struct FAfterGATData_SingleTargetHit : public FGameplayAbilityTargetData_SingleTargetHit
{
    GENERATED_BODY()

public:
    FAfterGATData_SingleTargetHit()
        : CartridgeID(-1)
    {
    }

    virtual void AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const override;

    UPROPERTY()
    int32 CartridgeID;

    bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

    virtual UScriptStruct* GetScriptStruct() const override { return FAfterGATData_SingleTargetHit::StaticStruct(); }
};

template <>
struct TStructOpsTypeTraits<FAfterGATData_SingleTargetHit> : public TStructOpsTypeTraitsBase2<FAfterGATData_SingleTargetHit>
{
    enum
    {
        WithNetSerializer = true
    };
};
