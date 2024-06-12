#pragma once

#include "AIController.h"
#include "ModularAIController.generated.h"

UCLASS(Blueprintable)
class AFTER_API AModularAIController : public AAIController
{
    GENERATED_BODY()

public:
    //~ Begin AActor Interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AActor Interface
};
