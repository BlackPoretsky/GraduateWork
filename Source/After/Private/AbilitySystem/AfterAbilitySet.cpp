#include "AbilitySystem/AfterAbilitySet.h"

#include "AbilitySystem/Abilities/AfterGameplayAbility.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "AfterLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilitySet)

/////////////////////////////////////
// FAfterAbilitySet_GrantedHandles //
/////////////////////////////////////
void FAfterAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
    if (Handle.IsValid())
    {
        AbilitySpecHandles.Add(Handle);
    }
}

void FAfterAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
    if (Handle.IsValid())
    {
        GameplayEffectHandles.Add(Handle);
    }
}

void FAfterAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
    GrantedAttributeSets.Add(Set);
}

void FAfterAbilitySet_GrantedHandles::TakeFromAbilitySystem(UAfterAbilitySystemComponent* AfterASC)
{
    check(AfterASC);

    if (!AfterASC->IsOwnerActorAuthoritative()) return;

    for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
    {
        if (Handle.IsValid())
        {
            AfterASC->ClearAbility(Handle);
        }
    }

    for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
    {
        if (Handle.IsValid())
        {
            AfterASC->RemoveActiveGameplayEffect(Handle);
        }
    }

    for (UAttributeSet* Set : GrantedAttributeSets)
    {
        AfterASC->RemoveSpawnedAttribute(Set);
    }

    AbilitySpecHandles.Reset();
    GameplayEffectHandles.Reset();
    GrantedAttributeSets.Reset();
}

//////////////////////
// UAfterAbilitySet //
//////////////////////
UAfterAbilitySet::UAfterAbilitySet(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterAbilitySet::GiveToAbilitySystem(
    UAfterAbilitySystemComponent* AfterASC, FAfterAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    check(AfterASC);

    if (!AfterASC->IsOwnerActorAuthoritative()) return;

    for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
    {
        const FAfterAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

        if (!IsValid(AbilityToGrant.Ability))
        {
            UE_LOG(LogAfterAbilitySystem, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex,
                *GetNameSafe(this));
            continue;
        }

        UAfterGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UAfterGameplayAbility>();

        FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;
        AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

        const FGameplayAbilitySpecHandle AbilitySpecHandle = AfterASC->GiveAbility(AbilitySpec);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
        }
    }

    for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
    {
        const FAfterAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

        if (!IsValid(EffectToGrant.GameplayEffect))
        {
            UE_LOG(LogAfterAbilitySystem, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex,
                *GetNameSafe(this));
            continue;
        }

        const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
        const FActiveGameplayEffectHandle GameplayEffectHandle =
            AfterASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, AfterASC->MakeEffectContext());

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
        }
    }

    for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
    {
        const FAfterAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

        if (!IsValid(SetToGrant.AttributeSet))
        {
            UE_LOG(
                LogAfterAbilitySystem, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
            continue;
        }

        UAttributeSet* NewSet = NewObject<UAttributeSet>(AfterASC->GetOwner(), SetToGrant.AttributeSet);
        AfterASC->AddAttributeSetSubobject(NewSet);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAttributeSet(NewSet);
        }
    }
}
