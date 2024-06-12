#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/Weapon.h"
#include "AfterCoreTypes.h"
#include "RangedWeapon.generated.h"

UCLASS()
class AFTER_API ARangedWeapon : public AWeapon
{
    GENERATED_BODY()

public:
    ARangedWeapon();

    bool CanReload() const;

    void ChangeClip();

    virtual void StartAttacking() override;
    virtual void StopAttacking() override;

    ERangedWeaponCategory GetWeaponCategory() const;

    UFUNCTION(BlueprintCallable)
    int32 GetCurrenBullets();

    UFUNCTION(BlueprintCallable)
    int32 GetCurrentCarriedAmmo();

    FRangedWeaponAnimations GetAnimations() const;

    UFUNCTION(BlueprintCallable)
    void GrabMagazine();

    UFUNCTION(BlueprintCallable)
    void ReleaseMagazine();

protected:
    virtual void BeginPlay() override;

    virtual void MakeShot();
    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;

    APlayerController* GetPlayerController() const;
    bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
    FVector GetMuzzelWorldLocation() const;
    FVector GetShootingPointStart(FVector& ViewLocation, FRotator& ViewRotation) const;

    void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);

    void DecreaseAmmo();
    bool IsAmmoEmpty() const;
    bool IsClipEmpty() const;

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    ERangedWeaponCategory WeaponCategory = ERangedWeaponCategory::ERWC_Primary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effect")
    UParticleSystem* MuzzelFlash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effect")
    UParticleSystem* BeamParticals;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Magazine")
    USkeletalMeshComponent* MagazineMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Magazine")
    FName MagazineSocketName = "ClipSocket";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon|Magazine")
    FName GrabSocketName = "LeftHand";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FName MuzzelSocketName = "MuzzelSocket";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FRangedWeaponAnimations WeaponAnimations;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TraceMaxDistance = 1500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    FAmmoData DefaultAmmo;

    UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly)
    int32 CarriedAmmo = 0;

private:
    FAmmoData CurrentAmmo;

public:
    FOnClipEmptySignature OnClipEmpty;
};
