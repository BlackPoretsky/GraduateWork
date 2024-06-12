#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AfterCoreTypes.h"
#include "AfterWeaponComponent.generated.h"

class AWeapon;
class ARangedWeapon;
class AMeleeWeapon;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AFTER_API UAfterWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UAfterWeaponComponent();

    void StartAttacking();
    void StopAttacking();
    void Reload();

    void AimingPressed();
    void AimingReleased();

    void DropWeapon();
    void PickupWeapon(AWeapon* WeaponToPickup);

    void EquipNextWeapon();
    void EquipPrevWeapon();
    void EquipPrimaryWeapon();
    void EquipSecondaryWeapon();
    void EquipMeleeWeapon();

    void CalculatedCrosshairSpread(float DeltaTime);

    bool IsAiming() const;

    UFUNCTION(BlueprintCallable)
    AWeapon* GetCurrentWeapon() const;

    UFUNCTION(BlueprintCallable)
    float GetCrosshairSpreadMultiplier() const;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    void SpawnDefaultArsenal();
    void AddWeaponToArsenal(AWeapon* NewWeapon, int32 AddIndex);
    void OrganizeArsenal(AWeapon* WeaponToPickup, int32 WeaponIndex);

    void EquipWeapon(int32 WeaponIndex);
    void DropWeapon(int32 IndexWeapon);
    void DetachWeaponFromCharacter(AWeapon* DetachableWeapon, ACharacter* Character);
    void AttachWeaponToSocket(AWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    TArray<TSubclassOf<AWeapon>> GetDefaultArsenal();

    void InitEquipAnimation(UAnimMontage* EquipAnimation);
    void InitReloadAnimation(UAnimMontage* ReloadAnimation);
    void TerminateReloadAnimation(UAnimMontage* ReloadAnimation);

    void PlayAnimMontage(UAnimMontage* Animation);

    void OnEquipFinished(USkeletalMeshComponent* MeshComponent);
    void OnReloadFinished(USkeletalMeshComponent* MeshComponent);

    bool CanAttack() const;
    bool CanEquip() const;
    bool CanReload(ARangedWeapon* RangedCurrentWeapon) const;

    void OnEmptyClip();
    void ChangeClip();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Weapon|DefaultArsenal")
    TSubclassOf<AWeapon> PrimaryWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|DefaultArsenal")
    TSubclassOf<AWeapon> SecondaryWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|DefaultArsenal")
    TSubclassOf<AWeapon> MeleeWeapon;

    UPROPERTY(EditDefaultsOnly, Category = "Weapon")
    FName WeaponEquipSocketName = "WeaponEquipSocket";

    UPROPERTY(EditDefaultsOnly, Category = "Weapon|Animation");
    UAnimMontage* EquipAnimMontage = nullptr;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    AWeapon* CurrentWeapon = nullptr;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon|Crosshair");
    float CrosshairSpreadMultiplier = 0.f;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon|Crosshair");
    float CrosshairVelocityFactor = 0.f;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon|Crosshair");
    float CrosshairInAirFactor = 0.f;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon|Crosshair");
    float CrosshairAimFactor = 0.f;

    UPROPERTY(VisibleDefaultsOnly, Category = "Weapon|Crosshair");
    float CrosshairShootingFactor = 0.f;

private:
    UPROPERTY()
    TArray<AWeapon*> Arsenal;

    UPROPERTY()
    UAnimMontage* CurrentReloadAnimMontage = nullptr;

    int32 CurrentWeaponIndex = 0;
    bool bEquipAnimInProgress = false;
    bool bReloadAnimInProgress = false;

    bool bAiming = false;
    bool bAttack = false;
};
