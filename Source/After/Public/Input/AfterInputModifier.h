#pragma once

#include "InputModifiers.h"

#include "AfterInputModifier.generated.h"

/**
 */
UCLASS(NotBlueprintable, MinimalAPI, meta = (DisplayName = "After Aim Inversion Setting"))
class UAfterInputModifier : public UInputModifier
{
    GENERATED_BODY()

protected:
    virtual FInputActionValue ModifyRaw_Implementation(
        const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;
};
