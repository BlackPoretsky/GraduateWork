#pragma once

#include "Engine/World.h"

#include "AfterEquipmentInstance.generated.h"

class AActor;
class APawn;
struct FAfterEquipmentActorToSpawn;

UCLASS(BlueprintType, Blueprintable)
class UAfterEquipmentInstance : public UObject
{
    GENERATED_BODY()

public:
    UAfterEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~UObject interface
    virtual UWorld* GetWorld() const override final;
    //~End of UObject interface

    UFUNCTION(BlueprintPure, Category = "Equipment")
    UObject* GetInstigator() const { return Instigator; }

    void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

    UFUNCTION(BlueprintPure, Category = "Equipment")
    APawn* GetPawn() const;

    UFUNCTION(BlueprintPure, Category = "Equipment", meta = (DeterminesOutputType = "PawnType"))
    APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

    UFUNCTION(BlueprintPure, Category = "Equipment")
    TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

    virtual void SpawnEquipmentActors(const TArray<FAfterEquipmentActorToSpawn>& ActorsToSpawn);
    virtual void DestroyEquipmentActors();

    virtual void OnEquipped();
    virtual void OnUnequipped();

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnEquipped"))
    void K2_OnEquipped();

    UFUNCTION(BlueprintImplementableEvent, Category = "Equipment", meta = (DisplayName = "OnUnequipped"))
    void K2_OnUnequipped();

private:
    UPROPERTY()
    TObjectPtr<UObject> Instigator;

    UPROPERTY()
    TArray<TObjectPtr<AActor>> SpawnedActors;
};
