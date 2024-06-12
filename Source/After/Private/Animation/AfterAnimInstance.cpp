#include "Animation/AfterAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "Character/AfterCharacter.h"
#include "Character/AfterCharacterMovementComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAnimInstance)

UAfterAnimInstance::UAfterAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
    check(ASC);

    GameplayTagPropertyMap.Initialize(this, ASC);
}

void UAfterAnimInstance::UpdateAnimationProperties(float Deltatime)
{
    if (!Owner) return;

    FVector Velocity = Owner->GetVelocity();
    Velocity.Z = 0;

    Speed = Velocity.Size();

    bIsInAir = Owner->GetCharacterMovement()->IsFalling();

    if (Owner->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
    {
        bIsAcceleratig = true;
    }
    else
    {
        bIsAcceleratig = false;
    }

    FRotator AimRotation = Owner->GetBaseAimRotation();
    FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Owner->GetVelocity());

    MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

    if (Owner->GetVelocity().Size() > 0.f)
    {
        LastMovementOffsetYaw = MovementOffsetYaw;
    }
}

void UAfterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    Owner = Cast<AAfterCharacter>(TryGetPawnOwner());

    if (AActor* OwningActor = GetOwningActor())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
        {
            InitializeWithAbilitySystem(ASC);
        }
    }
}

void UAfterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    const AAfterCharacter* Character = Cast<AAfterCharacter>(GetOwningActor());
    if (!Character) return;

    UAfterCharacterMovementComponent* CharMoveComp = CastChecked<UAfterCharacterMovementComponent>(Character->GetCharacterMovement());
    const FAfterCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
    GroundDistance = GroundInfo.GroundDistance;
}
