#include "Weapons/AfterWeaponInstance.h"

#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterWeaponInstance)

UAfterWeaponInstance::UAfterWeaponInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterWeaponInstance::OnEquipped()
{
    Super::OnEquipped();

    UWorld* World = GetWorld();
    check(World);
    TimeLastEquipped = World->GetTimeSeconds();
}

void UAfterWeaponInstance::OnUnequipped()
{
    Super::OnUnequipped();
}

void UAfterWeaponInstance::UpdateFiringTime()
{
    UWorld* World = GetWorld();
    check(World);
    TimeLastFired = World->GetTimeSeconds();
}

float UAfterWeaponInstance::GetTimeSinceLastInteractedWith() const
{
    UWorld* World = GetWorld();
    check(World);
    const double WorldTime = World->GetTimeSeconds();

    double Result = WorldTime - TimeLastEquipped;

    if (TimeLastFired > 0.0)
    {
        const double TimeSinceFired = WorldTime - TimeLastFired;
        Result = FMath::Min(Result, TimeSinceFired);
    }

    return Result;
}