#pragma once

#include "Components/ControllerComponent.h"
#include "Inventory/AfterInventoryItemInstance.h"

#include "AfterQuickBarComponent.generated.h"

class AActor;
class UAfterEquipmentInstance;
class UAfterEquipmentManagerComponent;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class UAfterQuickBarComponent : public UControllerComponent
{
    GENERATED_BODY()

public:
    UAfterQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After")
    void CycleActiveSlotForward();

    UFUNCTION(BlueprintCallable, Category = "After")
    void CycleActiveSlotBackward();

    UFUNCTION(BlueprintCallable, Category = "After")
    void SetActiveSlotIndex(int32 NewIndex);

    UFUNCTION(BlueprintCallable, BlueprintPure = false)
    TArray<UAfterInventoryItemInstance*> GetSlots() const { return Slots; }

    UFUNCTION(BlueprintCallable, BlueprintPure = false)
    int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

    UFUNCTION(BlueprintCallable, BlueprintPure = false)
    UAfterInventoryItemInstance* GetActiveSlotItem() const;

    UFUNCTION(BlueprintCallable, BlueprintPure = false)
    int32 GetNextFreeItemSlot() const;

    UFUNCTION(BlueprintCallable)
    void AddItemToSlot(int32 SlotIndex, UAfterInventoryItemInstance* Item);

    UFUNCTION(BlueprintCallable)
    UAfterInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

    virtual void BeginPlay() override;

private:
    void EquipItemInSlot();
    void UnequipItemInSlot();

    UAfterEquipmentManagerComponent* FindEquipmentManager() const;

protected:
    int32 NumSlots = 3;

    UFUNCTION()
    void OnRep_ActiveSlotIndex();

private:
    UPROPERTY()
    TArray<TObjectPtr<UAfterInventoryItemInstance>> Slots;

    int32 ActiveSlotIndex = -1;

    UPROPERTY()
    TObjectPtr<UAfterEquipmentInstance> EquippedItem;
};

USTRUCT(BlueprintType)
struct FAfterQuickBarActiveIndexChangedMessage
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<AActor> Owner = nullptr;

    UPROPERTY(BlueprintReadOnly, Category = "Inventory")
    int32 ActiveIndex = 0;
};
