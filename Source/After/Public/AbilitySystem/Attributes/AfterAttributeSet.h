#pragma once

#include "AttributeSet.h"

#include "AfterAttributeSet.generated.h"

class AActor;
class UAfterAbilitySystemComponent;
class UWorld;
struct FGameplayEffectSpec;

/**
 * Этот макрос определяет набор вспомогательных функций для доступа к атрибутам и их инициализации.
 *
 * Пример использования макроса:
 *     ATTRIBUTE_ACCESSORS(UAfterHealthSet, Health)
 * будут созданы следующие функции:
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
 * Делегат, используемый для трансляции событий атрибутов:
 * @param EffectInstigator	Инициатор события.
 * @param EffectCauser		Физиический объект вызвавший изменения.
 * @param EffectSpec		Полная спецификация изменений для эффекта.
 * @param EffectMagnitude	Необработанная величина.
 * @param OldValue			Значение атрибута до изменения.
 * @param NewValue			Значение после изменения.
 */
DECLARE_MULTICAST_DELEGATE_SixParams(FAfterAttributeEvent, AActor* /*EffectInstigator*/, AActor* /*EffectCauser*/,
    const FGameplayEffectSpec* /*EffectSpec*/, float /*EffectMagnitude*/, float /*OldValue*/, float /*NewValue*/);

/**
 * UAfterAttributeSet
 *
 * Базовый класс набора атрибут для проекта.
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
