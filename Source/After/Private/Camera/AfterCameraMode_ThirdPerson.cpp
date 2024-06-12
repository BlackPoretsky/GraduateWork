#include "AfterCameraMode_ThirdPerson.h"
#include "AfterCameraMode.h"
#include "AfterCameraAssistInterface.h"
#include "AfterPenetrationAvoidanceFeeler.h"

#include "Components/PrimitiveComponent.h"

#include "GameFramework/CameraBlockingVolume.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Character.h"

#include "Curves/CurveVector.h"
#include "Math/RotationMatrix.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCameraMode_ThirdPerson)

namespace AfterCameraMode_ThirdPerson_Statics
{
    static const FName NAME_IgnoreCameraCollision(TEXT("IgnoreCameraCollision"));
}

UAfterCameraMode_ThirdPerson::UAfterCameraMode_ThirdPerson()
{
    TargetOffsetCurve = nullptr;

    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+00.0f, +00.0f, 0.0f), 1.00f, 1.00f, 14.f, 0));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+00.0f, +16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+00.0f, -16.0f, 0.0f), 0.75f, 0.75f, 00.f, 3));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+00.0f, +32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+00.0f, -32.0f, 0.0f), 0.50f, 0.50f, 00.f, 5));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(+20.0f, +00.0f, 0.0f), 1.00f, 1.00f, 00.f, 4));
    PenetrationAvoidanceFeelers.Add(FAfterPenetrationAvoidanceFeeler(FRotator(-20.0f, +00.0f, 0.0f), 0.50f, 0.50f, 00.f, 4));
}

void UAfterCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
    UpdateForTarget(DeltaTime);
    UpdateCrouchOffset(DeltaTime);

    FVector PivotLocation = GetPivotLocation() + CurrentCrouchOffset;
    FRotator PivotRotation = GetPivotRotation();

    PivotRotation.Pitch = FMath::ClampAngle(PivotRotation.Pitch, ViewPitchMin, ViewPitchMax);

    View.Location = PivotLocation;
    View.Rotation = PivotRotation;
    View.ControlRotation = View.Rotation;
    View.FieldOfView = FieldOfView;

    // Прмиенить смещение камеры
    if (!bUseRuntimeFloatCurves)
    {
        if (TargetOffsetCurve)
        {
            const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(PivotRotation.Pitch);
            View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
        }
    }
    else
    {
        FVector TargetOffset(0.0f);

        TargetOffset.X = TargetOffsetX.GetRichCurveConst()->Eval(PivotRotation.Pitch);
        TargetOffset.Y = TargetOffsetY.GetRichCurveConst()->Eval(PivotRotation.Pitch);
        TargetOffset.Z = TargetOffsetZ.GetRichCurveConst()->Eval(PivotRotation.Pitch);

        View.Location = PivotLocation + PivotRotation.RotateVector(TargetOffset);
    }

    UpdatePreventPenetration(DeltaTime);
}

void UAfterCameraMode_ThirdPerson::UpdateForTarget(float DeltaTime)
{
    if (const ACharacter* TargetCharacter = Cast<ACharacter>(GetTargetActor()))
    {
        if (TargetCharacter->bIsCrouched)
        {
            const ACharacter* TargetCharacterCDO = TargetCharacter->GetClass()->GetDefaultObject<ACharacter>();
            const float CrouchedHeightAdjustment = TargetCharacterCDO->CrouchedEyeHeight - TargetCharacterCDO->BaseEyeHeight;

            SetTargetCrouchOffset(FVector(0.0f, 0.0f, CrouchedHeightAdjustment));

            return;
        }
    }

    SetTargetCrouchOffset(FVector::ZeroVector);
}

