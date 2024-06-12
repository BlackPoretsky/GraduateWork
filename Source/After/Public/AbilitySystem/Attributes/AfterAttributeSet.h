#pragma once

#include "AttributeSet.h"

#include "AfterAttributeSet.generated.h"

class AActor;
class UAfterAbilitySystemComponent;
class UWorld;
struct FGameplayEffectSpec;

/**
 * ���� ������ ���������� ����� ��������������� ������� ��� ������� � ��������� � �� �������������.
 *
 * ������ ������������� �������:
 *     ATTRIBUTE_ACCESSORS(UAfterHealthSet, Health)
 * ����� ������� ��������� �������:
 *     static FGameplayAttribute GetHealthAttribute();
 *     float GetHealth() const;
 *     void SetHealth(float NewVal);
 *     void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)           \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)               \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * �������, ������������ ��� ���������� ������� ���������:
 * @param EffectInstigator	��������� �������.
 * @param EffectCauser		����������� ������ ��������� ���������.
 * @param EffectSpec		������ ������������ ��������� ��� �������.
 * @param EffectMagnitude	�������������� ��������.
 * @param OldValue			�������� �������� �� ���������.
 * @param NewValue			�������� ����� ���������.
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FAfterAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
    const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/**
 * UAfterAttributeSet
 *
 * ������� ����� ������ ������� ��� �������.
 */
UCLASS()
class AFTER_API UAfterAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UAfterAttributeSet();

    UWorld* GetWorld() const override;

    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponent() const;
};
