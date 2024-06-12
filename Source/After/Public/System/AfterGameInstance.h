#pragma once

#include "Engine/GameInstance.h"
#include "AfterGameInstance.generated.h"

UCLASS(Config = Game)
class AFTER_API UAfterGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UAfterGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    //~UGameInstance interface
    virtual void Init() override;
    //~End of UGameInstance interface
};
