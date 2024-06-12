#pragma once

#include "GameFramework/Actor.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"

#include "AfterTaggedActor.generated.h"

UCLASS()
class AAfterTaggedActor : public AActor, public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    AAfterTaggedActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~IGameplayTagAssetInterface
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
    //~End of IGameplayTagAssetInterface

#if WITH_EDITOR
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Actor")
    FGameplayTagContainer StaticGameplayTags;
};
