#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "AfterDamageExecution.generated.h"

class UObject;

/**
 * UAfterDamagExecution
 *
 * ����������� ������������ ��������� ��� ��������� ����� ��������� ��������.
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
