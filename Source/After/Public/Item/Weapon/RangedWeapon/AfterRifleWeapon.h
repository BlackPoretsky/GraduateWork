#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon/RangedWeapon.h"
#include "AfterRifleWeapon.generated.h"

UCLASS()
class AFTER_API AAfterRifleWeapon : public ARangedWeapon
{
    GENERATED_BODY()

public:
    virtual void StartAttacking() override;
    virtual void StopAttacking() override;

protected:
    virtual void MakeShot() override;
    virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const override;

private:
    void MakeDamage(const FHitResult& HitResult);
    void DrawBulletTrace(const FVector& TraceEnd);

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float BulletSpread = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    float TimeBetweenShot = 0.1f;

private:
    FTimerHandle ShotTimerHandle;
};
