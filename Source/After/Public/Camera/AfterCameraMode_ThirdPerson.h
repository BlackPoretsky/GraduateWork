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
    /**  рива€, котора€ определ€ет смещени€ в локальном пространстве от цели, использу€ угол обзора дл€ оценки кривой.*/
    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "!bUserRuntimeFloatCurves"))
    TObjectPtr<const UCurveVector> TargetOffsetCurve;

    /** ≈сли значение равно true, то крива€ TargetOffsetCurve не будет использоватьс€ */
    UPROPERTY(EditDefaultsOnly, Category = "Third Person")
    bool bUseRuntimeFloatCurves;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetX;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetY;

    UPROPERTY(EditDefaultsOnly, Category = "Third Person", Meta = (EditCondition = "bUserRuntimeFloatCurves"))
    FRuntimeFloatCurve TargetOffsetZ;

    /** »змен€ет скорость смещени€ при приседании.*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Third Person")
    float CrouchOffsetBlendMultiplier = 5.f;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float PenetrationBlendInTime = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float PenetrationBlendOutTime = 0.15f;

    /** ≈сли значение равно true, выполн€ет проверку на столкновение, чтобы защитить камеру от проникновени€ в другие объекты. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bPreventPenetration = true;

    /** ≈сли значение равно true, пробует обнаружить близлежащие стены и переместить камеру в ожидании столкновени€. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    bool bDoPredictiveAvoidance = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float CollisionPushOutDistance = 2.f;

    /** ѕроцент смещени€ камеры из за проникновени€ */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collision")
    float ReportPenetrationPercent = 0.f;

    /**
     * Ёто датчики, которые используютс€ дл€ определени€ места установки камеры.
     * »ндекс: 0: Ёто обычный датчик, который используетс€ дл€ предотвращени€ столкновений.
     * »ндекс: 1+: Ёти датчики используютс€, если используетс€ функци€ предсказывани€ столкновени€, дл€ поиска возможных
     *             столкновений, если игрок повернетс€ в указанном направлении и столкнетс€ с камерой, чтобы она приблизилась,
     *             прежде чем заденет объект.
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
