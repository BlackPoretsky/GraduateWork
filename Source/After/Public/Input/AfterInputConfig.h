#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AfterInputConfig.generated.h"

class UInputAction;

/**
 * FAfterInputAction
 *
 * ���������, ������������ ��� ������������� �������� ����� � �����.
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
 * ������������ ������ ������, ���������� �������� ������� ������������.
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
     * ������ �������� �����, ������������ ����������.
     *
     * ��� �������� ����� ������������ � ����� �������� �������� � ������ ���� ��������� �������.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FAfterInputAction> NativeInputAction;

    /**
     * ������ �������� �����, ������������ ����������.
     *
     * ��� �������� ����� ������������ � ����� �������� �������� �
     * ������������� ������������� � ������������ � ���������������� �������� ������.
     */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
    TArray<FAfterInputAction> AbilityInputAction;
};
