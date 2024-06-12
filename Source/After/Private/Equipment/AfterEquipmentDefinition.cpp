#include "Equipment/AfterEquipmentDefinition.h"
#include "Equipment/AfterEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterEquipmentDefinition)

UAfterEquipmentDefinition::UAfterEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InstanceType = UAfterEquipmentInstance::StaticClass();
}
