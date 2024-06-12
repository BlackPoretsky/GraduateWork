#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "CommonLocalPlayer.generated.h"

class APawn;
class APlayerController;
class APlayerState;
class UPrimaryGameLayout;

/**
 *
 */
UCLASS(Config = Engine, Transient)
class AFTER_API UCommonLocalPlayer : public ULocalPlayer
{
    GENERATED_BODY()

public:
    UCommonLocalPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    DECLARE_MULTICAST_DELEGATE_TwoParams(
        FPlayerControllerSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerController* PlayerController);
    FPlayerControllerSetDelegate OnPlayerControllerSet;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateSetDelegate, UCommonLocalPlayer* LocalPlayer, APlayerState* PlayerState);
    FPlayerStateSetDelegate OnPlayerStateSet;

    DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerPawnSetDelegate, UCommonLocalPlayer* LocalPlayer, APawn* Pawn);
    FPlayerPawnSetDelegate OnPlayerPawnSet;

    FDelegateHandle CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate);
    FDelegateHandle CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate);
    FDelegateHandle CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate);

public:
    virtual bool GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const override;

    bool IsPlayerViewEnabled() const { return bIsPlayerViewEnabled; }
    void SetIsPlayerViewEnabled(bool bInIsPlayerViewEnabled) { bIsPlayerViewEnabled = bInIsPlayerViewEnabled; }

    UPrimaryGameLayout* GetRootUILayout() const;

private:
    bool bIsPlayerViewEnabled = true;
};
