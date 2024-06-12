#include "Item/Weapon/Weapon.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeapon, All, All)

AWeapon::AWeapon()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::StartAttacking() {}

void AWeapon::StopAttacking() {}

void AWeapon::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (GetItemState() == EItemState::EIS_Falling && bFalling)
    {
        FRotator MeshRotation = FRotator(0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f);
        GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

void AWeapon::ThrowWeapon(const FVector& OwnerForward, const FRotator& OwnerRotation)
{
    GetItemMesh()->SetWorldRotation(FRotator(0.f, OwnerRotation.Yaw, 0.f), false, nullptr, ETeleportType::TeleportPhysics);

    FVector ImpulseDiraction = OwnerForward.RotateAngleAxis(90.f, FVector(0.f, 0.f, 1.f));
    ImpulseDiraction *= 2'000.f;
    GetItemMesh()->AddImpulse(ImpulseDiraction);

    bFalling = true;
    GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
    bFalling = false;
    SetItemState(EItemState::EIS_Pickup);
}
