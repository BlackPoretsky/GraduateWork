#include "Cosmetics/AfterCosmeticAnimationTypes.h"

#include "Animation/AnimInstance.h"
#include "Engine/SkeletalMesh.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCosmeticAnimationTypes)

TSubclassOf<UAnimInstance> FAfterAnimLayerSelectionSet::SelectBestLayer(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FAfterAnimLayerSelectionEntry& Rule : LayerRules)
    {
        if ((Rule.Layer != nullptr) && (CosmeticTags.HasAll(Rule.RequiredTags)))
        {
            return Rule.Layer;
        }
    }

    return DefaultLayer;
}

USkeletalMesh* FAfterAnimBodyStyleSelectionSet::SelectBestBodyStyle(const FGameplayTagContainer& CosmeticTags) const
{
    for (const FAfterAnimBodyStyleSelectionEntry& Rule : MeshRules)
    {
        if ((Rule.Mesh != nullptr) && (CosmeticTags.HasAll(Rule.RequiredTags)))
        {
            return Rule.Mesh;
        }
    }

    return DefaultMesh;
}
