#pragma once

#include "AfterEquipmentDefinition.generated.h"

class AActor;
class UAfterAbilitySet;
class UAfterEquipmentInstance;

USTRUCT()
struct FAfterEquipmentActorToSpawn
{
    GENERATED_BODY()

    FAfterEquipmentActorToSpawn() {}

    UPROPERTY(EditAnywhere, Category = "Equipment")
    TSubclassOf<AActor> ActorToSpawn;

    UPROPERTY(EditAnywhere, Category = "Equipment")
    FName AttachSocket;

    UPROPERTY(EditAnywhere, Category = "Equipment")
    FTransform AttachTransform;
};

UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UAfterEquipmentDefinition : public UObject
{
    GENERATED_BODY()

public:
    UAfterEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TSubclassOf<UAfterEquipmentInstance> InstanceType;

    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TArray<TObjectPtr<const UAfterAbilitySet>> AbilitySetsToGrant;

    UPROPERTY(EditDefaultsOnly, Category = "Equipment")
    TArray<FAfterEquipmentActorToSpawn> ActorsToSpawn;
};
