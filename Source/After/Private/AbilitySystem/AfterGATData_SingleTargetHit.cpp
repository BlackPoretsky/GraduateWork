#include "AbilitySystem/AfterGATData_SingleTargetHit.h"
#include "AbilitySystem/AfterGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGATData_SingleTargetHit)

void FAfterGATData_SingleTargetHit::AddTargetDataToContext(FGameplayEffectContextHandle& Context, bool bIncludeActorArray) const
{
    FGameplayAbilityTargetData_SingleTargetHit::AddTargetDataToContext(Context, bIncludeActorArray);

    if (FAfterGameplayEffectContext* TypedContext = FAfterGameplayEffectContext::ExtractEffectContext(Context))
    {
        TypedContext->CartridgeID = CartridgeID;
    }
}

bool FAfterGATData_SingleTargetHit::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    FGameplayAbilityTargetData_SingleTargetHit::NetSerialize(Ar, Map, bOutSuccess);

    Ar << CartridgeID;

    return true;
}