void UAfterCameraMode_ThirdPerson::UpdatePreventPenetration(float DeltaTime)
{
    if (!bPreventPenetration) return;

    AActor* TargetActor = GetTargetActor();

    APawn* TargetPawn = Cast<APawn>(TargetActor);
    AController* TargetController = TargetPawn ? TargetPawn->GetController() : nullptr;
    IAfterCameraAssistInterface* TargetControllerAssist = Cast<IAfterCameraAssistInterface>(TargetController);

    IAfterCameraAssistInterface* TargetActorsAssist = Cast<IAfterCameraAssistInterface>(TargetActor);

    TOptional<AActor*> OptionalPPTarget =
        TargetActorsAssist ? TargetActorsAssist->GetCameraPreventPenetrationTarget() : TOptional<AActor*>();
    AActor* PPActor = OptionalPPTarget.IsSet() ? OptionalPPTarget.GetValue() : TargetActor;
    IAfterCameraAssistInterface* PPActorAssist = OptionalPPTarget.IsSet() ? Cast<IAfterCameraAssistInterface>(PPActor) : nullptr;

    const UPrimitiveComponent* PPActorRootComponent = Cast<UPrimitiveComponent>(PPActor->GetRootComponent());
    if (PPActorRootComponent)
    {
        // Получаем ближайшую точку на векторе по направлению взгляда от центра капсулы персонажа
        FVector ClosestPointOnLineToCapsuleCenter;
        FVector SafeLocation = PPActor->GetActorLocation();
        FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

        float const PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
        float const MaxHalfHeight = PPActor->GetSimpleCollisionHalfHeight() - PushInDistance;
        SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

        float DistanceSqr;
        PPActorRootComponent->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr, SafeLocation);

        if (PenetrationAvoidanceFeelers.Num() > 0)
        {
            SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
        }

        bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
        PreventCameraPenetration(
            *PPActor, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct, bSingleRayPenetrationCheck);

        IAfterCameraAssistInterface* AssistArray[] = {TargetControllerAssist, TargetActorsAssist, PPActorAssist};

        if (AimLineToDesiredPosBlockedPct < ReportPenetrationPercent)
        {
            for (IAfterCameraAssistInterface* Assist : AssistArray)
            {
                if (Assist)
                {
                    Assist->OnCameraPenetratingTarget();
                }
            }
        }
    }
}

