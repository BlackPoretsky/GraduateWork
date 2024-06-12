#pragma once

#include "Camera/AfterCameraMode.h"
#include "Curves/CurveFloat.h"
#include "AfterPenetrationAvoidanceFeeler.h"
#include "AfterCameraMode_ThirdPerson.generated.h"

class UCurveVector;
class AActor;

UCLASS(Abstract, Blueprintable)
class AFTER_API UAfterCameraMode_ThirdPerson : public UAfterCameraMode
{
    GENERATED_BODY()

public:
    UAfterCameraMode_ThirdPerson();

protected:
    virtual void UpdateView(float DeltaTime) override;

    void UpdateForTarget(float DeltaTime);
    void UpdatePreventPenetration(float DeltaTime);
    void PreventCameraPenetration(const AActor& ViewTarget, const FVector& SafeLoc, FVector& CameraLoc, const float& DeltaTime,
        float& DistBlockedPct, bool bSingleRayOnly);

protected:
    /** ������, ������� ���������� �������� � ��������� ������������ �� ����, ��������� ���� ������ ��� ������ ������.*/
    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUserRuntimeFloatCurves"))
    TObjectPtr<const UCurveVector> TargetOffsetCurve;

    /** ���� �������� ����� true, �� ������ TargetOffsetCurve �� ����� �������������� */
    UPROPERTY(EditDefaultsOnly, Category = "Third Person")
    bool bUseRuntimeFloatCurves;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetX;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetY;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetZ;

    /** �������� �������� �������� ��� ����������.*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
    float CrouchOffsetBlendMultiplier = 5.f;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float PenetrationBlendInTime = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float PenetrationBlendOutTime = 0.15f;

    /** ���� �������� ����� true, ��������� �������� �� ������������, ����� �������� ������ �� ������������� � ������ �������. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bPreventPenetration = true;

    /** ���� �������� ����� true, ������� ���������� ����������� ����� � ����������� ������ � �������� ������������. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bDoPredictiveAvoidance = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float CollisionPushOutDistance = 2.f;

    /** ������� �������� ������ �� �� ������������� */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float ReportPenetrationPercent = 0.f;

    /**
     * ��� �������, ������� ������������ ��� ����������� ����� ��������� ������.
     * ������: 0: ��� ������� ������, ������� ������������ ��� �������������� ������������.
     * ������: 1+: ��� ������� ������������, ���� ������������ ������� �������������� ������������, ��� ������ ���������
     *             ������������, ���� ����� ���������� � ��������� ����������� � ���������� � �������, ����� ��� ������������,
     *             ������ ��� ������� ������.
     */
    UPROPERTY(EditDefaultsOnly, Category = "Collision")
    TArray<FAfterPenetrationAvoidanceFeeler> PenetrationAvoidanceFeelers;

    UPROPERTY(Transient)
    float AimLineToDesiredPosBlockedPct;

    UPROPERTY(Transient)
    TArray<TObjectPtr<const AActor>> DebugActorsHitDuringCameraPenetration;

protected:
    void SetTargetCrouchOffset(FVector NewTargetOffset);
    void UpdateCrouchOffset(float DeltaTime);

    FVector InitialCrouchOffset = FVector::ZeroVector;
    FVector TargetCrouchOffset = FVector::ZeroVector;
    float CrouchOffsetBlendPct = 1.0f;
    FVector CurrentCrouchOffset = FVector::ZeroVector;
};
