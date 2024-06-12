#include "Equipment/AfterEquipmentInstance.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Equipment/AfterEquipmentDefinition.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterEquipmentInstance)

UAfterEquipmentInstance::UAfterEquipmentInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

UWorld* UAfterEquipmentInstance::GetWorld() const
{
    if (APawn* OwningPawn = GetPawn()) return OwningPawn->GetWorld();

    return nullptr;
}

APawn* UAfterEquipmentInstance::GetPawn() const
{
    return Cast<APawn>(GetOuter());
}

APawn* UAfterEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
    APawn* Result = nullptr;
    if (UClass* ActualPawnType = PawnType)
    {
        if (GetOuter()->IsA(ActualPawnType))
        {
            Result = Cast<APawn>(GetOuter());
        }
    }

    return Result;
}

void UAfterEquipmentInstance::SpawnEquipmentActors(const TArray<FAfterEquipmentActorToSpawn>& ActorsToSpawn)
{
    if (APawn* OwningPawn = GetPawn())
    {
        USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
        if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
        {
            AttachTarget = Char->GetMesh();
        }

        for (const FAfterEquipmentActorToSpawn& SpawnInfo : ActorsToSpawn)
        {
            AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
            NewActor->FinishSpawning(FTransform::Identity, true);
            NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
            NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

            SpawnedActors.Add(NewActor);
        }
    }
}

void UAfterEquipmentInstance::DestroyEquipmentActors()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
}

void UAfterEquipmentInstance::OnEquipped()
{
    K2_OnEquipped();
}

void UAfterEquipmentInstance::OnUnequipped()
{
    K2_OnUnequipped();
}
