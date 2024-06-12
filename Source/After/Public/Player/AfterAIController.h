#pragma once

#include "ModularGameplayActors/ModularAIController.h"
#include "AfterAIController.generated.h"

class APlayerState;

UCLASS(Blueprintable)
class AAfterAIController : public AModularAIController
{
    GENERATED_BODY()

public:
    AAfterAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void OnUnPossess() override;
    virtual void Tick(float DeltaTime) override;

private:
    bool bPawnExtinsionComponentInit;
};
