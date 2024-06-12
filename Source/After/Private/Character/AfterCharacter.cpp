// Project After. Creator Egor Gorochkin.

#include "Character/AfterCharacter.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Item/Item.h"
#include "Components/WidgetComponent.h"
#include "Components/CapsuleComponent.h"

#include "Camera/AfterCameraComponent.h"

#include "Character/AfterHealthComponent.h"
#include "Character/AfterCharacterMovementComponent.h"

#include "Character/AfterPawnExtensionComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "AfterGameplayTags.h"
#include "AfterLogChannels.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"

#include "Player/AfterPlayerController.h"
#include "Player/AfterPlayerState.h"

#include "Equipment/AfterEquipmentManagerComponent.h"
#include "Equipment/AfterPickupDefinition.h"
#include "Inventory/AfterInventoryManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterCharacter)

DEFINE_LOG_CATEGORY_STATIC(LogAfterCharacter, All, All);

static FName NAME_AfterCharacterCollisionProfile_Capsule(TEXT("AfterPawnCapsule"));
static FName NAME_AfterCharacterCollisionProfile_Mesh(TEXT("AfterPawnMesh"));

AAfterCharacter::AAfterCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UAfterCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;

    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    check(CapsuleComp);
    CapsuleComp->InitCapsuleSize(40.0f, 90.0f);
    CapsuleComp->SetCollisionProfileName(NAME_AfterCharacterCollisionProfile_Capsule);

    USkeletalMeshComponent* MeshComp = GetMesh();
    check(MeshComp);
    MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
    MeshComp->SetCollisionProfileName(NAME_AfterCharacterCollisionProfile_Mesh);

    UAfterCharacterMovementComponent* AfterMoveComp = CastChecked<UAfterCharacterMovementComponent>(GetCharacterMovement());
    AfterMoveComp->GravityScale = 1.0f;
    AfterMoveComp->MaxAcceleration = 2400.0f;
    AfterMoveComp->BrakingFrictionFactor = 1.0f;
    AfterMoveComp->BrakingFriction = 6.0f;
    AfterMoveComp->GroundFriction = 8.0f;
    AfterMoveComp->BrakingDecelerationWalking = 1400.0f;
    AfterMoveComp->bUseControllerDesiredRotation = false;
    AfterMoveComp->bOrientRotationToMovement = false;
    AfterMoveComp->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
    AfterMoveComp->bAllowPhysicsRotationDuringAnimRootMotion = false;
    AfterMoveComp->GetNavAgentPropertiesRef().bCanCrouch = true;
    AfterMoveComp->bCanWalkOffLedgesWhenCrouching = true;
    AfterMoveComp->SetCrouchedHalfHeight(65.0f);

    PawnExtComponent = CreateDefaultSubobject<UAfterPawnExtensionComponent>("PawnExtinsionComponent");
    PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
    PawnExtComponent->OnAbilitySystemUninitialized_Register(
        FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));

    EquipmentManagerComponent = CreateDefaultSubobject<UAfterEquipmentManagerComponent>("EquipmentManagerComponent");

    HealthComponent = CreateDefaultSubobject<UAfterHealthComponent>("HealthComponent");
    HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
    HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

    CameraComponent = CreateDefaultSubobject<UAfterCameraComponent>("CameraComponent");
    CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    BaseEyeHeight = 80.0f;
    CrouchedEyeHeight = 50.0f;
}

void AAfterCharacter::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AAfterCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(HealthComponent);
    check(CameraComponent);
    check(GetCharacterMovement());

    LandedDelegate.AddDynamic(this, &AAfterCharacter::OnGraundLanded);
}

void AAfterCharacter::Reset()
{
    DisableMovementAndCollision();

    K2_OnReset();

    UninitAndDestroy();
}

void AAfterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void AAfterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    TraceForItems();
}

void AAfterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PawnExtComponent->SetupPlayerInputComponent();
}

AAfterPlayerController* AAfterCharacter::GetAfterPlayerController() const
{
    return CastChecked<AAfterPlayerController>(Controller, ECastCheckedType::NullAllowed);
}

AAfterPlayerState* AAfterCharacter::GetAfterPlayerState() const
{
    return CastChecked<AAfterPlayerState>(GetPlayerState(), ECastCheckedType::NullAllowed);
}

