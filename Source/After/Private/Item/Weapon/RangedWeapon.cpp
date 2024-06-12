#include "Item/Weapon/RangedWeapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogRangedWeapon, All, All)

ARangedWeapon::ARangedWeapon()
{
    MagazineMesh = CreateDefaultSubobject<USkeletalMeshComponent>("MagazineMesh");
}

void ARangedWeapon::BeginPlay()
{
    Super::BeginPlay();

    checkf(DefaultAmmo.Bullets > 0, TEXT("Bullets count can't be less or equal zero!"));
    checkf(DefaultAmmo.Clips > 0, TEXT("Clips count can't be less or equal zero!"));

    MagazineMesh->AttachToComponent(DisplayMesh, FAttachmentTransformRules::KeepRelativeTransform, MagazineSocketName);

    CurrentAmmo = DefaultAmmo;
    CarriedAmmo = CurrentAmmo.Clips * (DefaultAmmo.Infinite ? 0 : CurrentAmmo.Bullets);
}

void ARangedWeapon::StartAttacking() {}

void ARangedWeapon::StopAttacking() {}

ERangedWeaponCategory ARangedWeapon::GetWeaponCategory() const
{
    return WeaponCategory;
}

int32 ARangedWeapon::GetCurrenBullets()
{
    return CurrentAmmo.Bullets;
}

int32 ARangedWeapon::GetCurrentCarriedAmmo()
{
    return CarriedAmmo;
}

FRangedWeaponAnimations ARangedWeapon::GetAnimations() const
{
    return WeaponAnimations;
}

void ARangedWeapon::GrabMagazine()
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player || !MagazineMesh) return;

    MagazineMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    MagazineMesh->AttachToComponent(Player->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, GrabSocketName);
}

void ARangedWeapon::ReleaseMagazine()
{
    if (!MagazineMesh) return;

    MagazineMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
    MagazineMesh->AttachToComponent(DisplayMesh, FAttachmentTransformRules::KeepRelativeTransform, MagazineSocketName);
}

APlayerController* ARangedWeapon::GetPlayerController() const
{
    const auto Player = Cast<ACharacter>(GetOwner());
    if (!Player) return nullptr;

    return Player->GetController<APlayerController>();
}

bool ARangedWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const auto Controller = GetPlayerController();
    if (!Controller) return false;

    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);
    return true;
}

FVector ARangedWeapon::GetMuzzelWorldLocation() const
{
    return DisplayMesh->GetSocketLocation(MuzzelSocketName);
}

bool ARangedWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRotation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

    TraceStart = GetShootingPointStart(ViewLocation, ViewRotation);
    const FVector ShootDiraction = ViewRotation.Vector();
    TraceEnd = ViewLocation + ShootDiraction * TraceMaxDistance;

    return true;
}

void ARangedWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
    if (!GetWorld()) return;

    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(GetOwner());

    GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility, CollisionParams);
}

FVector ARangedWeapon::GetShootingPointStart(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const FVector MuzzelLocation = GetMuzzelWorldLocation();
    const FVector VectorCameraToMuzzel = MuzzelLocation - ViewLocation;
    const FVector ProjectionOnViewVector = VectorCameraToMuzzel.ProjectOnToNormal(ViewRotation.Vector());

    const FVector Out = ViewLocation + ProjectionOnViewVector;

    DrawDebugLine(GetWorld(), MuzzelLocation, Out, FColor::Green, false, 3.0f, 0, 3.0f);

    return Out;
}

void ARangedWeapon::DecreaseAmmo()
{
    CurrentAmmo.Bullets--;

    if (IsClipEmpty() && !IsAmmoEmpty())
    {
        StopAttacking();
        OnClipEmpty.Broadcast();
    }
}

bool ARangedWeapon::IsAmmoEmpty() const
{
    return !CurrentAmmo.Infinite && CurrentAmmo.Clips == 0 && IsClipEmpty();
}

bool ARangedWeapon::IsClipEmpty() const
{
    return CurrentAmmo.Bullets == 0;
}

void ARangedWeapon::ChangeClip()
{
    if (CurrentAmmo.Bullets == DefaultAmmo.Bullets) return;

    if (!CurrentAmmo.Infinite)
    {
        if (CurrentAmmo.Clips == 0)
        {
            return;
        }

        CarriedAmmo = CarriedAmmo - DefaultAmmo.Bullets + CurrentAmmo.Bullets;
        if (CurrentAmmo.Bullets == 0                      //
            || ((CarriedAmmo % DefaultAmmo.Bullets == 0)  //
                   && (CarriedAmmo / DefaultAmmo.Bullets < CurrentAmmo.Clips)))
        {
            CurrentAmmo.Clips--;
        }
    }
    CurrentAmmo.Bullets = DefaultAmmo.Bullets;
}

bool ARangedWeapon::CanReload() const
{
    return CurrentAmmo.Bullets < DefaultAmmo.Bullets && CurrentAmmo.Clips > 0;
}

void ARangedWeapon::MakeShot() {}