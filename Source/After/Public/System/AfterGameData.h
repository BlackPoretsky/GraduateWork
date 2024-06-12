#pragma once

#include "Engine/DataAsset.h"
#include "AfterGameData.generated.h"

class UGameplayEffect;
class UObject;

UCLASS(BlueprintType, Const, Meta = (DisplayName = "After Game Data", ShortTooltip = "Data asset containing global game data."))
class UAfterGameData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UAfterGameData();

    static const UAfterGameData& Get();

public:
    UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effect", meta = (DisplayName = "Damage Gameplay Effect (SetByCaller)"))
    TSoftClassPtr<UGameplayEffect> DamageGameplayEffect_SetByCaller;

    UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effect", meta = (DisplayName = "Heal Gameplay Effect (SetByCaller)"))
    TSoftClassPtr<UGameplayEffect> HealGameplayEffect_SetByCaller;

    UPROPERTY(EditDefaultsOnly, Category = "Default Gameplay Effect")
    TSoftClassPtr<UGameplayEffect> DynamicTagGameplayEffect;
};
