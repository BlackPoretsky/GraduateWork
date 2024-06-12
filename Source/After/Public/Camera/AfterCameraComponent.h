#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "AfterCameraComponent.generated.h"

class UAfterCameraMode;
class UAfterCameraModeStack;

struct FGameplayTag;
struct FMinimalViewInfo;

DECLARE_DELEGATE_RetVal(TSubclassOf<UAfterCameraMode>, FAfterCameraModeDelegate);

/**
 * UAfterCameraComponent
 *
 */
UCLASS()
class AFTER_API UAfterCameraComponent : public UCameraComponent
{
    GENERATED_BODY()

public:
    UAfterCameraComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "After|Camera")
    static UAfterCameraComponent* FindCameraComponent(const AActor* Actor)
    {
        return (Actor ? Actor->FindComponentByClass<UAfterCameraComponent>() : nullptr);
    };

    /** ���������� ����� � �������� ������������ ������.*/
    virtual AActor* GetTargetActor() const { return GetOwner(); }

    /** �������, ������������ ��� ������� ���������� ������ ������ ������.*/
    FAfterCameraModeDelegate DetermineCameraModeDelegate;

    /** ��������� �������� ��� FOV. */
    void AddFielOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

    void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

protected:
    virtual void OnRegister() override;
    virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

    virtual void UpdateCameraModes();

private:
    /** ����, ������������ ��� ���������� ������� ������. */
    UPROPERTY()
    TObjectPtr<UAfterCameraModeStack> CameraModeStack;

    /** ��������, ����������� � FOV. */
    float FieldOfViewOffset;
};
