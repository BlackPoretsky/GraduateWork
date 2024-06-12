#include "AbilitySystem/AfterAbilityTagRelationshipMapping.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilityTagRelationshipMapping)

void UAfterAbilityTagRelationshipMapping::GetAbilityTagsToBlockAndCancel(
    const FGameplayTagContainer& AbilityTags, FGameplayTagContainer* OutTagsToBlock, FGameplayTagContainer* OutTagsToCancel) const
{
    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        const FAfterAbilityTagRelationship& Tags = AbilityTagRelationships[i];
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            if (OutTagsToBlock)
            {
                OutTagsToBlock->AppendTags(Tags.AbilityTagsToBlock);
            }
            if (OutTagsToCancel)
            {
                OutTagsToCancel->AppendTags(Tags.AbilityTagsToCancel);
            }
        }
    }
}

void UAfterAbilityTagRelationshipMapping::GetRequiredAndBlockedActivationTags(const FGameplayTagContainer& AbilityTags,
    FGameplayTagContainer* OutActivationRequired, FGameplayTagContainer* OutActivationBlocked) const
{
    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        const FAfterAbilityTagRelationship& Tags = AbilityTagRelationships[i];
        if (AbilityTags.HasTag(Tags.AbilityTag))
        {
            if (OutActivationRequired)
            {
                OutActivationRequired->AppendTags(Tags.ActivationRequiredTags);
            }
            if (OutActivationBlocked)
            {
                OutActivationBlocked->AppendTags(Tags.ActivationBlockedTags);
            }
        }
    }
}

bool UAfterAbilityTagRelationshipMapping::IsAbilityCancelledByTag(
    const FGameplayTagContainer& AbilityTags, const FGameplayTag& ActionTag) const
{
    for (int32 i = 0; i < AbilityTagRelationships.Num(); i++)
    {
        const FAfterAbilityTagRelationship& Tags = AbilityTagRelationships[i];

        if (Tags.AbilityTag == ActionTag && Tags.AbilityTagsToCancel.HasAny(AbilityTags))
        {
            return true;
        }
    }

    return false;
}
