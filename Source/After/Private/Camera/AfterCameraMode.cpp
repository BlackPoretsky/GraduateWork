// Fill out your copyright notice in the Description page of Project Settings.

#include "AfterCameraMode.h"

#include "Components/CapsuleComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Character.h"
#include "AfterCameraComponent.h"
#include "AfterPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCameraMode)

/////////////////////////
// FAfterCameraModeView//
/////////////////////////
FAfterCameraModeView::FAfterCameraModeView()
    : Location(ForceInit),
      Rotation(ForceInit),
      ControlRotation(ForceInit),
      FieldOfView(AFTER_CAMERA_DEFAULT_FOV)
{
}

void FAfterCameraModeView::Blend(const FAfterCameraModeView& Other, float OtherWeight)
{
    if (OtherWeight <= 0.0f)
    {
        return;
    }
    else if (OtherWeight >= 1.0f)
    {
        *this = Other;
        return;
    }

    Location = FMath::Lerp(Location, Other.Location, OtherWeight);

    const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
    Rotation = Rotation + (OtherWeight * DeltaRotation);

    const FRotator DeltaControlRotation = (Other.ControlRotation - ControlRotation).GetNormalized();
    ControlRotation = ControlRotation + (OtherWeight * DeltaControlRotation);

    FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

////////////////////
// UAfterCameraMode//
////////////////////
UAfterCameraMode::UAfterCameraMode()
{
    FieldOfView = AFTER_CAMERA_DEFAULT_FOV;
    ViewPitchMin = AFTER_CAMERA_DEFAULT_PITCH_MIN;
    ViewPitchMax = AFTER_CAMERA_DEFAULT_PITCH_MAX;

    BlendTime = 0.5f;
    BlendFunction = EAfterCameraModeBlendFunction::EaseOut;
    BlendExponent = 4.0f;
    BlendAlpha = 1.0f;
    BlendWeight = 1.0f;
}

UAfterCameraComponent* UAfterCameraMode::GetAfterCameraComponent() const
{
    return CastChecked<UAfterCameraComponent>(GetOuter());
}

UWorld* UAfterCameraMode::GetWorld() const
{
    return HasAnyFlags(RF_ClassDefaultObject) ? nullptr : GetOuter()->GetWorld();
}

AActor* UAfterCameraMode::GetTargetActor() const
{
    const UAfterCameraComponent* AfteCameraComponent = GetAfterCameraComponent();

    return AfteCameraComponent->GetTargetActor();
}

FVector UAfterCameraMode::GetPivotLocation() const
{
    const AActor* TargetActor = GetTargetActor();
    check(TargetActor);

    if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
    {
        if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetPawn))
        {
            const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
            check(TargetCharacterCDO);

            const UCapsuleComponent* CapsuleComp = TargetCharacterCDO->GetCapsuleComponent();
            check(CapsuleComp);

            const UCapsuleComponent* CapsuleCompCDO = TargetCharacterCDO->GetCapsuleComponent();
            check(CapsuleCompCDO);

            const float DefaultHalfHeight = CapsuleCompCDO->GetUnscaledCapsuleHalfHeight();
            const float ActualHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
            const float HeightAdjustment = (DefaultHalfHeight - ActualHalfHeight) + TargetCharacterCDO->BaseEyeHeight;

            return TargetCharacter->GetActorLocation() + (FVector::UpVector * HeightAdjustment);
        }

        return TargetPawn->GetPawnViewLocation();
    }

    return TargetActor->GetActorLocation();
}

FRotator UAfterCameraMode::GetPivotRotation() const
{
    const AActor* TargetActor = GetTargetActor();
    check(TargetActor);

    if (const APawn* TargetPawn = Cast<APawn>(TargetActor))
    {
        return TargetPawn->GetViewRotation();
    }

    return TargetActor->GetActorRotation();
}

void UAfterCameraMode::UpdateCameraMode(float DeltaTime)
{
    UpdateView(DeltaTime);
    UpdateBlending(DeltaTime);
}

