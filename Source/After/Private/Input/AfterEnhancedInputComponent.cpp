#include "Input/AfterEnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterEnhancedInputComponent)

UAfterEnhancedInputComponent::UAfterEnhancedInputComponent(const FObjectInitializer& ObjectInitializer) {}

void UAfterEnhancedInputComponent::AddInputMappings(
    const UAfterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
    check(InputConfig);
    check(InputSubsystem);
}

void UAfterEnhancedInputComponent::RemoveInputMappings(
    const UAfterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
    check(InputConfig);
    check(InputSubsystem);
}

void UAfterEnhancedInputComponent::RemoveBind(TArray<uint32>& BindHandles)
{
    for (uint32 Handle : BindHandles)
    {
        RemoveBindingByHandle(Handle);
    }
    BindHandles.Reset();
}
