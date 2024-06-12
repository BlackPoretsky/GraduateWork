// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/AfterCameraComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "AfterCameraMode.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCameraComponent)

UAfterCameraComponent::UAfterCameraComponent(const FObjectInitializer& ObjectInitilizer) : Super(ObjectInitilizer)
{
    CameraModeStack = nullptr;
    FieldOfViewOffset = 0.f;
}

void UAfterCameraComponent::OnRegister()
{
    Super::OnRegister();

    if (!CameraModeStack)
    {
        CameraModeStack = NewObject<UAfterCameraModeStack>(this);
        check(CameraModeStack)
    }
}

void UAfterCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
    check(CameraModeStack);

    UpdateCameraModes();

    FAfterCameraModeView CameraModeView;
    CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

    if (APawn* TargetPawn = Cast<APawn>(GetTargetActor()))
    {
        if (APlayerController* PC = TargetPawn->GetController<APlayerController>())
        {
            PC->SetControlRotation(CameraModeView.ControlRotation);
        }
    }

    CameraModeView.FieldOfView += FieldOfViewOffset;
    FieldOfViewOffset = 0.f;

    SetWorldLocationAndRotation(CameraModeView.Location, CameraModeView.Rotation);
    FieldOfView = CameraModeView.FieldOfView;

    DesiredView.Location = CameraModeView.Location;
    DesiredView.Rotation = CameraModeView.Rotation;
    DesiredView.FOV = CameraModeView.FieldOfView;
    DesiredView.OrthoWidth = OrthoWidth;
    DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
    DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
    DesiredView.AspectRatio = AspectRatio;
    DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
    DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
    DesiredView.ProjectionMode = ProjectionMode;

    DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
    if (PostProcessBlendWeight > 0.f)
    {
        DesiredView.PostProcessSettings = PostProcessSettings;
    }

    if (IsXRHeadTrackedCamera())
    {
        Super::GetCameraView(DeltaTime, DesiredView);
    }
}

void UAfterCameraComponent::UpdateCameraModes()
{
    check(CameraModeStack);

    if (CameraModeStack->IsStackActivate())
    {
        if (DetermineCameraModeDelegate.IsBound())
        {
            if (const TSubclassOf<UAfterCameraMode> CameraMode = DetermineCameraModeDelegate.Execute())
            {
                CameraModeStack->PushCameraMode(CameraMode);
            }
        }
    }
}

void UAfterCameraComponent::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagTopLayer) const {
    check(CameraModeStack);
    CameraModeStack->GetBlendInfo(OutWeightOfTopLayer, OutTagTopLayer);
}