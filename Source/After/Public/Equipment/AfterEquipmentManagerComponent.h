#pragma once

#include "AbilitySystem/AfterAbilitySet.h"
#include "Components/PawnComponent.h"

#include "AfterEquipmentManagerComponent.generated.h"

class UActorComponent;
class UAfterAbilitySystemComponent;
class UAfterEquipmentDefinition;
class UAfterEquipmentInstance;
class UAfterEquipmentManagerComponent;

USTRUCT(BlueprintType)
struct FAfterAppliedEquipmentEntry
{
    GENERATED_BODY()

    FAfterAppliedEquipmentEntry() {}

    FString GetDebugString() const;

private:
    friend FAfterEquipmentList;
    friend UAfterEquipmentManagerComponent;

    UPROPERTY()
    TSubclassOf<UAfterEquipmentDefinition> EquipmentDefintion;

    UPROPERTY()
    TObjectPtr<UAfterEquipmentInstance> Instance = nullptr;

    UPROPERTY()
    FAfterAbilitySet_GrantedHandles GrantedHandles;
};

USTRUCT(BlueprintType)
struct FAfterEquipmentList
{
    GENERATED_BODY()

    FAfterEquipmentList()
        : OwnerComponent(nullptr)
    {
    }

    FAfterEquipmentList(UActorComponent* InOwnerComponent)
        : OwnerComponent(InOwnerComponent)
    {
    }

    UAfterEquipmentInstance* AddEntry(TSubclassOf<UAfterEquipmentDefinition> EquipmentDefintion);
    void RemoveEntry(UAfterEquipmentInstance* Instace);

private:
    UAfterAbilitySystemComponent* GetAbilitySystemComponent() const;

    friend UAfterEquipmentManagerComponent;

private:
    UPROPERTY()
    TArray<FAfterAppliedEquipmentEntry> Entries;

    UPROPERTY()
    TObjectPtr<UActorComponent> OwnerComponent;
};

/**
 */
UCLASS(BlueprintType, Const)
class UAfterEquipmentManagerComponent : public UPawnComponent
{
    GENERATED_BODY()

public:
    UAfterEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable)
    UAfterEquipmentInstance* EquipItem(TSubclassOf<UAfterEquipmentDefinition> EquipmentClass);

    UFUNCTION(BlueprintCallable)
    void UnequipItem(UAfterEquipmentInstance* ItemInstance);

    //~UActorComponent interface
    virtual void InitializeComponent() override;
    virtual void UninitializeComponent() override;
    //~End of UActorComponent interface

    UFUNCTION(BlueprintCallable, BlueprintPure)
    UAfterEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UAfterEquipmentInstance> InstanceType);

    UFUNCTION(BlueprintCallable, BlueprintPure)
    TArray<UAfterEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UAfterEquipmentInstance> InstanceType) const;

    template <typename T>
    T* GetFirstInstanceOfType()
    {
        return (T*)GetFirstInstanceOfType(T::StaticClass());
    }

private:
    UPROPERTY()
    FAfterEquipmentList EquipmentList;
};
