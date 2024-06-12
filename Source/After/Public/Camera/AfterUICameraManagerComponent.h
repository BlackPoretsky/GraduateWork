#pragma once

#include "Components/ActorComponent.h"

#include "AfterUICameraManagerComponent.generated.h"

class AAfterPlayerCameraManager;

class AActor;
class AHUD;
class APlayerController;
class UCanvas;
class UObject;

UCLASS(Transient, Within = AfterPlayerCameraManager)
class UAfterUICameraManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    static UAfterUICameraManagerComponent* GetComponent(APlayerController* PC);

public:
    UAfterUICameraManagerComponent();
    virtual void InitializeComponent() override;

    bool IsSettingViewTarget() const { return bUpdatingViewTarget; }
    AActor* GetViewTarget() const { return ViewTarget; }
    void SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams = FViewTargetTransitionParams());

    bool NeedsToUpdateViewTarget() const;
    void UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime);

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> ViewTarget;

    UPROPERTY(Transient)
    bool bUpdatingViewTarget;
};
