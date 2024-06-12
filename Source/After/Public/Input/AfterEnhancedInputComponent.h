#pragma once

#include "EnhancedInputComponent.h"
#include "AfterInputConfig.h"

#include "AfterEnhancedInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;

/**
 * UAfterInputComponent
 *
 */
UCLASS(Config = Input)
class AFTER_API UAfterEnhancedInputComponent : public UEnhancedInputComponent
{
    GENERATED_BODY()

public:
    UAfterEnhancedInputComponent(const FObjectInitializer& ObjectInitializer);

    void AddInputMappings(const UAfterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
    void RemoveInputMappings(const UAfterInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

    template <class UserClass, typename FuncType>
    void BindNativeAction(const UAfterInputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggeredEvent,
        UserClass* Object, FuncType Func, bool bLogIfNotFound);

    template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
    void BindAbilityActions(const UAfterInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
        ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

    void RemoveBind(TArray<uint32>& BindHandles);
};

template <class UserClass, typename FuncType>
void UAfterEnhancedInputComponent::BindNativeAction(const UAfterInputConfig* InputConfig, const FGameplayTag& InputTag,
    ETriggerEvent TriggeredEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
    check(InputConfig);

    if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
    {
        BindAction(IA, TriggeredEvent, Object, Func);
    }
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UAfterEnhancedInputComponent::BindAbilityActions(const UAfterInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
    ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
    check(InputConfig);

    for (const FAfterInputAction& Action : InputConfig->AbilityInputAction)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            if (PressedFunc)
            {
                BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
            }

            if (ReleasedFunc)
            {
                BindHandles.Add(
                    BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
            }
        }
    }
}