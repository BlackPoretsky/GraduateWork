#pragma once

#include "Engine/World.h"
#include "GameplayTagContainer.h"

#include "AfterCameraMode.generated.h"

class AActor;
class UAfterCameraComponent;

UENUM(BlueprintType)
enum class EAfterCameraModeBlendFunction : uint8
{
    Linear,
    EaseIn,
    EaseOut,
    EaseInOut,

    COUNT UMETA(Hidden)
};

struct FAfterCameraModeView
{
public:
    FAfterCameraModeView();

    void Blend(const FAfterCameraModeView& Other, float OtherWeight);

public:
    FVector Location;
    FRotator Rotation;
    FRotator ControlRotation;
    float FieldOfView;
};

/**
 * UAfterCameraMode
 *
 */
UCLASS(Abstract, NotBlueprintable)
class AFTER_API UAfterCameraMode : public UObject
{
    GENERATED_BODY()

public:
    UAfterCameraMode();

    UAfterCameraComponent* GetAfterCameraComponent() const;

    virtual UWorld* GetWorld() const override;

    AActor* GetTargetActor() const;

    const FAfterCameraModeView& GetCameraModeView() const { return View; }

    // Вызывается, когда этот режим камеры активирован в стеке режимов камеры.
    virtual void OnActivation(){};

    // Вызывается, когда этот режим камеры деактивирован в стеке режимов камеры.
    virtual void OnDeactivation(){};

    void UpdateCameraMode(float DeltaTime);

    float GetBlendTime() const { return BlendTime; }
    float GetBlendWeight() const { return BlendWeight; }
    void SetBlendWeight(float Weight);

    FGameplayTag GetCameraTypeTag() const { return CameraTypeTag; }

protected:
    virtual FVector GetPivotLocation() const;
    virtual FRotator GetPivotRotation() const;

    virtual void UpdateView(float DeltaTime);
    virtual void UpdateBlending(float DeltaTime);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Blending")
    FGameplayTag CameraTypeTag;

    FAfterCameraModeView View;

    UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170"))
    float FieldOfView;

    UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-90.0", UIMax = "0.0", ClampMin = "-90.0", ClampMax = "0.0"))
    float ViewPitchMin;

    UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "0.0", UIMax = "90.0", ClampMin = "0.0", ClampMax = "90.0"))
    float ViewPitchMax;

    UPROPERTY(EditDefaultsOnly, Category = "Blending")
    float BlendTime;

    UPROPERTY(EditDefaultsOnly, Category = "Blending")
    EAfterCameraModeBlendFunction BlendFunction;

    UPROPERTY(EditDefaultsOnly, Category = "Blending")
    float BlendExponent;

    float BlendAlpha;

    float BlendWeight;

protected:
    UPROPERTY(Transient)
    uint32 bResetInterpolation : 1;
};

UCLASS()
class UAfterCameraModeStack : public UObject
{
    GENERATED_BODY()

public:
    UAfterCameraModeStack();

    void ActivateStack();
    void DeactivateStack();

    bool IsStackActivate() const { return bIsActive; }

    void PushCameraMode(TSubclassOf<UAfterCameraMode> CameraModeClass);

    bool EvaluateStack(float DeltaTime, FAfterCameraModeView& OutCameraModeView);

    void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer);

protected:
    UAfterCameraMode* GetCameraModeInstance(TSubclassOf<UAfterCameraMode> CameraModeClass);

    void UpdateStack(float DeltaTime);
    void BlendStack(FAfterCameraModeView& OutCameraModeView) const;

protected:
    bool bIsActive;

    UPROPERTY()
    TArray<TObjectPtr<UAfterCameraMode>> CameraModeInstances;

    UPROPERTY()
    TArray<TObjectPtr<UAfterCameraMode>> CameraModeStack;
};