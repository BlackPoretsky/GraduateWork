#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Engine/DataAsset.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "AfterAbilitySet.generated.h"

class UAttributeSet;
class UGameplayEffect;
class UAfterAbilitySystemComponent;
class UAfterGameplayAbility;

/**
 * FAfterAbilitySet_GameplayAbility
 *
 * Данные, описывающие способность.
 */
USTRUCT(BlueprintType)
struct FAfterAbilitySet_GameplayAbility
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAfterGameplayAbility> Ability = nullptr;

    UPROPERTY(EditDefaultsOnly)
    int32 AbilityLevel = 1;

    UPROPERTY(EditDefaultsOnly, meta = (Categories = "InputTag"))
    FGameplayTag InputTag;
};

/**
 * FAfterAbilitySet_GameplayEffect
 *
 * Данные, описывающие эффект способности.
 */
USTRUCT(BlueprintType)
struct FAfterAbilitySet_GameplayEffect
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

    UPROPERTY(EditDefaultsOnly)
    float EffectLevel = 1.0f;
};

/**
 * FAfterAbilitySet_AttributeSet
 *
 * Данные, описывающие атрибут способности.
 */
USTRUCT(BlueprintType)
struct FAfterAbilitySet_AttributeSet
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UAttributeSet> AttributeSet;
};

/**
 * FAfterAbilitySet_GrantedHandles
 *
 * Данные, используемые для хранения обработчиков предоставленных множеству способностей.
 */
USTRUCT(BlueprintType)
struct FAfterAbilitySet_GrantedHandles
{
    GENERATED_BODY()

public:
    void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
    void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
    void AddAttributeSet(UAttributeSet* Set);

    void TakeFromAbilitySystem(UAfterAbilitySystemComponent* AfterASC);

protected:
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

    UPROPERTY()
    TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};

/**
 * UAfterAbilitySet
 *
 * Служит для предоставления игровых способностей и эффектво.
 */
UCLASS(BlueprintType, Const)
class AFTER_API UAfterAbilitySet : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UAfterAbilitySet(const FObjectInitializer& ObjectInitializer);

    /** Предоставляет набор способностей указанному компоненту системы способностей. */
    void GiveToAbilitySystem(
        UAfterAbilitySystemComponent* AfterASC, FAfterAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:
    /** Геймплейные способности, которые предоставляются, когда предоставляется этот набор способностей. */
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitlePropery = "Ablity"))
    TArray<FAfterAbilitySet_GameplayAbility> GrantedGameplayAbilities;

    /** Геймплейные эффекты, которые предоставляются, когда предоставляется этот набор способностей. */
    UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effect", meta = (TitleProperty = "GameplayEffect"))
    TArray<FAfterAbilitySet_GameplayEffect> GrantedGameplayEffects;

    /**  Атрибуты, которые предоставляются, когда предоставляется этот набор способностей. */
    UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = "AttributeSet"))
    TArray<FAfterAbilitySet_AttributeSet> GrantedAttributes;
};
