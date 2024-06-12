#include "Interface/Weapons/AfterReticleWidgetBase.h"

#include "Inventory/AfterInventoryItemInstance.h"
#include "Weapons/AfterRangedWeaponInstance.h"
#include "Weapons/AfterWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterReticleWidgetBase)

UAfterReticleWidgetBase::UAfterReticleWidgetBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterReticleWidgetBase::InitializeFromWeapon(UAfterWeaponInstance* InWeapon)
{
    WeaponInstance = InWeapon;
    InventoryInstance = nullptr;
    if (WeaponInstance)
    {
        InventoryInstance = Cast<UAfterInventoryItemInstance>(WeaponInstance->GetInstigator());
    }

    OnWeaponInitialized();
}

float UAfterReticleWidgetBase::ComputeSpreadAngle() const
{
    if (const UAfterRangedWeaponInstance* RangedWeapon = Cast<const UAfterRangedWeaponInstance>(WeaponInstance))
    {
        const float SpreadAngleMultiplier = RangedWeapon->GetCalculatedSpreadAngleMultiplier();
        const float ActualSpreadAngle = SpreadAngleMultiplier;

        return ActualSpreadAngle;
    }
    else
    {
        return 0.0f;
    }
}

float UAfterReticleWidgetBase::ComputeMaxScreenspaceSpreadRadius() const
{
    const float LongShotDistance = 10000.f;

    APlayerController* PC = GetOwningPlayer();
    if (PC && PC->PlayerCameraManager)
    {
        const float SpreadRadiusRads = FMath::DegreesToRadians(ComputeSpreadAngle() * 0.5f);
        const float SpreadRadiusAtDistance = FMath::Tan(SpreadRadiusRads) * LongShotDistance;

        FVector CamPos;
        FRotator CamOrient;
        PC->PlayerCameraManager->GetCameraViewPoint(CamPos, CamOrient);

        FVector CamForwDir = CamOrient.RotateVector(FVector::ForwardVector);
        FVector CamUpDir = CamOrient.RotateVector(FVector::UpVector);

        FVector OffsetTargetAtDistance = CamPos + (CamForwDir * LongShotDistance) + (CamUpDir * SpreadRadiusAtDistance);

        FVector2D OffsetTargetInScreenspace;

        if (PC->ProjectWorldLocationToScreen(OffsetTargetAtDistance, OffsetTargetInScreenspace, true))
        {
            int32 ViewportSizeX(0), ViewportSizeY(0);
            PC->GetViewportSize(ViewportSizeX, ViewportSizeY);

            const FVector2D ScreenSpaceCenter(FVector::FReal(ViewportSizeX) * 0.5f, FVector::FReal(ViewportSizeY) * 0.5f);

            return (OffsetTargetInScreenspace - ScreenSpaceCenter).Length();
        }
    }

    return 0.0f;
}

bool UAfterReticleWidgetBase::HasFirstShotAccuracy() const
{
    if (const UAfterRangedWeaponInstance* RangedWeapon = Cast<const UAfterRangedWeaponInstance>(WeaponInstance))
    {
        return RangedWeapon->HasFirstShotAccuracy();
    }
    else
    {
        return false;
    }
}
