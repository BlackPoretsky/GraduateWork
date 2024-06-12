#include "AbilitySystem/AfterTaggedActor.h"
#include "UObject/UnrealType.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterTaggedActor)

AAfterTaggedActor::AAfterTaggedActor(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void AAfterTaggedActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    TagContainer.AppendTags(StaticGameplayTags);
}

#if WITH_EDITOR
bool AAfterTaggedActor::CanEditChange(const FProperty* InProperty) const
{
    if (InProperty->GetName() == GET_MEMBER_NAME_CHECKED(AActor, Tags))
    {
        return false;
    }

    return Super::CanEditChange(InProperty);
}
#endif
