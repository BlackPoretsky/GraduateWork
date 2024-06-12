#pragma once

#include "Camera/PlayerCameraManager.h"

#include "AfterPlayerCameraManager.generated.h"

class UObject;

#define AFTER_CAMERA_DEFAULT_FOV (90.0f)
#define AFTER_CAMERA_DEFAULT_PITCH_MIN (-85.0f)
#define AFTER_CAMERA_DEFAULT_PITCH_MAX (85.0f)

class UAfterUICameraManagerComponent;

/**
 * AAfterPlayerCameraManagerComponent
 *
 */
UCLASS(NotPlaceable)
class AFTER_API AAfterPlayerCameraManager : public APlayerCameraManager
{
    GENERATED_BODY()

public:
    AAfterPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

    UAfterUICameraManagerComponent* GetUICameraComponent() const;

protected:
    virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

private:
    UPROPERTY(Transient)
    TObjectPtr<UAfterUICameraManagerComponent> UICamera;
};