void UAfterCameraMode_ThirdPerson::PreventCameraPenetration(const AActor& ViewTarget, const FVector& SafeLoc, FVector& CameraLoc,
    const float& DeltaTime, float& DistBlockedPct, bool bSingleRayOnly)
{
    float HardBlockedPct = DistBlockedPct;
    float SoftBlockedPct = DistBlockedPct;

    FVector BaseRay = CameraLoc - SafeLoc;
    FRotationMatrix BaseRayMatrix(BaseRay.Rotation());
    FVector BaseRayLocalUp, BaseRayLocalFwd, BaseRayLocalRight;

    BaseRayMatrix.GetScaledAxes(BaseRayLocalFwd, BaseRayLocalRight, BaseRayLocalUp);

    float DistBlockedPctThisFrame = 1.f;

    const int32 NumRaysToShoot = bSingleRayOnly ? FMath::Min(1, PenetrationAvoidanceFeelers.Num()) : PenetrationAvoidanceFeelers.Num();
    FCollisionQueryParams SphereParams(SCENE_QUERY_STAT(CameraPen), false, nullptr);

    SphereParams.AddIgnoredActor(&ViewTarget);

    FCollisionShape SphereShape = FCollisionShape::MakeSphere(0.f);
    UWorld* World = GetWorld();

    for (int32 RayIndex = 0; RayIndex < NumRaysToShoot; ++RayIndex)
    {
        FAfterPenetrationAvoidanceFeeler& Feeler = PenetrationAvoidanceFeelers[RayIndex];
        if (Feeler.FramesUntilNextTrace <= 0)
        {
            FVector RayTarget;
            {
                FVector RotatedRay = BaseRay.RotateAngleAxis(Feeler.AdjustmentRot.Yaw, BaseRayLocalUp);
                RotatedRay = RotatedRay.RotateAngleAxis(Feeler.AdjustmentRot.Pitch, BaseRayLocalRight);
                RayTarget = SafeLoc + RotatedRay;
            }

            SphereShape.Sphere.Radius = Feeler.Extent;
            ECollisionChannel TraceChannel = ECC_Camera;

            FHitResult Hit;
            const bool bHit =
                World->SweepSingleByChannel(Hit, SafeLoc, RayTarget, FQuat::Identity, TraceChannel, SphereShape, SphereParams);

            Feeler.FramesUntilNextTrace = Feeler.TraceInterval;

            const AActor* HitActor = Hit.GetActor();

            if (bHit && IsValid(HitActor))
            {
                bool bIgnoreHit = false;

                if (HitActor->ActorHasTag(AfterCameraMode_ThirdPerson_Statics::NAME_IgnoreCameraCollision))
                {
                    bIgnoreHit = true;
                    SphereParams.AddIgnoredActor(HitActor);
                }

                if (!bIgnoreHit && HitActor->IsA<ACameraBlockingVolume>())
                {
                    const FVector ViewTargetForwardXY = ViewTarget.GetActorForwardVector().GetSafeNormal2D();
                    const FVector ViewTargetLocation = ViewTarget.GetActorLocation();
                    const FVector HitOffset = Hit.Location - ViewTargetLocation;
                    const FVector HitDirectionXY = HitOffset.GetSafeNormal2D();
                    const float DotHitDirection = FVector::DotProduct(ViewTargetForwardXY, HitDirectionXY);
                    if (DotHitDirection > 0.0f)
                    {
                        bIgnoreHit = true;
                        SphereParams.AddIgnoredActor(HitActor);
                    }
                }

                if (!bIgnoreHit)
                {
                    const float Weight = Cast<APawn>(HitActor) ? Feeler.PawnWeight : Feeler.WorldWeight;
                    float NewBlockPct = Hit.Time;
                    NewBlockPct += (1.f - NewBlockPct) * (1.f - Weight);

                    NewBlockPct = ((Hit.Location - SafeLoc).Size() - CollisionPushOutDistance) / (RayTarget - SafeLoc).Size();
                    DistBlockedPctThisFrame = FMath::Min(NewBlockPct, DistBlockedPctThisFrame);

                    Feeler.FramesUntilNextTrace = 0;
                }
            }

            if (RayIndex == 0)
            {
                HardBlockedPct = DistBlockedPctThisFrame;
            }
            else
            {
                SoftBlockedPct = DistBlockedPctThisFrame;
            }
        }
        else
        {
            --Feeler.FramesUntilNextTrace;
        }
    }

    if (bResetInterpolation)
    {
        DistBlockedPct = DistBlockedPctThisFrame;
    }
    else if (DistBlockedPct < DistBlockedPctThisFrame)
    {
        if (PenetrationBlendOutTime > DeltaTime)
        {
            DistBlockedPct = DistBlockedPct + DeltaTime / PenetrationBlendOutTime * (DistBlockedPctThisFrame - DistBlockedPct);
        }
        else
        {
            DistBlockedPct = DistBlockedPctThisFrame;
        }
    }
    else
    {
        if (DistBlockedPct > HardBlockedPct)
        {
            DistBlockedPct = HardBlockedPct;
        }
        else if (DistBlockedPct > SoftBlockedPct)
        {
            if (PenetrationBlendInTime > DeltaTime)
            {
                DistBlockedPct = DistBlockedPct - DeltaTime / PenetrationBlendInTime * (DistBlockedPct - SoftBlockedPct);
            }
            else
            {
                DistBlockedPct = SoftBlockedPct;
            }
        }
    }

    DistBlockedPct = FMath::Clamp<float>(DistBlockedPct, 0.f, 1.0f);
    if (DistBlockedPct < (1.f - ZERO_ANIMWEIGHT_THRESH))
    {
        CameraLoc = SafeLoc + (CameraLoc - SafeLoc) * DistBlockedPct;
    }
}

void UAfterCameraMode_ThirdPerson::SetTargetCrouchOffset(FVector NewTargetOffset)
{
    CrouchOffsetBlendPct = 0.0f;
    InitialCrouchOffset = CurrentCrouchOffset;
    TargetCrouchOffset = NewTargetOffset;
}

void UAfterCameraMode_ThirdPerson::UpdateCrouchOffset(float DeltaTime)
{
    if (CrouchOffsetBlendPct < 1.0f)
    {
        CrouchOffsetBlendPct = FMath::Min(CrouchOffsetBlendPct + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);
        CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendPct, 1.0);
    }
    else
    {
        CurrentCrouchOffset = TargetCrouchOffset;
        CrouchOffsetBlendPct = 1.0f;
    }
}
