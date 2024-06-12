#pragma once

#include "Engine/DataAsset.h"
#include "AfterPawnData.generated.h"

class APawn;
class UAfterAbilitySet;
class UAfterAbilityTagRelationshipMapping;
class UAfterCameraMode;
class UAfterInputConfig;

UCLASS(BlueprintType, Const, meta = (DisplayName = "After Pawn Data", ShortToolTip = "Data asset used to define a Pawn."))
class AFTER_API UAfterPawnData : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UAfterPawnData(const FObjectInitializer& ObjectInitializer);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Pawn")
    TSubclassOf<APawn> PawnClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Abilities")
    TArray<TObjectPtr<UAfterAbilitySet>> AbilitySets;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Abilities")
    TObjectPtr<UAfterAbilityTagRelationshipMapping> TagRelationshipMapping;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Input")
    TObjectPtr<UAfterInputConfig> InputConfig;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "After|Camera")
    TSubclassOf<UAfterCameraMode> DefaultCameraMode;
};