void UAfterCameraMode::UpdateView(float DeltaTime)
{
    FVector PivotLocation = GetPivotLocation();
    FRotator PivotRotation = GetPivotRotation();

    PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

    View.Location = PivotLocation;
    View.Rotation = PivotRotation;
    View.ControlRotation = View.Rotation;
    View.FieldOfView = FieldOfView;
}

void UAfterCameraMode::SetBlendWeight(float Weight)
{
    BlendWeight = FMath::Clamp(Weight, 0.0f, 1.0f);

    const float InvExponent = (BlendExponent > 0.0f) ? (1.0f / BlendExponent) : 1.0f;

    switch (BlendFunction)
    {
        case EAfterCameraModeBlendFunction::Linear:
            BlendAlpha = BlendWeight;
            break;
        case EAfterCameraModeBlendFunction::EaseIn:
            BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, InvExponent);
            break;
        case EAfterCameraModeBlendFunction::EaseOut:
            BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, InvExponent);
            break;
        case EAfterCameraModeBlendFunction::EaseInOut:
            BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, InvExponent);
            break;

        default:
            checkf(false, TEXT("SetBlendWeight: Invalid BlendFunction [%d]\n"), (uint8)BlendFunction);
            break;
    }
}

void UAfterCameraMode::UpdateBlending(float DeltaTime)
{
    if (BlendTime > 0.0f)
    {
        BlendAlpha += (DeltaTime / BlendTime);
        BlendAlpha = FMath::Min(BlendAlpha, 1.0f);
    }
    else
    {
        BlendAlpha = 1.0f;
    }

    const float Exponent = (BlendExponent > 0.0f) ? BlendExponent : 1.0f;

    switch (BlendFunction)
    {
        case EAfterCameraModeBlendFunction::Linear:
            BlendWeight = BlendAlpha;
            break;
        case EAfterCameraModeBlendFunction::EaseIn:
            BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, Exponent);
            break;
        case EAfterCameraModeBlendFunction::EaseOut:
            BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, Exponent);
            break;
        case EAfterCameraModeBlendFunction::EaseInOut:
            BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, Exponent);
            break;

        default:
            checkf(false, TEXT("UpdateBlending: Invalid BlendFunction [%d]"), (uint8)BlendFunction);
            break;
    }
}

//////////////////////////
// UAfterCameraModeStack//
//////////////////////////
UAfterCameraModeStack::UAfterCameraModeStack()
{
    bIsActive = true;
}

void UAfterCameraModeStack::ActivateStack()
{
    if (!bIsActive)
    {
        bIsActive = true;

        for (UAfterCameraMode* CameraMode : CameraModeStack)
        {
            check(CameraMode);
            CameraMode->OnActivation();
        }
    }
}

void UAfterCameraModeStack::DeactivateStack()
{
    if (bIsActive)
    {
        bIsActive = false;

        for (UAfterCameraMode* CameraMode : CameraModeStack)
        {
            check(CameraMode);
            CameraMode->OnDeactivation();
        }
    }
}

void UAfterCameraModeStack::PushCameraMode(TSubclassOf<UAfterCameraMode> CameraModeClass)
{
    if (!CameraModeClass) return;

    UAfterCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
    check(CameraMode);

    int32 StackSize = CameraModeStack.Num();

    // Уже находится на вершине стека.
    if ((StackSize > 0) && (CameraModeStack[0] == CameraMode)) return;

    // Посмотреть, есть ли он уже в стеке, если есть то удалить его.
    int32 ExistingStackIndex = INDEX_NONE;
    float ExistingStackContribution = 1.0f;

    for (int32 StackIndex = 0; StackIndex < StackSize; ++StackSize)
    {
        if (CameraModeStack[StackIndex] == CameraMode)
        {
            ExistingStackIndex = StackIndex;
            ExistingStackContribution *= CameraMode->GetBlendWeight();
            break;
        }
        else
        {
            ExistingStackContribution *= (1.0f - CameraModeStack[StackIndex]->GetBlendWeight());
        }
    }

    if (ExistingStackIndex != INDEX_NONE)
    {
        CameraModeStack.RemoveAt(ExistingStackIndex);
        StackSize--;
    }
    else
    {
        ExistingStackContribution = 0.0f;
    }

    // Определить, начальный вес.
    const bool bShouldBlend = ((CameraMode->GetBlendTime() > 0.0f) && (StackSize > 0));
    const float BlendWeight = (bShouldBlend ? ExistingStackContribution : 1.0f);

    CameraMode->SetBlendWeight(BlendWeight);

    // Добавьте в начало стека.
    CameraModeStack.Insert(CameraMode, 0);

    // Последний элемент всегда имеет вес 100%.
    CameraModeStack.Last()->SetBlendWeight(1.0f);

    // Сообщаем режиму камеры, добавляется ли он в стек.
    if (ExistingStackIndex == INDEX_NONE)
    {
        CameraMode->OnActivation();
    }
}

