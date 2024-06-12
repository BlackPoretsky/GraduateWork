#include "Character/AfterCharacterMovementComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"

#include "Character/AfterCharacter.h"

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_MovementStopped, "Gameplay.MovementStopped")

namespace AfterCharacter
{
    static float GroundTraceDistance = 1000.0f;
    FAutoConsoleVariableRef CVar_GroundTraceDistance(TEXT("AfterCharacter.GroundTraceDistance"), GroundTraceDistance,
        TEXT("Distance to trace down when generating ground information."), ECVF_Cheat);
}  // namespace AfterCharacter

UAfterCharacterMovementComponent::UAfterCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UAfterCharacterMovementComponent::CanAttemptJump() const
{
    return IsJumpAllowed() && (IsMovingOnGround() || IsFalling());
}

void UAfterCharacterMovementComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

const FAfterCharacterGroundInfo& UAfterCharacterMovementComponent::GetGroundInfo()
{
    if (!CharacterOwner || (GFrameCounter == CachedGroundInfo.LastUpdateFrame))
    {
        return CachedGroundInfo;
    }

    if (MovementMode == MOVE_Walking)
    {
        CachedGroundInfo.GroundHitResult = CurrentFloor.HitResult;
        CachedGroundInfo.GroundDistance = 0.0f;
    }
    else
    {
        const UCapsuleComponent* CapsuleComp = CharacterOwner->GetCapsuleComponent();
        check(CapsuleComp);

        const float CapsuleHalfHeight = CapsuleComp->GetUnscaledCapsuleHalfHeight();
        const ECollisionChannel CollisionChannel = (UpdatedComponent ? UpdatedComponent->GetCollisionObjectType() : ECC_Pawn);
        const FVector TraceStart(GetActorLocation());
        const FVector TraceEnd(TraceStart.X, TraceStart.Y, (TraceStart.Z - AfterCharacter::GroundTraceDistance - CapsuleHalfHeight));

        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AfterCharacterMovementComponent_GetGroundInfo), false, CharacterOwner);
        FCollisionResponseParams ResponseParam;
        InitCollisionParams(QueryParams, ResponseParam);

        FHitResult HitResult;
        GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, CollisionChannel, QueryParams, ResponseParam);

        CachedGroundInfo.GroundHitResult = HitResult;
        CachedGroundInfo.GroundDistance = AfterCharacter::GroundTraceDistance;

        if (MovementMode == MOVE_NavWalking)
        {
            CachedGroundInfo.GroundDistance = 0.0f;
        }
        else if (HitResult.bBlockingHit)
        {
            CachedGroundInfo.GroundDistance = FMath::Max((HitResult.Distance - CapsuleHalfHeight), 0.0f);
        }
    }

    CachedGroundInfo.LastUpdateFrame = GFrameCounter;

    return CachedGroundInfo;
}

FRotator UAfterCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
    {
        if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
        {
            return FRotator(0, 0, 0);
        }
    }

    return Super::GetDeltaRotation(DeltaTime);
}

float UAfterCharacterMovementComponent::GetMaxSpeed() const
{
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
    {
        if (ASC->HasMatchingGameplayTag(TAG_Gameplay_MovementStopped))
        {
            return 0;
        }
    }

    return Super::GetMaxSpeed();
}
