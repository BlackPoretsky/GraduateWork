#include "CommonGame/CommonLocalPlayer.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CommonLocalPlayer)

UCommonLocalPlayer::UCommonLocalPlayer(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerControllerSet(FPlayerControllerSetDelegate::FDelegate Delegate)
{
    APlayerController* PC = GetPlayerController(GetWorld());

    if (PC)
    {
        Delegate.Execute(this, PC);
    }

    return OnPlayerControllerSet.Add(Delegate);
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerStateSet(FPlayerStateSetDelegate::FDelegate Delegate)
{
    APlayerController* PC = GetPlayerController(GetWorld());
    APlayerState* PS = PC ? PC->PlayerState : nullptr;

    if (PS)
    {
        Delegate.Execute(this, PS);
    }

    return OnPlayerStateSet.Add(Delegate);
}

FDelegateHandle UCommonLocalPlayer::CallAndRegister_OnPlayerPawnSet(FPlayerPawnSetDelegate::FDelegate Delegate)
{
    APlayerController* PC = GetPlayerController(GetWorld());
    APawn* Pawn = PC ? PC->GetPawn() : nullptr;

    if (Pawn)
    {
        Delegate.Execute(this, Pawn);
    }

    return OnPlayerPawnSet.Add(Delegate);
}

bool UCommonLocalPlayer::GetProjectionData(FViewport* Viewport, FSceneViewProjectionData& ProjectionData, int32 StereoViewIndex) const
{
    if (!bIsPlayerViewEnabled)
    {
        return false;
    }

    return Super::GetProjectionData(Viewport, ProjectionData, StereoViewIndex);
}

UPrimaryGameLayout* UCommonLocalPlayer::GetRootUILayout() const
{
    // TODO: implement recive RootUILayout
    return nullptr;
}
