#include "AbilitySystem/Attributes/AfterAttributeSet.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAttributeSet)

UAfterAttributeSet::UAfterAttributeSet() {}

UWorld* UAfterAttributeSet::GetWorld() const
{
    const UObject* Outer = GetOuter();
    check(Outer);

    return Outer->GetWorld();
}

UAfterAbilitySystemComponent* UAfterAttributeSet::GetAfterAbilitySystemComponent() const
{
    return Cast<UAfterAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}
