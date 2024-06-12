#pragma once

#include "ModularGameplayActors/ModularPlayerController.h"
#include "CommonPlayerController.generated.h"

class APawn;

UCLASS(Config = Game)
class AFTER_API ACommonPlayerController : public AModularPlayerController
{
    GENERATED_BODY()

public:
    ACommonPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void ReceivedPlayer() override;
    virtual void SetPawn(APawn* InPawn) override;
    virtual void OnPossess(APawn* APawn) override;
    virtual void OnUnPossess() override;

protected:
    virtual void OnRep_PlayerState() override;
};
