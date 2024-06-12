#include "Equipment/AfterEquipmentManagerComponent.h"

#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "Equipment/AfterEquipmentDefinition.h"
#include "Equipment/AfterEquipmentInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterEquipmentManagerComponent)

/////////////////////////////////
// FAfterAppliedEquipmentEntry //
/////////////////////////////////
FString FAfterAppliedEquipmentEntry::GetDebugString() const
{
    return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefintion.Get()));
}

/////////////////////////
// FAfterEquipmentList //
/////////////////////////
UAfterEquipmentInstance* FAfterEquipmentList::AddEntry(TSubclassOf<UAfterEquipmentDefinition> EquipmentDefintion)
{
    UAfterEquipmentInstance* Result = nullptr;

    check(EquipmentDefintion != nullptr);
    check(OwnerComponent);
    check(OwnerComponent->GetOwner()->HasAuthority());

    const UAfterEquipmentDefinition* EquipmentCDO = GetDefault<UAfterEquipmentDefinition>(EquipmentDefintion);

    TSubclassOf<UAfterEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
    if (InstanceType == nullptr)
    {
        InstanceType = UAfterEquipmentInstance::StaticClass();
    }

    FAfterAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
    NewEntry.EquipmentDefintion = EquipmentDefintion;
    NewEntry.Instance = NewObject<UAfterEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);
    Result = NewEntry.Instance;

    if (UAfterAbilitySystemComponent* ASC = GetAbilitySystemComponent())
    {
        for (TObjectPtr<const UAfterAbilitySet> AbilitySet : EquipmentCDO->AbilitySetsToGrant)
        {
            AbilitySet->GiveToAbilitySystem(ASC, &NewEntry.GrantedHandles, Result);
        }
    }

    Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

    return Result;
}

void FAfterEquipmentList::RemoveEntry(UAfterEquipmentInstance* Instace)
{
    for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
    {
        FAfterAppliedEquipmentEntry& Entry = *EntryIt;
        if (Entry.Instance == Instace)
        {
            if (UAfterAbilitySystemComponent* ASC = GetAbilitySystemComponent())
            {
                Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
            }

            Instace->DestroyEquipmentActors();

            EntryIt.RemoveCurrent();
        }
    }
}

UAfterAbilitySystemComponent* FAfterEquipmentList::GetAbilitySystemComponent() const
{
    check(OwnerComponent);

    AActor* OwningActor = OwnerComponent->GetOwner();
    return Cast<UAfterAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

/////////////////////////////////////
// UAfterEquipmentManagerComponent //
/////////////////////////////////////
UAfterEquipmentManagerComponent::UAfterEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer),
      EquipmentList(this)
{
    bWantsInitializeComponent = true;
}

UAfterEquipmentInstance* UAfterEquipmentManagerComponent::EquipItem(TSubclassOf<UAfterEquipmentDefinition> EquipmentClass)
{
    UAfterEquipmentInstance* Result = nullptr;
    if (EquipmentClass != nullptr)
    {
        Result = EquipmentList.AddEntry(EquipmentClass);
        if (Result != nullptr)
        {
            Result->OnEquipped();
        }
    }
    return Result;
}

void UAfterEquipmentManagerComponent::UnequipItem(UAfterEquipmentInstance* ItemInstance)
{
    if (ItemInstance != nullptr)
    {
        ItemInstance->OnUnequipped();
        EquipmentList.RemoveEntry(ItemInstance);
    }
}

void UAfterEquipmentManagerComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UAfterEquipmentManagerComponent::UninitializeComponent()
{
    TArray<UAfterEquipmentInstance*> AllEquipmentInstances;

    for (const FAfterAppliedEquipmentEntry& Entry : EquipmentList.Entries)
    {
        AllEquipmentInstances.Add(Entry.Instance);
    }

    for (UAfterEquipmentInstance* EquipInstance : AllEquipmentInstances)
    {
        UnequipItem(EquipInstance);
    }

    Super::UninitializeComponent();
}

UAfterEquipmentInstance* UAfterEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UAfterEquipmentInstance> InstanceType)
{
    for (const FAfterAppliedEquipmentEntry& Entry : EquipmentList.Entries)
    {
        if (UAfterEquipmentInstance* Instance = Entry.Instance)
        {
            if (Instance->IsA(InstanceType))
            {
                return Instance;
            }
        }
    }

    return nullptr;
}

TArray<UAfterEquipmentInstance*> UAfterEquipmentManagerComponent::GetEquipmentInstancesOfType(
    TSubclassOf<UAfterEquipmentInstance> InstanceType) const
{
    TArray<UAfterEquipmentInstance*> Results;
    for (const FAfterAppliedEquipmentEntry& Entry : EquipmentList.Entries)
    {
        if (UAfterEquipmentInstance* Instance = Entry.Instance)
        {
            if (Instance->IsA(InstanceType))
            {
                Results.Add(Instance);
            }
        }
    }

    return Results;
}
