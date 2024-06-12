#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USphereComponent;
class UAfterPickupDefinition;

UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup UMETA(DisplayName = "Pickup"),
    EIS_PickedUp UMETA(DisplayName = "PickedUp"),
    EIS_Equipped UMETA(DisplayName = "Equipped"),
    EIS_Falling UMETA(DisplayName = "Falling"),
};

UCLASS()
class AFTER_API AItem : public AActor
{
    GENERATED_BODY()

public:
    AItem();

    virtual void Tick(float DeltaTime) override;

    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return DisplayMesh; }
    FORCEINLINE UAfterPickupDefinition* GetPickupDefinition() const { return PickupDefintion; }

    void SetItemState(EItemState State);
    void SetIngnoreAllCollisions() const;

protected:
    UFUNCTION()
    void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnSphereEndOverlap(
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    virtual void BeginPlay() override;

    void SetItemProperties(EItemState State);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    TObjectPtr<USkeletalMeshComponent> DisplayMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    FName DisplayName;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    TObjectPtr<UAfterPickupDefinition> PickupDefintion;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    TObjectPtr<UWidgetComponent> PickupWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    TObjectPtr<UBoxComponent> CollisionBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    TObjectPtr<USphereComponent> AreaSphere;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Item Properties")
    EItemState ItemState;
};
