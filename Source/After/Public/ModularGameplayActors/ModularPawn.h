#pragma once

#include "GameFramework/Pawn.h"
#include "ModularPawn.generated.h"

UCLASS(Blueprintable)
class AFTER_API AModularPawn : public APawn
{
    GENERATED_BODY()

public:
    //~ Begin AActor interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~ End AActor interface
};
