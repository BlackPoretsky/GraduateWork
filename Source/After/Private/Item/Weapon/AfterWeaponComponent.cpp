#include "Weapon/AfterWeaponComponent.h"

#include "Weapon/Weapon.h"
#include "Weapon/RangedWeapon.h"
#include "Weapon/MeleeWeapon.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Animation/AfterEquipFinishedAnimNotify.h"
#include "Animation/AfterReloadFinishedAnimNotify.h"
#include "Animation/AnimUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

constexpr static int32 WeaponNum = 3;

UAfterWeaponComponent::UAfterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAfterWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    Arsenal.Init(nullptr, WeaponNum);
    SpawnDefaultArsenal();

    int32 ValidWeaponIndex = Arsenal.IndexOfByPredicate([](const AWeapon* Weapon) { return Weapon; });
    CurrentWeaponIndex = ValidWeaponIndex >= 0 ? ValidWeaponIndex : 0;

    InitEquipAnimation(EquipAnimMontage);
    EquipWeapon(CurrentWeaponIndex);
}

void UAfterWeaponComponent::StartAttacking()
{
    if (!CanAttack()) return;

    bAttack = true;
    CurrentWeapon->StartAttacking();
}

void UAfterWeaponComponent::StopAttacking()
{
    if (!CurrentWeapon) return;

    bAttack = false;
    CurrentWeapon->StopAttacking();
}

void UAfterWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    CurrentWeapon = nullptr;
    for (auto Weapon : Arsenal)
    {
        if (!Weapon) continue;

        Weapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        Weapon->Destroy();
    }
    Arsenal.Empty();

    Super::EndPlay(EndPlayReason);
}

void UAfterWeaponComponent::SpawnDefaultArsenal()
{
    if (!GetWorld() || !GetOwner()) return;

    auto Weapons = GetDefaultArsenal();
    for (int32 Index = 0; Index != Weapons.Num(); ++Index)
    {
        if (!Weapons[Index]) continue;

        auto Weapon = GetWorld()->SpawnActor<AWeapon>(Weapons[Index]);
        if (!Weapon) continue;

        AddWeaponToArsenal(Weapon, Index);
    }
}

void UAfterWeaponComponent::AddWeaponToArsenal(AWeapon* NewWeapon, int32 AddIndex)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (auto WeaponCast = Cast<ARangedWeapon>(NewWeapon))
    {
        WeaponCast->OnClipEmpty.AddUObject(this, &UAfterWeaponComponent::OnEmptyClip);
        InitReloadAnimation(WeaponCast->GetAnimations().ReloadAnimMotage);
    }

    NewWeapon->SetItemState(EItemState::EIS_Equipped);
    NewWeapon->SetOwner(Character);
    NewWeapon->OnPlayAnimMontage.AddUObject(this, &UAfterWeaponComponent::PlayAnimMontage);
    Arsenal[AddIndex] = NewWeapon;

    AttachWeaponToSocket(NewWeapon, Character->GetMesh(), NewWeapon->GetWeaponSocketName());
}

void UAfterWeaponComponent::OrganizeArsenal(AWeapon* WeaponToPickup, int32 WeaponIndex)
{
    if (CurrentWeapon && CurrentWeaponIndex == WeaponIndex)
    {
        DropWeapon();
        AddWeaponToArsenal(WeaponToPickup, WeaponIndex);
        EquipWeapon(WeaponIndex);
    }
    else if (!CurrentWeapon && CurrentWeaponIndex == WeaponIndex)
    {
        AddWeaponToArsenal(WeaponToPickup, WeaponIndex);
        EquipWeapon(WeaponIndex);
    }
    else if (Arsenal[WeaponIndex])
    {
        DropWeapon(WeaponIndex);
        AddWeaponToArsenal(WeaponToPickup, WeaponIndex);
    }
    else
    {
        AddWeaponToArsenal(WeaponToPickup, WeaponIndex);
    }
}

void UAfterWeaponComponent::AttachWeaponToSocket(AWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent) return;

    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

TArray<TSubclassOf<AWeapon>> UAfterWeaponComponent::GetDefaultArsenal()
{
    return {PrimaryWeapon, SecondaryWeapon, MeleeWeapon};
}

