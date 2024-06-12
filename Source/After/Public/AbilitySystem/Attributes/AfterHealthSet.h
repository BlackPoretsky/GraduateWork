#pragma once

#include "AbilitySystem/Attributes/AfterAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "AfterHealthSet.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Damage);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageImmunity);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_DamageSelfDestruct);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_FellOutOfWorld);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_After_Damage_Message);

struct FGameplayEffectModCallbackData;

/**
 * UAfterHealthSet
 *
 *  ласс, определ€ющий атрибуты, необходимые дл€ получени€ урона.
 */
UCLASS(BlueprintType)
class UAfterHealthSet : public UAfterAttributeSet
{
    GENERATED_BODY()

public:
    UAfterHealthSet();

    ATTRIBUTE_ACCESSORS(UAfterHealthSet, Health);
    ATTRIBUTE_ACCESSORS(UAfterHealthSet, MaxHealth);
    ATTRIBUTE_ACCESSORS(UAfterHealthSet, Healing);
    ATTRIBUTE_ACCESSORS(UAfterHealthSet, Damage);

    mutable FAfterAttributeEvent OnHealthChanged;

    mutable FAfterAttributeEvent OnMaxHealthChanged;

    mutable FAfterAttributeEvent OnOutOfHealth;

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    UFUNCTION()
    void OnRep_Health(const FGameplayAttributeData& OldValue);

    UFUNCTION()
    void OnRep_MaxHealth(const FGameplayAttributeData& OldValue);

    virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribure, float OldValue, float NewValue) override;

    void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:
    UPROPERTY(
        BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "After|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
    FGameplayAttributeData Health;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "After|Helath",
        Meta = (HideFromModifiers, AllowPrivateAccess = true))
    FGameplayAttributeData MaxHealth;

    // »спользуетс€ дл€ отслеживани€, когда HP равны 0
    bool bOutOfHealth;

    // —охран€ем здоровье перед любыми изменени€ми
    float MaxHealthBeforeAttributeChange;
    float HealthBeforeAttributeChange;

    UPROPERTY(BlueprintReadOnly, Category = "After|Health", Meta = (AllowPrivateAccess = true))
    FGameplayAttributeData Healing;

    UPROPERTY(BlueprintReadOnly, Category = "After|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
    FGameplayAttributeData Damage;
};