UAfterAbilitySystemComponent* AAfterCharacter::GetAfterAbilitySystemComponent() const
{
    return Cast<UAfterAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AAfterCharacter::GetAbilitySystemComponent() const
{
    if (!IsValid(PawnExtComponent)) return nullptr;

    return PawnExtComponent->GetAfterAbilitySystemComponent();
}

void AAfterCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
    if (const UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->GetOwnedGameplayTags(TagContainer);
    }
}

bool AAfterCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
    if (const UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        return AfterASC->HasMatchingGameplayTag(TagToCheck);
    }

    return false;
}

bool AAfterCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (const UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->HasAllMatchingGameplayTags(TagContainer);
    }

    return false;
}

bool AAfterCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
    if (const UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->HasAnyMatchingGameplayTags(TagContainer);
    }

    return false;
}

void AAfterCharacter::OnAbilitySystemInitialized()
{
    UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent();
    check(AfterASC);

    HealthComponent->InitializeWithAbilitySystem(AfterASC);

    InitializeGameplayTags();
}

void AAfterCharacter::OnAbilitySystemUninitialized()
{
    HealthComponent->UninitializeFromAbilitySystem();
}

void AAfterCharacter::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    PawnExtComponent->HandleControllerChanged();
}

void AAfterCharacter::UnPossessed()
{
    Super::UnPossessed();

    PawnExtComponent->HandleControllerChanged();
}

void AAfterCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();

    PawnExtComponent->HandleControllerChanged();
}

void AAfterCharacter::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    PawnExtComponent->HandlePlayerStateReplicated();
}

void AAfterCharacter::InitializeGameplayTags()
{
    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        for (const TPair<uint8, FGameplayTag>& TagMapping : AfterGameplayTags::MovementModeTagMap)
        {
            if (TagMapping.Value.IsValid())
            {
                AfterASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
            }
        }

        for (const TPair<uint8, FGameplayTag>& TagMapping : AfterGameplayTags::CustomMovementModeTagMap)
        {
            if (TagMapping.Value.IsValid())
            {
                AfterASC->SetLooseGameplayTagCount(TagMapping.Value, 0);
            }
        }

        UAfterCharacterMovementComponent* AfterMoveComp = CastChecked<UAfterCharacterMovementComponent>(GetCharacterMovement());
        SetMovementModeTag(AfterMoveComp->MovementMode, AfterMoveComp->CustomMovementMode, true);
    }
}

void AAfterCharacter::OnDeathStarted(AActor* OwningActor)
{
    DisableMovementAndCollision();
}

void AAfterCharacter::OnDeathFinished(AActor* OwningActor)
{
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AAfterCharacter::DisableMovementAndCollision()
{
    if (Controller)
    {
        Controller->SetIgnoreMoveInput(true);
    }

    UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
    check(CapsuleComp);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

    UAfterCharacterMovementComponent* AfterMoveComp = CastChecked<UAfterCharacterMovementComponent>(GetCharacterMovement());
    AfterMoveComp->StopMovementImmediately();
    AfterMoveComp->DisableMovement();
}

void AAfterCharacter::DestroyDueToDeath()
{
    K2_OnDeathFinished();

    UninitAndDestroy();
}

void AAfterCharacter::UninitAndDestroy()
{
    if (GetLocalRole() == ROLE_Authority)
    {
        DetachFromControllerPendingDestroy();
        SetLifeSpan(0.1f);
    }

    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        if (AfterASC->GetAvatarActor() == this)
        {
            PawnExtComponent->UninitializeAbilitySystem();
        }
    }

    SetActorHiddenInGame(true);
}

void AAfterCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
    Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

    UAfterCharacterMovementComponent* AfterMoveComp = CastChecked<UAfterCharacterMovementComponent>(GetCharacterMovement());

    SetMovementModeTag(PrevMovementMode, PreviousCustomMode, false);
    SetMovementModeTag(AfterMoveComp->MovementMode, AfterMoveComp->CustomMovementMode, true);
}

void AAfterCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        const FGameplayTag* MovementModeTag = nullptr;
        if (MovementMode == MOVE_Custom)
        {
            MovementModeTag = AfterGameplayTags::CustomMovementModeTagMap.Find(CustomMovementMode);
        }
        else
        {
            MovementModeTag = AfterGameplayTags::MovementModeTagMap.Find(MovementMode);
        }

        if (MovementModeTag && MovementModeTag->IsValid())
        {
            AfterASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
        }
    }
}

void AAfterCharacter::ToggleCrouch()
{
    const UAfterCharacterMovementComponent* AfterMoveComp = CastChecked<UAfterCharacterMovementComponent>(GetCharacterMovement());

    if (bIsCrouched || AfterMoveComp->bWantsToCrouch)
    {
        UnCrouch();
    }
    else if (AfterMoveComp->IsMovingOnGround())
    {
        Crouch();
    }
}

void AAfterCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->SetLooseGameplayTagCount(AfterGameplayTags::Status_Crouching, 1);
    }

    Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AAfterCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->SetLooseGameplayTagCount(AfterGameplayTags::Status_Crouching, 0);
    }

    Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool AAfterCharacter::CanJumpInternal_Implementation() const
{
    return JumpIsAllowedInternal();
}

FVector AAfterCharacter::GetTracePointStart(FVector& ViewLocation, FRotator& ViewRotation) const
{
    const FVector HeadLocation = GetMesh()->GetBoneLocation(FName("Head"));
    const FVector VectorCameraToHead = HeadLocation - ViewLocation;
    const FVector ProjectionOnViewVector = VectorCameraToHead.ProjectOnToNormal(ViewRotation.Vector());

    const FVector Out = ViewLocation + ProjectionOnViewVector;

    DrawDebugLine(GetWorld(), HeadLocation, Out, FColor::Blue, false);

    return Out;
}

void AAfterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
    if (OverlappedItemCount + Amount <= 0)
    {
        OverlappedItemCount = 0;
        bShouldTraceForItems = false;
    }
    else
    {
        OverlappedItemCount += Amount;
        bShouldTraceForItems = true;
    }
}

void AAfterCharacter::SelectButtonPressed(const FInputActionValue& InputActionValue)
{
    if (!TraceHitItem) return;

    if (auto TraceHitItemDefinition = Cast<AItem>(TraceHitItem))
    {
        if (UAfterPickupDefinition* PickupDefinition = TraceHitItemDefinition->GetPickupDefinition())
        {
            GiveWeapon(PickupDefinition->InventoryItemDefinition, this);
        }
    }
}

bool AAfterCharacter::TraceResult(FHitResult& OutHitResult, FVector& OutHitLocation)
{
    FVector ViewLocation;
    FRotator ViewRotation;
    Controller->GetPlayerViewPoint(ViewLocation, ViewRotation);

    const FVector Start = GetTracePointStart(ViewLocation, ViewRotation);
    const FVector End = Start + ViewRotation.Vector() * 5'000.f;
    OutHitLocation = End;

    GetWorld()->LineTraceSingleByChannel(OutHitResult, Start, End, ECollisionChannel::ECC_Visibility);
    if (OutHitResult.bBlockingHit)
    {
        OutHitLocation = OutHitResult.Location;
        return true;
    }

    return false;
}

void AAfterCharacter::TraceForItems()
{
    if (bShouldTraceForItems)
    {
        FHitResult ItemTraceResult;
        FVector HitLocation;
        if (TraceResult(ItemTraceResult, HitLocation))
        {
            TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
            if (TraceHitItem && TraceHitItem->GetPickupWidget())
            {
                TraceHitItem->GetPickupWidget()->SetVisibility(true);
            }

            if (TraceHitItemLastFrame && TraceHitItem != TraceHitItemLastFrame)
            {
                TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
            }

            TraceHitItemLastFrame = TraceHitItem;
        }
    }
    else if (TraceHitItemLastFrame)
    {
        TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
    }
}

//! TODO: implement in Blueprints.
void AAfterCharacter::OnGraundLanded(const FHitResult& Hit)
{
    const auto FallVelocityZ = -GetCharacterMovement()->Velocity.Z;

    if (FallVelocityZ < LandedVelocityDamage.X) return;

    const auto FallDamage = FMath::GetMappedRangeValueClamped(LandedVelocityDamage, LandedDamage, FallVelocityZ);
}
