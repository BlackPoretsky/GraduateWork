#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "AfterHealExecution.generated.h"

class UObject;

/**
 * UAfterHealExecution
 *
 * Исполнитель, используемый игровыми эффектами для исцеления атрибутов здоровья.
 */
UCLASS()
class UAfterHealExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UAfterHealExecution();

protected:
    virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
        FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
