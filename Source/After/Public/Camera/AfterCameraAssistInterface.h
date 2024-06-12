#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "AfterCameraAssistInterface.generated.h"

UINTERFACE(BlueprintType)
class UAfterCameraAssistInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class IAfterCameraAssistInterface
{
    GENERATED_BODY()

public:
    virtual void GetIgnoreActorsForCameraPenetration(TArray<const AActor*>& OutActorsAllowPenetration) const {};

    virtual TOptional<AActor*> GetCameraPreventPenetrationTarget() const { return TOptional<AActor*>(); }

    virtual void OnCameraPenetratingTarget() {}
};
