#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AfterInputConfig.generated.h"

class UInputAction;

/**
 * FAfterInputAction
 *
 * Структура, используемая для сопоставления действия ввода с тегом.
 */
USTRUCT(BlueprintType)
struct FAfterInputAction
{
    GENERATED_BODY()

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<const UInputAction> InputAction = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
    FGameplayTag InputTag;
};

/**
 * UAfteInputConfig
 *
 * Неизменяемый ресурс данных, содержащий свойства входной конфигурации.
 */
UCLASS(BlueprintType, Const)
class UAfterInputConfig : public UDataAsset
{
    GENERATED_BODY()

public:
    UAfterInputConfig(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintCallable, Category = "After|Pawn")
    const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

    UFUNCTION(BlueprintCallable, Category = "After|Pawn")
    const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
    /**
     * Список действий ввода, используемых владельцем.
     *
     * Эти действия ввода сопоставлены с тегом игрового процесса и должны быть привязаны вручную.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FAfterInputAction> NativeInputAction;

    /**
     * Список действий ввода, используемых владельцем.
     *
     * Эти действия ввода сопоставлены с тегом игрового процесса и
     * автоматически привязываются к способностям с соответствующими входными тегами.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FAfterInputAction> AbilityInputAction;
};
