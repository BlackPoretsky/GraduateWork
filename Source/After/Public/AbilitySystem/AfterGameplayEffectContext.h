#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "AfterGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class IAfterAbilitySourceInterface;
class UObject;
class UPhysicalMaterial;

/**
 *
 */
USTRUCT()
struct FAfterGameplayEffectContext : public FGameplayEffectContext
{
    GENERATED_BODY()

public:
    FAfterGameplayEffectContext()
        : FGameplayEffectContext(){};

    FAfterGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
        : FGameplayEffectContext(InInstigator, InEffectCauser)
    {
    }

    static AFTER_API FAfterGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

    void SetAbilitySource(const IAfterAbilitySourceInterface* InObject, float InSourceLevel);

    const IAfterAbilitySourceInterface* GetAbilitySource() const;

    virtual FGameplayEffectContext* Duplicate() const override
    {
        FAfterGameplayEffectContext* NewContext = new FAfterGameplayEffectContext();
        *NewContext = *this;
        if (GetHitResult())
        {
            NewContext->AddHitResult(*GetHitResult(), true);
        }

        return NewContext;
    }

    virtual UScriptStruct* GetScriptStruct() const override { return FAfterGameplayEffectContext::StaticStruct(); }

    virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Mapm, bool& bOutSuccess) override;

    const UPhysicalMaterial* GetPhysicalMaterial() const;

public:
    UPROPERTY()
    int32 CartridgeID = -1;

protected:
    UPROPERTY()
    TWeakObjectPtr<const UObject> AbilitySourceObject;
};

template <>
struct TStructOpsTypeTraits<FAfterGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FAfterGameplayEffectContext>
{
    enum
    {
        WithNetSerializer = true,
        WithCopy = true
    };
};
