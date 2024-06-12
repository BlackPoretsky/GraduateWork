#include "ModularGameplayActors/ModularGameModeBase.h"

#include "ModularGameplayActors/ModularGameStateBase.h"
#include "ModularGameplayActors/ModularPlayerController.h"
#include "ModularGameplayActors/ModularPawn.h"
#include "ModularGameplayActors/ModularPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularGameModeBase)

AModularGameModeBase::AModularGameModeBase()
{
    GameStateClass = AModularGameStateBase::StaticClass();
    PlayerControllerClass = AModularPlayerController::StaticClass();
    PlayerStateClass = AModularPlayerState::StaticClass();
    DefaultPawnClass = AModularPawn::StaticClass();
}

AModularGameMode::AModularGameMode()
{
    GameStateClass = AModularGameState::StaticClass();
    PlayerControllerClass = AModularPlayerController::StaticClass();
    PlayerStateClass = AModularPlayerState::StaticClass();
    DefaultPawnClass = AModularPawn::StaticClass();
}
