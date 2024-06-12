#include "Inventory/Pickupable.h"

#include "GameFramework/Actor.h"
#include "Inventory/AfterInventoryManagerComponent.h"
#include "UObject/ScriptInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Pickupable)

UPickupbleStatics::UPickupbleStatics()
    : Super(FObjectInitializer::Get())
{
}

TScriptInterface<IPickupable> UPickupbleStatics::GetFirstPickupableFromActor(AActor* Actor)
{
    TScriptInterface<IPickupable> PickupableActor(Actor);
    if (PickupableActor)
    {
        return PickupableActor;
    }

    TArray<UActorComponent*> PickupableComponents =
        Actor ? Actor->GetComponentsByInterface(UPickupable::StaticClass()) : TArray<UActorComponent*>();
    if (PickupableComponents.Num() > 0)
    {
        return TScriptInterface<IPickupable>(PickupableComponents[0]);
    }

    return TScriptInterface<IPickupable>();
}

void UPickupbleStatics::AddPickupToInventory(UAfterInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup)
{
    if (InventoryComponent && Pickup)
    {
        const FInventoryPickup& PickupInventory = Pickup->GetPickupInventory();

        for (const FPickupTemplate& Template : PickupInventory.Templates)
        {
            InventoryComponent->AddItemDefinition(Template.ItemDef, Template.StackCount);
        }

        for (const FPickupInstance& Instance : PickupInventory.Instances)
        {
            InventoryComponent->AddItemInstance(Instance.Item);
        }
    }
}
