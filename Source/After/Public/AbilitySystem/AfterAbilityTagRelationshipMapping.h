#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "AfterAbilityTagRelationshipMapping.generated.h"

/** —труктура, определ€юща€ отношени€ между разными тегами способностей. */
USTRUCT()
struct FAfterAbilityTagRelationship
{
    GENERATED_BODY()

    /** “ег, дл€ которого определ€тс€ отношени€. */
    UPROPERTY(EditAnywhere, Category = "Ability", meta = (Categories = "Gameplay.Action"))
    FGameplayTag AbilityTag;

    /** “еги способностей, которые будут заблокированы способностью, использующей этот тег. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer AbilityTagsToBlock;

    /** “еги способностей, которые будут отменены способностью, использующей этот тег. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer AbilityTagsToCancel;

    /** “еги дл€ активации способности. */
    UPROPERTY(EditAnywhere, Category = "Ability")
    FGameplayTagContainer ActivationRequiredTags;

    /** “еги блокирующие активацию способности. */
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
    /** —писок св€зей между различными тегами игрового процесса. */
    UPROPERTY(EditAnywhere, Category = "Ability", meta = (TitleProperty = "AbilityTag"))
    TArray<FAfterAbilityTagRelationship> AbilityTagRelationships;
};
