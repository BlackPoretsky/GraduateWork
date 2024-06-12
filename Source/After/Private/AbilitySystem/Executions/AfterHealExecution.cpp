#include "AbilitySystem/Executions/AfterHealExecution.h"
#include "AbilitySystem/Attributes/AfterCombatSet.h"
#include "AbilitySystem/Attributes/AfterHealthSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterHealExecution)

struct FHealStatics
{
    FGameplayEffectAttributeCaptureDefinition BaseHealDef;

    FHealStatics()
    {
        BaseHealDef = FGameplayEffectAttributeCaptureDefinition(
            UAfterCombatSet::GetBaseHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
    }
};

static FHealStatics& HealStatics()
{
    static FHealStatics Statics;
    return Statics;
}

UAfterHealExecution::UAfterHealExecution()
{
    RelevantAttributesToCapture.Add(HealStatics().BaseHealDef);
}

void UAfterHealExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    float BaseHeal = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealStatics().BaseHealDef, EvaluateParameters, BaseHeal);

    const float HealingDone = FMath::Max(0.0f, BaseHeal);

    if (HealingDone > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(
            FGameplayModifierEvaluatedData(UAfterHealthSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
    }
}
