#pragma once

#include "GameFramework/HUD.h"
#include "AfterHUD.generated.h"

UCLASS(Config = Game)
class AAfterHUD : public AHUD
{
    GENERATED_BODY()

public:
    AAfterHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    //~UObject interface
    virtual void PreInitializeComponents() override;
    //~End of UObject interface

    //~AActor interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor interface
};
