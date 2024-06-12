// Fill out your copyright notice in the Description page of Project Settings.


#include "AfterPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "AfterCameraComponent.h"
#include "AfterUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterPlayerCameraManager)

static FName UICameraComponentName(TEXT("UICamera"));

AAfterPlayerCameraManager::AAfterPlayerCameraManager(
    const FObjectInitializer& ObjectInitializer)
{
    DefaultFOV = AFTER_CAMERA_DEFAULT_FOV;
    ViewPitchMin = AFTER_CAMERA_DEFAULT_PITCH_MIN;
    ViewPitchMax = AFTER_CAMERA_DEFAULT_PITCH_MAX;

    UICamera = CreateDefaultSubobject<UAfterUICameraManagerComponent>(UICameraComponentName);
}

UAfterUICameraManagerComponent* AAfterPlayerCameraManager::GetUICameraComponent() const
{
    return UICamera;
}

void AAfterPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    if (UICamera->NeedsToUpdateViewTarget())
    {
        Super::UpdateViewTarget(OutVT, DeltaTime);
        UICamera->UpdateViewTarget(OutVT, DeltaTime);
        return;
    }

    Super::UpdateViewTarget(OutVT, DeltaTime);
}
