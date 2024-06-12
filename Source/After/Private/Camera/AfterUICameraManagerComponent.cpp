// Fill out your copyright notice in the Description page of Project Settings.

#include "AfterUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "AfterPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterUICameraManagerComponent)

class AActor;

UAfterUICameraManagerComponent* UAfterUICameraManagerComponent::GetComponent(APlayerController* PC)
{
    if (IsValid(PC))
    {
        if (AAfterPlayerCameraManager* PCCamera = Cast<AAfterPlayerCameraManager>(PC->PlayerCameraManager))
        {
            return PCCamera->GetUICameraComponent();
        }
    }

    return nullptr;
}

UAfterUICameraManagerComponent::UAfterUICameraManagerComponent()
{
    bWantsInitializeComponent = true;
}

void UAfterUICameraManagerComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UAfterUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
    TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

    ViewTarget = InViewTarget;
    CastChecked<AAfterPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UAfterUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
    return false;
}

void UAfterUICameraManagerComponent::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) {}
