#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "AfterVerbMessage.generated.h"

USTRUCT(BlueprintType)
struct FAfterVerbMessage
{
    GENERATED_BODY();

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    FGameplayTag Verb;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    TObjectPtr<UObject> Instigator = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    TObjectPtr<UObject> Target = nullptr;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    FGameplayTagContainer InstigatorTags;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    FGameplayTagContainer TargetTags;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    FGameplayTagContainer ContextTag;

    UPROPERTY(BlueprintReadWrite, Category = "Gameplay");
    double Magnitude = 1.0;

    AFTER_API FString ToString() const;
};
