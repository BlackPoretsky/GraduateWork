// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AfterInputConfig.h"

#include "AfterLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterInputConfig)


UAfterInputConfig::UAfterInputConfig(const FObjectInitializer& ObjectInitializer) {}

const UInputAction* UAfterInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FAfterInputAction& Action : NativeInputAction)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogAfter, Error, TEXT("Can't find NativeInputAction for input tag [%s] on InputConfig [%s]"), *InputTag.ToString(),
            *GetNameSafe(this));
    }

    return nullptr;
}

const UInputAction* UAfterInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
    for (const FAfterInputAction& Action : AbilityInputAction)
    {
        if (Action.InputAction && (Action.InputTag == InputTag))
        {
            return Action.InputAction;
        }
    }

    if (bLogNotFound)
    {
        UE_LOG(LogAfter, Error, TEXT("Can't find AbilityInputAction for input tag [%s] on InputConfig [%s]"), *InputTag.ToString(),
            *GetNameSafe(this));
    }

    return nullptr;
}
