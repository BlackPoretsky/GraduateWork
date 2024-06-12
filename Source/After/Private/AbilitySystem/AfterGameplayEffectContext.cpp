#include "AbilitySystem/AfterGameplayEffectContext.h"

#include "AbilitySystem/AfterAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameplayEffectContext)

AFTER_API FAfterGameplayEffectContext* FAfterGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
    FGameplayEffectContext* BaseEffectContext = Handle.Get();
    if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FAfterGameplayEffectContext::StaticStruct()))
    {
        return (FAfterGameplayEffectContext*)BaseEffectContext;
    }

    return nullptr;
}

#if UE_WITH_IRIS
namespace UE::Net
{
    UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(AfterGameplayEffectContext, FGameplayEffectContextNetSerializer);
}  // namespace UE::Net
#endif

void FAfterGameplayEffectContext::SetAbilitySource(const IAfterAbilitySourceInterface* InObject, float InSourceLevel)
{
    AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
}

const IAfterAbilitySourceInterface* FAfterGameplayEffectContext::GetAbilitySource() const
{
    return Cast<IAfterAbilitySourceInterface>(AbilitySourceObject.Get());
}

bool FAfterGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
    FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

    return true;
}

const UPhysicalMaterial* FAfterGameplayEffectContext::GetPhysicalMaterial() const
{
    if (const FHitResult* HitResultPtr = GetHitResult())
    {
        return HitResultPtr->PhysMaterial.Get();
    }

    return nullptr;
}
