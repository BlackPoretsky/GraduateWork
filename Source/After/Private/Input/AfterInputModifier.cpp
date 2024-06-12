// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AfterInputModifier.h"

#include "EnhancedPlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterInputModifier)

DEFINE_LOG_CATEGORY_STATIC(LogAfterInputModifiers, Log, All)

/////////////////////////////////////
// UAfterInputModifierAimInversion //
/////////////////////////////////////
FInputActionValue UAfterInputModifier::ModifyRaw_Implementation(
    const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
    
    return FInputActionValue();
}
