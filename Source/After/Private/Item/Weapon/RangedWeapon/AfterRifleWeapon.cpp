#include "Item/Weapon/RangedWeapon/AfterRifleWeapon.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "AfterLogChannels.h"

void AAfterRifleWeapon::StartAttacking()
{
    Super::StartAttacking();

    GetWorldTimerManager().SetTimer(ShotTimerHandle, this, &AAfterRifleWeapon::MakeShot, TimeBetweenShot, true);
    MakeShot();
}

void AAfterRifleWeapon::StopAttacking()
{
    GetWorldTimerManager().ClearTimer(ShotTimerHandle);
}

void AAfterRifleWeapon::MakeShot()
{
    if (!GetWorld() || IsClipEmpty())
    {
        StopAttacking();
        OnClipEmpty.Broadcast();
        return;
    }

    FVector TraceStart;
    FVector TraceEnd;
    if (!GetTraceData(TraceStart, TraceEnd))
    {
        StopAttacking();
        return;
    }

    if (AttackMontage)
    {
        OnPlayAnimMontage.Broadcast(AttackMontage);
    }

    if (AttackSound)
    {
        UGameplayStatics::PlaySound2D(this, AttackSound);
    }

    const USkeletalMeshSocket* MuzzelSocket = DisplayMesh->GetSocketByName(MuzzelSocketName);
    if (MuzzelSocket)
    {
        const FTransform SocketTransform = MuzzelSocket->GetSocketTransform(GetItemMesh());
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzelFlash, SocketTransform);
    }

    DecreaseAmmo();

    FHitResult HitResult;
    MakeHit(HitResult, TraceStart, TraceEnd);
    if (HitResult.bBlockingHit)
    {
        MakeDamage(HitResult);
        DrawDebugLine(GetWorld(), GetMuzzelWorldLocation(), HitResult.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);
        DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10.f, 24, FColor::Red, false, 3.f, 0, 3.f);

        DrawBulletTrace(HitResult.ImpactPoint);
    }
    else
    {
        DrawDebugLine(GetWorld(), GetMuzzelWorldLocation(), TraceEnd, FColor::Red, false, 3.f, 0, 3.f);

        DrawBulletTrace(TraceEnd);
    }
}

bool AAfterRifleWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
    FVector ViewLocation;
    FRotator ViewRoatation;
    if (!GetPlayerViewPoint(ViewLocation, ViewRoatation)) return false;

    TraceStart = GetShootingPointStart(ViewLocation, ViewRoatation);
    const auto HalfRad = FMath::DegreesToRadians(BulletSpread);
    const FVector ShootDirection = FMath::VRandCone(ViewRoatation.Vector(), HalfRad);
    TraceEnd = ViewLocation + ShootDirection * TraceMaxDistance;

    return true;
}

void AAfterRifleWeapon::MakeDamage(const FHitResult& HitResult)
{
    const auto DamageActor = HitResult.GetActor();
    if (!DamageActor) return;
    UE_LOG(LogAfter, Display, TEXT("Apply damage!"))
    DamageActor->TakeDamage(DamageAmount, FDamageEvent(), GetPlayerController(), this);
}

void AAfterRifleWeapon::DrawBulletTrace(const FVector& TraceEnd)
{
    UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticals,  //
        DisplayMesh->GetSocketTransform(MuzzelSocketName));
    if (!Beam) return;

    Beam->SetVectorParameter(FName("Target"), TraceEnd);
}
