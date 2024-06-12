#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "AfterCoreTypes.h"
#include "Weapon.generated.h"

class USoundCue;
class UParticleSystem;
class UAnimMontage;

UCLASS()
class AFTER_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    FOnPlayAnimMontage OnPlayAnimMontage;

    AWeapon();

    virtual void StartAttacking();
    virtual void StopAttacking();

    void ThrowWeapon(const FVector& OwnerForward, const FRotator& OwnerRotation);

    UAnimMontage* GetAttackMontage() const { return AttackMontage; }
    FName GetWeaponSocketName() const { return WeaponSocketName; }

    virtual void Tick(float DeltaTime) override;

protected:
    void StopFalling();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Sound")
    USoundCue* AttackSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* AttackMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Effect")
    UParticleSystem* ImpactPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
    FName WeaponSocketName = "SpineSocketForWeapon";

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float DamageAmount = 10.f;

private:
    FTimerHandle ThrowWeaponTimer;

    float ThrowWeaponTime = 0.7f;
    bool bFalling = false;
};