void UAfterWeaponComponent::EquipWeapon(int32 WeaponIndex)
{
    if (WeaponIndex < 0 || WeaponIndex > Arsenal.Num())
    {
        UE_LOG(LogWeaponComponent, Warning, TEXT("Invalid weapon index!"));
        return;
    }

    if (!Arsenal[WeaponIndex]) return;

    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    if (CurrentWeapon)
    {
        CurrentWeapon->StopAttacking();

        AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), CurrentWeapon->GetWeaponSocketName());
    }

    CurrentWeapon = Arsenal[WeaponIndex];
    CurrentWeaponIndex = WeaponIndex;

    if (auto CurrentWeaponCast = Cast<ARangedWeapon>(CurrentWeapon))
    {
        CurrentReloadAnimMontage = CurrentWeaponCast ? CurrentWeaponCast->GetAnimations().ReloadAnimMotage : nullptr;
    }
    else
    {
        CurrentReloadAnimMontage = nullptr;
    }

    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponEquipSocketName);
    CurrentWeapon->SetItemState(EItemState::EIS_Equipped);
    bEquipAnimInProgress = true;
    PlayAnimMontage(EquipAnimMontage);
}

void UAfterWeaponComponent::InitEquipAnimation(UAnimMontage* EquipAnimation)
{
    auto EquipFinishedNotify = AnimUtils::FindNotifyByClass<UAfterEquipFinishedAnimNotify>(EquipAnimation);
    if (EquipFinishedNotify)
    {
        EquipFinishedNotify->OnNotified.AddUObject(this, &UAfterWeaponComponent::OnEquipFinished);
    }
    else
    {
    }
}

void UAfterWeaponComponent::InitReloadAnimation(UAnimMontage* ReloadAnimation)
{
    auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<UAfterReloadFinishedAnimNotify>(ReloadAnimation);
    if (ReloadFinishedNotify)
    {
        ReloadFinishedNotify->OnNotified.AddUObject(this, &UAfterWeaponComponent::OnReloadFinished);
    }
    else
    {
    }
}

void UAfterWeaponComponent::TerminateReloadAnimation(UAnimMontage* ReloadAnimation)
{
    auto ReloadFinishedNotify = AnimUtils::FindNotifyByClass<UAfterReloadFinishedAnimNotify>(ReloadAnimation);
    if (ReloadFinishedNotify)
    {
        ReloadFinishedNotify->OnNotified.Clear();
    }
}

void UAfterWeaponComponent::CalculatedCrosshairSpread(float DeltaTime)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    FVector2D WalkSpeedRange{0.f, 600.f};
    FVector2D VelocityMultiplierRange{0.f, 1.f};
    FVector Velocity = Character->GetVelocity();
    Velocity.Z = 0.f;

    CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

    if (Character->GetCharacterMovement()->IsFalling())
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
    }
    else
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
    }

    if (bAiming)
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
    }
    else
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
    }

    if (bAttack)
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
    }
    else
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
    }

    CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

bool UAfterWeaponComponent::IsAiming() const
{
    return bAiming;
}

AWeapon* UAfterWeaponComponent::GetCurrentWeapon() const
{
    return CurrentWeapon;
}

float UAfterWeaponComponent::GetCrosshairSpreadMultiplier() const
{
    return CrosshairSpreadMultiplier;
}

void UAfterWeaponComponent::EquipNextWeapon()
{
    if (!CanEquip()) return;

    int32 NextValidIndex = (CurrentWeaponIndex + 1) % WeaponNum;
    while (NextValidIndex != CurrentWeaponIndex)
    {
        if (Arsenal[NextValidIndex])
        {
            EquipWeapon(NextValidIndex);
            return;
        }
        NextValidIndex = (NextValidIndex + 1) % WeaponNum;
    }
}

void UAfterWeaponComponent::EquipPrevWeapon()
{
    if (!CanEquip()) return;

    int32 PrevValidIndex = CurrentWeaponIndex - 1;
    while (PrevValidIndex != CurrentWeaponIndex)
    {
        if (PrevValidIndex < 0) PrevValidIndex = Arsenal.Num() - 1;

        if (Arsenal[PrevValidIndex])
        {
            EquipWeapon(PrevValidIndex);
            return;
        }

        PrevValidIndex -= 1;
    }
}

void UAfterWeaponComponent::PlayAnimMontage(UAnimMontage* Animation)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character) return;

    Character->PlayAnimMontage(Animation);
}

void UAfterWeaponComponent::OnEquipFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;

    bEquipAnimInProgress = false;
}

void UAfterWeaponComponent::OnReloadFinished(USkeletalMeshComponent* MeshComponent)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Character || Character->GetMesh() != MeshComponent) return;
    if (auto RangedCurrentWeapon = Cast<ARangedWeapon>(CurrentWeapon)) RangedCurrentWeapon->ChangeClip();

    bReloadAnimInProgress = false;
}

