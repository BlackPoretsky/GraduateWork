#pragma once

#include "ActiveGameplayEffectHandle.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayAbilitySpecHandle.h"

#include "AfterGlobalAbilitySystem.generated.h"

class UGameplayAbility;
class UGameplayEffect;
class UAfterAbilitySystemComponent;
class UObject;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpecHandle;

USTRUCT()
struct FGlobalAppliedAbilityList
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UAfterAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

    void AddToASC(TSubclassOf<UGameplayAbility> Ability, UAfterAbilitySystemComponent* ASC);
    void RemoveFromASC(UAfterAbilitySystemComponent* ASC);
    void RemoveFromAll();
};

USTRUCT()
struct FGlobalAppliedEffectList
{
    GENERATED_BODY()

    UPROPERTY()
    TMap<TObjectPtr<UAfterAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

    void AddToASC(TSubclassOf<UGameplayEffect> Effect, UAfterAbilitySystemComponent* ASC);
    void RemoveFromASC(UAfterAbilitySystemComponent* ASC);
    void RemoveFromAll();
};

UCLASS()
class UAfterGlobalAbilitySystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UAfterGlobalAbilitySystem();

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "After")
    void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "After")
    void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "After")
    void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "After")
    void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

    void RegisterASC(UAfterAbilitySystemComponent* ASC);
    void UnregisterASC(UAfterAbilitySystemComponent* ASC);

private:
    UPROPERTY()
    TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilityList> AppliedAbilities;

    UPROPERTY()
    TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffectList> AppliedEffects;

    UPROPERTY()
    TArray<TObjectPtr<UAfterAbilitySystemComponent>> RegisteredASCs;
};
