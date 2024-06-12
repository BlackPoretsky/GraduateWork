#include "Weapons/AfterRangedWeaponInstance.h"

#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/AfterCameraComponent.h"
#include "Weapons/AfterWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterRangedWeaponInstance)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_After_Weapon_SteadyAimingCamera, "After.WeaponSteadyAimingCamera")

UAfterRangedWeaponInstance::UAfterRangedWeaponInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterRangedWeaponInstance::Tick(float DeltaSeconds)
{
    APawn* Pawn = GetPawn();
    check(Pawn);

    const bool bMinMultipliers = UpdateMultipliers(DeltaSeconds);

    bHasFirstShotAccuracy = bAllowFirstShotAccuracy && bMinMultipliers;
}

void UAfterRangedWeaponInstance::OnEquipped()
{
    Super::OnEquipped();

    CurrentSpreadAngleMultiplier = 1.0f;
    StandingStillMultiplier = 1.0f;
    JumpFallMultiplier = 1.0f;
    CrouchingMultiplier = 1.0f;
}

void UAfterRangedWeaponInstance::OnUnequipped()
{
    Super::OnUnequipped();
}

float UAfterRangedWeaponInstance::GetDistanceAttenuation(
    float Distance, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags) const
{
    const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
    return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
}

bool UAfterRangedWeaponInstance::UpdateMultipliers(float DeltaSeconds)
{
    const float MultiplierNearlyEqualThreshold = 0.05f;

    APawn* Pawn = GetPawn();
    check(Pawn);
    UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());

    const float PawnSpeed = Pawn->GetVelocity().Size();
    const float MovementTargetValue = FMath::GetMappedRangeValueClamped(
        FVector2D(StandingStillSpeedThreshold, StandingStillSpeedThreshold + StandingStillToMovingSpeedRange),
        FVector2D(SpreadAngleMultiplier_StandingStill, 1.0f), PawnSpeed);
    StandingStillMultiplier = FMath::FInterpTo(StandingStillMultiplier, MovementTargetValue, DeltaSeconds, TransitionRate_StandingStill);
    const bool bStandingStillMultiplierAtMin =
        FMath::IsNearlyEqual(StandingStillMultiplier, SpreadAngleMultiplier_StandingStill, SpreadAngleMultiplier_StandingStill * 0.1f);

    const bool bIsCrouching = CharMovementComp && CharMovementComp->IsCrouching();
    const float CrouchingTargetValue = bIsCrouching ? SpreadAngleMultiplier_Crouching : 1.0f;
    CrouchingMultiplier = FMath::FInterpTo(CrouchingMultiplier, CrouchingTargetValue, DeltaSeconds, TransitionRate_Crouching);
    const bool bCrouchingMultiplierAtTarget =
        FMath::IsNearlyEqual(CrouchingMultiplier, CrouchingTargetValue, MultiplierNearlyEqualThreshold);

    const bool bIsJumpintOrFalling = CharMovementComp && CharMovementComp->IsFalling();
    const float JumpFallTargetValue = bIsJumpintOrFalling ? SpreadAngleMultiplier_JumpingOrFalling : 1.0f;
    JumpFallMultiplier = FMath::FInterpTo(JumpFallMultiplier, JumpFallTargetValue, DeltaSeconds, TransitionRate_JumpingOrFalling);
    const bool bJumpFallMultiplierIs1 = FMath::IsNearlyEqual(JumpFallMultiplier, 1.0f, MultiplierNearlyEqualThreshold);

    float AimingAlpha = 0.0f;
    if (const UAfterCameraComponent* CameraComponent = UAfterCameraComponent::FindCameraComponent(Pawn))
    {
        float TopCameraWeight;
        FGameplayTag TopCameraTag;
        CameraComponent->GetBlendInfo(TopCameraWeight, TopCameraTag);

        AimingAlpha = (TopCameraTag == TAG_After_Weapon_SteadyAimingCamera) ? TopCameraWeight : 0.0f;
    }

    const float AimingMultiplier =
        FMath::GetMappedRangeValueClamped(FVector2D(0.0, 1.0f), FVector2D(1.0f, SpreadAngleMultiplier_Aiming), AimingAlpha);
    const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleMultiplier_Aiming, KINDA_SMALL_NUMBER);

    const float CombinedMultiplier = AimingMultiplier * StandingStillMultiplier * CrouchingMultiplier * JumpFallMultiplier;
    CurrentSpreadAngleMultiplier = CombinedMultiplier;

    return bStandingStillMultiplierAtMin && bCrouchingMultiplierAtTarget && bJumpFallMultiplierIs1 && bAimingMultiplierAtTarget;
}
