#include "AbilitySystem/Executions/AfterDamageExecution.h"
#include "AbilitySystem/Attributes/AfterCombatSet.h"
#include "AbilitySystem/Attributes/AfterHealthSet.h"
#include "AbilitySystem/AfterGameplayEffectContext.h"
#include "AbilitySystem/AfterAbilitySourceInterface.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterDamageExecution)

struct FDamageStatics
{
    FGameplayEffectAttributeCaptureDefinition BaseDamageDef;

    FDamageStatics()
    {
        BaseDamageDef = FGameplayEffectAttributeCaptureDefinition(
            UAfterCombatSet::GetBaseDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
    }
};

static FDamageStatics& DamageStatics()
{
    static FDamageStatics Statics;
    return Statics;
}

UAfterDamageExecution::UAfterDamageExecution()
{
    RelevantAttributesToCapture.Add(DamageStatics().BaseDamageDef);
}

void UAfterDamageExecution::Execute_Implementation(
    const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
    const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
    FAfterGameplayEffectContext* TypedContext = FAfterGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
    check(TypedContext);

    const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
    const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

    FAggregatorEvaluateParameters EvaluateParameters;
    EvaluateParameters.SourceTags = SourceTags;
    EvaluateParameters.TargetTags = TargetTags;

    float BaseDamage = 0.0f;
    ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BaseDamageDef, EvaluateParameters, BaseDamage);

    const float DamageDone = FMath::Max(BaseDamage, 0.0f);

    if (DamageDone > 0.0f)
    {
        OutExecutionOutput.AddOutputModifier(
            FGameplayModifierEvaluatedData(UAfterHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
    }
}
