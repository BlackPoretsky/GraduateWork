// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Attributes/AfterCombatSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCombatSet)

class FLifetimeProperty;

UAfterCombatSet::UAfterCombatSet()
    : BaseDamage(0.0f),
      BaseHeal(0.0f)
{
}

void UAfterCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAfterCombatSet, BaseDamage, OldValue)
}

void UAfterCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
    GAMEPLAYATTRIBUTE_REPNOTIFY(UAfterCombatSet, BaseHeal, OldValue);
}
