#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "AfterDamageExecution.generated.h"

class UObject;

/**
 * UAfterDamagExecution
 *
 * Исполнитель используемый эффектами для нанесения урона атрибутам здоровья.
 */
UCLASS()
class UAfterDamageExecution : public UGameplayEffectExecutionCalculation
{
    GENERATED_BODY()

public:
    UAfterDamageExecution();

protected:
    virtual void Execute_Implementation(
        const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const;
};