bool UAfterWeaponComponent::CanAttack() const
{
    return CurrentWeapon && !bEquipAnimInProgress && !bReloadAnimInProgress;
}

bool UAfterWeaponComponent::CanEquip() const
{
    return !bEquipAnimInProgress && !bReloadAnimInProgress;
}

bool UAfterWeaponComponent::CanReload(ARangedWeapon* RangedCurrentWeapon) const
{
    return RangedCurrentWeapon        //
           && !bEquipAnimInProgress   //
           && !bReloadAnimInProgress  //
           && RangedCurrentWeapon->CanReload();
}

void UAfterWeaponComponent::Reload()
{
    ChangeClip();
}

void UAfterWeaponComponent::AimingPressed()
{
    if (!CurrentWeapon || !Cast<ARangedWeapon>(CurrentWeapon)) return;

    bAiming = true;
}

void UAfterWeaponComponent::AimingReleased()
{
    bAiming = false;
}

void UAfterWeaponComponent::DetachWeaponFromCharacter(AWeapon* DetachableWeapon, ACharacter* Character)
{
    DetachableWeapon->SetItemState(EItemState::EIS_Falling);
    DetachableWeapon->ThrowWeapon(Character->GetMesh()->GetForwardVector(),  //
        Character->GetMesh()->GetComponentRotation());

    FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
    DetachableWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
}

void UAfterWeaponComponent::DropWeapon()
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!CurrentWeapon || !Character) return;

    CurrentWeapon->OnPlayAnimMontage.Clear();
    if (auto WeaponCast = Cast<ARangedWeapon>(CurrentWeapon))
    {
        WeaponCast->StopAttacking();
        WeaponCast->OnClipEmpty.Clear();
        TerminateReloadAnimation(WeaponCast->GetAnimations().ReloadAnimMotage);
    }

    CurrentWeapon->SetItemState(EItemState::EIS_Falling);
    DetachWeaponFromCharacter(CurrentWeapon, Character);

    Arsenal[CurrentWeaponIndex] = nullptr;
    CurrentWeapon = nullptr;
    bReloadAnimInProgress = false;
    EquipNextWeapon();
}

void UAfterWeaponComponent::DropWeapon(int32 IndexWeapon)
{
    ACharacter* Character = Cast<ACharacter>(GetOwner());
    if (!Arsenal[IndexWeapon] || !Character) return;

    if (auto WeaponCast = Cast<ARangedWeapon>(Arsenal[IndexWeapon]))
    {
        WeaponCast->OnClipEmpty.Clear();
    }

    Arsenal[IndexWeapon]->SetItemState(EItemState::EIS_Falling);
    DetachWeaponFromCharacter(Arsenal[IndexWeapon], Character);

    Arsenal[IndexWeapon] = nullptr;
}

void UAfterWeaponComponent::PickupWeapon(AWeapon* WeaponToPickup)
{
    if (WeaponToPickup->IsA(AMeleeWeapon::StaticClass()))
    {
        OrganizeArsenal(WeaponToPickup, MELEE_WEAPON_INDEX);
    }
    else if (auto Weapon = Cast<ARangedWeapon>(WeaponToPickup))
    {
        if (Weapon->GetWeaponCategory() == ERangedWeaponCategory::ERWC_Primary)
            OrganizeArsenal(WeaponToPickup, PRIMARY_WEAPON_INDEX);
        else if (Weapon->GetWeaponCategory() == ERangedWeaponCategory::ERWC_Secondary)
            OrganizeArsenal(WeaponToPickup, SECONDARY_WEAPON_INDEX);
    }
}

void UAfterWeaponComponent::EquipPrimaryWeapon()
{
    EquipWeapon(PRIMARY_WEAPON_INDEX);
}

void UAfterWeaponComponent::EquipSecondaryWeapon()
{
    EquipWeapon(SECONDARY_WEAPON_INDEX);
}

void UAfterWeaponComponent::EquipMeleeWeapon()
{
    EquipWeapon(MELEE_WEAPON_INDEX);
}

void UAfterWeaponComponent::OnEmptyClip()
{
    ChangeClip();
}

void UAfterWeaponComponent::ChangeClip()
{
    if (auto RangedCurrentWeapon = Cast<ARangedWeapon>(CurrentWeapon))
    {
        if (!CanReload(RangedCurrentWeapon)) return;

        RangedCurrentWeapon->StopAttacking();
        bReloadAnimInProgress = true;
        PlayAnimMontage(CurrentReloadAnimMontage);
    }
}