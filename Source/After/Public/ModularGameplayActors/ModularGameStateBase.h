#pragma once

#include "GameFramework/GameState.h"
#include "ModularGameStateBase.generated.h"

UCLASS(Blueprintable)
class AFTER_API AModularGameStateBase : public AGameStateBase
{
    GENERATED_BODY()

public:
    //~ Begin AActor interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AActor interface
};

UCLASS(Blueprintable)
class AFTER_API AModularGameState : public AGameState
{
    GENERATED_BODY()

public:
    //~ Begin AActor interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AActor interface

protected:
    //~ Begin AGameState interface
    virtual void HandleMatchHasStarted() override;
    //~ Begin AGameState interface
};
