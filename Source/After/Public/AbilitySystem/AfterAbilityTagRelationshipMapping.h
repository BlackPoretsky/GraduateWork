#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AfterAbilityTagRelationshipMapping.generated.h"

/** ���������, ������������ ��������� ����� ������� ������ ������������. */
USTRUCT()
struct FAfterAbilityTagRelationship
{
    GENERATED_BODY()

    /** ���, ��� �������� ����������� ���������. */
    UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Gameplay.Action"))
    FGameplayTag AbilityTag;

    /** ���� ������������, ������� ����� ������������� ������������, ������������ ���� ���. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer AbilityTagsToBlock;

    /** ���� ������������, ������� ����� �������� ������������, ������������ ���� ���. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer AbilityTagsToCancel;

    /** ���� ��� ��������� �����������. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer ActivationRequiredTags;

    /** ���� ����������� ��������� �����������. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer ActivationBlockedTags;
};

UCLASS()
class UAfterAbilityTagRelationshipMapping : public UDataAsset
{
    GENERATED_BODY()

public:
    void GetAbilityTagsToBlockAndCancel(
        const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const;
    void GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutActivationRequired,
        FGameplayTagContainer* OutActivationBlocked) const;
    bool IsAbilityCancelledByTag(const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const;

private:
    /** ������ ������ ����� ���������� ������ �������� ��������. */
    UPROPERTY(EditAnywhere, Category = "Ability", meta = (TitleProperty = "AbilityTag"))
    TArray<FAfterAbilityTagRelationship> AbilityTagRelationships;
};
