#include "AbilitySystem/AfterGlobalAbilitySystem.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGlobalAbilitySystem)

///////////////////////////////
// FGlobalAppliedAbilityList //
///////////////////////////////
void FGlobalAppliedAbilityList::AddToASC(TSubclassOf<UGameplayAbility> Ability, UAfterAbilitySystemComponent* ASC)
{
    if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
    {
        RemoveFromASC(ASC);
    }

    UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
    FGameplayAbilitySpec AbilitySpec(AbilityCDO);
    const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
    Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilityList::RemoveFromASC(UAfterAbilitySystemComponent* ASC)
{
    if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
    {
        ASC->ClearAbility(*SpecHandle);
        Handles.Remove(ASC);
    }
}

void FGlobalAppliedAbilityList::RemoveFromAll()
{
    for (auto& KVP : Handles)
    {
        if (KVP.Key != nullptr)
        {
            KVP.Key->ClearAbility(KVP.Value);
        }
    }
    Handles.Empty();
}

///////////////////////////////
// FGlobalAppliedEffectList //
///////////////////////////////
void FGlobalAppliedEffectList::AddToASC(TSubclassOf<UGameplayEffect> Effect, UAfterAbilitySystemComponent* ASC)
{
    if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
    {
        RemoveFromASC(ASC);
    }

    const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
    const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, 1, ASC->MakeEffectContext());
    Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffectList::RemoveFromASC(UAfterAbilitySystemComponent* ASC)
{
    if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
    {
        ASC->RemoveActiveGameplayEffect(*EffectHandle);
        Handles.Remove(ASC);
    }
}

void FGlobalAppliedEffectList::RemoveFromAll()
{
    for (auto& KVP : Handles)
    {
        if (KVP.Key != nullptr)
        {
            KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
        }
    }
    Handles.Empty();
}

///////////////////////////////
// UAfterGlobalAbilitySystem //
///////////////////////////////
UAfterGlobalAbilitySystem::UAfterGlobalAbilitySystem() {}

void UAfterGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
    if ((IsValid(Ability)) && (!AppliedAbilities.Contains(Ability)))
    {
        FGlobalAppliedAbilityList& Entry = AppliedAbilities.Add(Ability);
        for (UAfterAbilitySystemComponent* ASC : RegisteredASCs)
        {
            Entry.AddToASC(Ability, ASC);
        }
    }
}

void UAfterGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
    if ((IsValid(Effect)) && (!AppliedEffects.Contains(Effect)))
    {
        FGlobalAppliedEffectList& Entry = AppliedEffects.Add(Effect);
        for (UAfterAbilitySystemComponent* ASC : RegisteredASCs)
        {
            Entry.AddToASC(Effect, ASC);
        }
    }
}

void UAfterGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
    if ((IsValid(Ability)) && AppliedAbilities.Contains(Ability))
    {
        FGlobalAppliedAbilityList& Entry = AppliedAbilities[Ability];
        Entry.RemoveFromAll();
        AppliedAbilities.Remove(Ability);
    }
}

void UAfterGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
    if ((IsValid(Effect)) && AppliedEffects.Contains(Effect))
    {
        FGlobalAppliedEffectList& Entry = AppliedEffects[Effect];
        Entry.RemoveFromAll();
        AppliedEffects.Remove(Effect);
    }
}

void UAfterGlobalAbilitySystem::RegisterASC(UAfterAbilitySystemComponent* ASC)
{
    check(ASC);

    for (auto& Entry : AppliedAbilities)
    {
        Entry.Value.AddToASC(Entry.Key, ASC);
    }

    for (auto& Entry : AppliedEffects)
    {
        Entry.Value.AddToASC(Entry.Key, ASC);
    }

    RegisteredASCs.AddUnique(ASC);
}

void UAfterGlobalAbilitySystem::UnregisterASC(UAfterAbilitySystemComponent* ASC)
{
    check(ASC);

    for (auto& Entry : AppliedAbilities)
    {
        Entry.Value.RemoveFromASC(ASC);
    }

    for (auto& Entry : AppliedEffects)
    {
        Entry.Value.RemoveFromASC(ASC);
    }

    RegisteredASCs.Remove(ASC);
}
