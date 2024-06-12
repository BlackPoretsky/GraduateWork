#pragma once

#include "Components/GameFrameworkComponent.h"
#include "AfterCoreTypes.h"
#include "AfterHealthComponent.generated.h"

class UAfterHealthComponent;

class UAfterAbilitySystemComponent;
class UAfterHealthSet;

struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAfterHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
    FAfterHealth_AttributeChanged, UAfterHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

UENUM(BlueprintType)
enum class EAfterDeathState : uint8
{
    NotDead = 0,
    DeathStarted,
    DeathFinished
};

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class AFTER_API UAfterHealthComponent : public UGameFrameworkComponent
{
    GENERATED_BODY()

public:
    UAfterHealthComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "After|Health")
    static UAfterHealthComponent* FindHealthComponent(const AActor* Actor)
    {
        return (Actor ? Actor->FindComponentByClass<UAfterHealthComponent>() : nullptr);
    }

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    void InitializeWithAbilitySystem(UAfterAbilitySystemComponent* InASC);

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    void UninitializeFromAbilitySystem();

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    float GetHealthNormalized() const;

    UFUNCTION(BlueprintCallable, Category = "After|Health")
    EAfterDeathState GetDeathState() const { return DeathState; };

    UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "After|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
    bool IsDeadOrDying() const { return (DeathState > EAfterDeathState::NotDead); }

    virtual void StartDeath();

    virtual void FinishDeath();

public:
    UPROPERTY(BlueprintAssignable)
    FAfterHealth_AttributeChanged OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FAfterHealth_AttributeChanged OnMaxHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FAfterHealth_DeathEvent OnDeathStarted;

    UPROPERTY(BlueprintAssignable)
    FAfterHealth_DeathEvent OnDeathFinished;

protected:
    virtual void OnUnregister() override;

    void ClearGameplayTags();

    virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
        float DamageMagnitude, float OldValue, float NewValue);
    virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
        float DamageMagnitude, float OldValue, float NewValue);
    virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec,
        float DamageMagnitude, float OldValue, float NewValue);

protected:
    UPROPERTY()
    TObjectPtr<UAfterAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<const UAfterHealthSet> HealthSet;

    EAfterDeathState DeathState;
};