bool UAfterCameraModeStack::EvaluateStack(float DeltaTime, FAfterCameraModeView& OutCameraModeView)
{
    if (!bIsActive) return false;

    UpdateStack(DeltaTime);
    BlendStack(OutCameraModeView);

    return true;
}

UAfterCameraMode* UAfterCameraModeStack::GetCameraModeInstance(TSubclassOf<UAfterCameraMode> CameraModeClass)
{
    check(CameraModeClass);

    for (UAfterCameraMode* CameraMode : CameraModeInstances)
    {
        if (IsValid(CameraMode) && (CameraMode->GetClass() == CameraModeClass))
        {
            return CameraMode;
        }
    }

    UAfterCameraMode* NewCameraMode = NewObject<UAfterCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
    check(NewCameraMode);

    CameraModeInstances.Add(NewCameraMode);

    return NewCameraMode;
}

void UAfterCameraModeStack::UpdateStack(float DeltaTime)
{
    const int32 StackSize = CameraModeStack.Num();
    if (StackSize <= 0) return;

    int32 RemoveCount = 0;
    int32 RemoveIndex = INDEX_NONE;

    for (int32 StackIndex = 0; StackIndex < StackSize; ++StackIndex)
    {
        UAfterCameraMode* CameraMode = CameraModeStack[StackIndex];
        check(CameraMode);

        CameraMode->UpdateCameraMode(DeltaTime);

        if (CameraMode->GetBlendWeight() >= 1.0f)
        {
            RemoveIndex = (StackIndex + 1);
            RemoveCount = (StackSize - RemoveIndex);
            break;
        }
    }

    if (RemoveCount > 0)
    {
        for (int32 StackIndex = RemoveIndex; StackIndex < StackSize; ++StackIndex)
        {
            UAfterCameraMode* CameraMode = CameraModeStack[StackIndex];
            check(CameraMode);

            CameraMode->OnDeactivation();
        }

        CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
    }
}

void UAfterCameraModeStack::BlendStack(FAfterCameraModeView& OutCameraModeView) const
{
    const int32 StackSize = CameraModeStack.Num();
    if (StackSize <= 0) return;

    const UAfterCameraMode* CameraMode = CameraModeStack[StackSize - 1];
    check(CameraMode);

    OutCameraModeView = CameraMode->GetCameraModeView();

    for (int32 StackIndex = (StackSize - 2); StackIndex >= 0; --StackIndex)
    {
        CameraMode = CameraModeStack[StackIndex];
        check(CameraMode);

        OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
    }
}

void UAfterCameraModeStack::GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer)
{
    if (CameraModeStack.Num() == 0)
    {
        OutWeightOfTopLayer = 1.0f;
        OutTagOfTopLayer = FGameplayTag();
        return;
    }
    else
    {
        UAfterCameraMode* TopEntry = CameraModeStack.Last();
        check(TopEntry);
        OutWeightOfTopLayer = TopEntry->GetBlendWeight();
        OutTagOfTopLayer = TopEntry->GetCameraTypeTag();
    }
}
