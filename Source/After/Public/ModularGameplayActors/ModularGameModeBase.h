#pragma once

#include "GameFramework/GameMode.h"
#include "ModularGameModeBase.generated.h"

/**
 *
 */
UCLASS(Blueprintable)
class AFTER_API AModularGameModeBase : public AGameModeBase
{
    GENERATED_BODY()

public:
    AModularGameModeBase();
};

UCLASS(Blueprintable)
class AFTER_API AModularGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    AModularGameMode();
};
