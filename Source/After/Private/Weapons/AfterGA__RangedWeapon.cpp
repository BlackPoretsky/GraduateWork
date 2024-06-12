#include "Weapons/AfterGA__RangedWeapon.h"
#include "Weapons/AfterRangedWeaponInstance.h"
#include "Physics/AfterCollisionChannels.h"
#include "AfterLogChannels.h"
#include "NativeGameplayTags.h"
#include "Weapons/AfterWeaponStateComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AfterGATData_SingleTargetHit.h"
#include "DrawDebugHelpers.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGA__RangedWeapon)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_WeaponFireBlocked, "Ability.Weapon.NoFiring")

FVector VRandConeNormalDistribution(const FVector& Dir, const float ConeHalfAngleRad, const float Exponent)
{
    if (ConeHalfAngleRad > 0.f)
    {
        const float ConeHalfAngleDegrees = FMath::RadiansToDegrees(ConeHalfAngleRad);

        const float FromCenter = FMath::Pow(FMath::FRand(), Exponent);
        const float AngleFromCenter = FromCenter * ConeHalfAngleDegrees;
        const float AngleAround = FMath::FRand() * 360.0f;

        FRotator Rot = Dir.Rotation();
        FQuat DirQuat(Rot);
        FQuat FromCenterQuat(FRotator(0.0f, AngleFromCenter, 0.0f));
        FQuat AroundQuat(FRotator(0.0f, 0.0, AngleAround));
        FQuat FinalDirectionQuat = DirQuat * AroundQuat * FromCenterQuat;
        FinalDirectionQuat.Normalize();

        return FinalDirectionQuat.RotateVector(FVector::ForwardVector);
    }
    else
    {
        return Dir.GetSafeNormal();
    }
}

UAfterGA__RangedWeapon::UAfterGA__RangedWeapon(const FObjectInitializer& ObjectIntializer)
    : Super(ObjectIntializer)
{
    SourceBlockedTags.AddTag(TAG_WeaponFireBlocked);
}

UAfterRangedWeaponInstance* UAfterGA__RangedWeapon::GetWeaponInstance() const
{
    return Cast<UAfterRangedWeaponInstance>(GetAssociatedEquipment());
}

bool UAfterGA__RangedWeapon::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

    if (bResult)
    {
        if (GetWeaponInstance() == nullptr)
        {
            UE_LOG(LogAfterAbilitySystem, Error,
                TEXT("Weapon ability %s cannot be activated because there is no associated ranged weapon (equipment instance=%s but needs "
                     "to be derived from %s)"),
                *GetPathName(), *GetPathNameSafe(GetAssociatedEquipment()), *UAfterRangedWeaponInstance::StaticClass()->GetName());
            bResult = false;
        }
    }

    return bResult;
}

void UAfterGA__RangedWeapon::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    check(MyAbilityComponent);

    UAfterRangedWeaponInstance* WeaponData = GetWeaponInstance();
    check(WeaponData);
    WeaponData->UpdateFiringTime();

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAfterGA__RangedWeapon::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    if (IsEndAbilityValid(Handle, ActorInfo))
    {
        if (ScopeLockCount > 0)
        {
            WaitingToExecute.Add(FPostLockDelegate::CreateUObject(
                this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
            return;
        }

        UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
        check(MyAbilityComponent);

        MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey())
            .Remove(OnTargetDataReadyCallbackDelegateHandle);
        MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

        Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
    }
}

int32 UAfterGA__RangedWeapon::FindFirstPawnHitResult(const TArray<FHitResult>& HitResults)
{
    for (int32 Idx = 0; Idx < HitResults.Num(); ++Idx)
    {
        const FHitResult& CurHitResult = HitResults[Idx];
        if (CurHitResult.HitObjectHandle.DoesRepresentClass(APawn::StaticClass()))
        {
            return Idx;
        }
        else
        {
            AActor* HitActor = CurHitResult.HitObjectHandle.FetchActor();
            if ((HitActor != nullptr) && (HitActor->GetAttachParentActor() != nullptr) &&
                (Cast<APawn>(HitActor->GetAttachParentActor()) != nullptr))
            {
                return Idx;
            }
        }
    }

    return INDEX_NONE;
}

FHitResult UAfterGA__RangedWeapon::WeaponTrace(
    const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHitResults) const
{
    TArray<FHitResult> HitResults;

    FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/true, /*IgnoreActor=*/GetAvatarActorFromActorInfo());
    TraceParams.bReturnPhysicalMaterial = true;
    AddAdditionalTraceIgnoreActors(TraceParams);

    const ECollisionChannel TraceChannel = DetermineTraceChannel(TraceParams, bIsSimulated);

    if (SweepRadius > 0.0f)
    {
        GetWorld()->SweepMultiByChannel(
            HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(SweepRadius), TraceParams);
    }
    else
    {
        GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
    }

    FHitResult Hit(ForceInit);
    if (HitResults.Num() > 0)
    {
        for (FHitResult& CurHitResult : HitResults)
        {
            auto Pred = [&CurHitResult](const FHitResult& Other) { return Other.HitObjectHandle == CurHitResult.HitObjectHandle; };

            if (!OutHitResults.ContainsByPredicate(Pred))
            {
                OutHitResults.Add(CurHitResult);
            }
        }

        Hit = OutHitResults.Last();
    }
    else
    {
        Hit.TraceStart = StartTrace;
        Hit.TraceEnd = EndTrace;
    }

    return Hit;
}

FHitResult UAfterGA__RangedWeapon::DoSingleBulletTrace(
    const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const
{
    FHitResult Impact;

    if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
    {
        Impact = WeaponTrace(StartTrace, EndTrace, 0.0f, bIsSimulated, OutHits);
    }

    if (FindFirstPawnHitResult(OutHits) == INDEX_NONE)
    {
        if (SweepRadius > 0.0f)
        {
            TArray<FHitResult> SweepHits;
            Impact = WeaponTrace(StartTrace, EndTrace, SweepRadius, bIsSimulated, SweepHits);

            const int32 FirstPawnIdx = FindFirstPawnHitResult(SweepHits);
            if (SweepHits.IsValidIndex(FirstPawnIdx))
            {
                bool bUseSweepHits = true;
                for (int32 Idx = 0; Idx < FirstPawnIdx; ++Idx)
                {
                    const FHitResult& CurHitResult = SweepHits[Idx];

                    auto Pred = [&CurHitResult](const FHitResult& Other) { return Other.HitObjectHandle == CurHitResult.HitObjectHandle; };
                    if (CurHitResult.bBlockingHit && OutHits.ContainsByPredicate(Pred))
                    {
                        bUseSweepHits = false;
                        break;
                    }
                }

                if (bUseSweepHits)
                {
                    OutHits = SweepHits;
                }
            }
        }
    }

    DrawDebugLine(GetWorld(), Impact.TraceStart, Impact.ImpactPoint, FColor::Red, false, 3.f, 0, 3.f);

    return Impact;
}

void UAfterGA__RangedWeapon::TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits)
{
    UAfterRangedWeaponInstance* WeaponData = InputData.WeaponData;
    check(WeaponData);

    const int32 BulletsPerCartridge = WeaponData->GetBulletsPerCartridge();

    for (int32 BulletIndex = 0; BulletIndex < BulletsPerCartridge; ++BulletIndex)
    {
        const float SpreadAngleMultiplier = WeaponData->GetCalculatedSpreadAngleMultiplier();
        const float ActualSpreadAngle = SpreadAngleMultiplier;

        const float HalfSpreadAngleInRadians = FMath::DegreesToRadians(ActualSpreadAngle * 0.5f);

        const FVector BulletDir = VRandConeNormalDistribution(InputData.AimDir, HalfSpreadAngleInRadians, WeaponData->GetSpreadExponent());

        const FVector EndTrace = InputData.StartTrace + (BulletDir * WeaponData->GetMaxDamageRange());
        FVector HitLocation = EndTrace;

        TArray<FHitResult> AllImpacts;

        FHitResult Impact = DoSingleBulletTrace(InputData.StartTrace, EndTrace, WeaponData->GetBulletTraceSweepRadius(), false, AllImpacts);

        const AActor* HitActor = Impact.GetActor();

        if (HitActor)
        {
            if (AllImpacts.Num() > 0)
            {
                OutHits.Append(AllImpacts);
            }

            HitLocation = Impact.ImpactPoint;
        }

        if (OutHits.Num() == 0)
        {
            if (!Impact.bBlockingHit)
            {
                Impact.Location = EndTrace;
                Impact.ImpactPoint = EndTrace;
            }

            OutHits.Add(Impact);
        }
    }
}

void UAfterGA__RangedWeapon::AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const
{
    if (AActor* Avatar = GetAvatarActorFromActorInfo())
    {
        TArray<AActor*> AttachedActors;
        Avatar->GetAttachedActors(AttachedActors);
        TraceParams.AddIgnoredActors(AttachedActors);
    }
}

ECollisionChannel UAfterGA__RangedWeapon::DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const
{
    return After_TraceChannel_Weapon;
}

void UAfterGA__RangedWeapon::PerformLocalTargeting(OUT TArray<FHitResult>& OutHits)
{
    APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

    UAfterRangedWeaponInstance* WeaponData = GetWeaponInstance();
    if (AvatarPawn && AvatarPawn->IsLocallyControlled() && WeaponData)
    {
        FRangedWeaponFiringInput InputData;
        InputData.WeaponData = WeaponData;
        InputData.bCanPlayBulletFX = (AvatarPawn->GetNetMode() != NM_DedicatedServer);

        const FTransform TargetTransform = GetTargetingTransform(AvatarPawn, EAfterAbilityTargetingSource::CameraTowardsFocus);
        InputData.AimDir = TargetTransform.GetUnitAxis(EAxis::X);
        InputData.StartTrace = TargetTransform.GetTranslation();

        InputData.EndAim = InputData.StartTrace + InputData.AimDir * WeaponData->GetMaxDamageRange();

        TraceBulletsInCartridge(InputData, OutHits);
    }
}

FVector UAfterGA__RangedWeapon::GetWeaponTargetingSourceLocation() const
{
    APawn* const AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
    check(AvatarPawn);

    const FVector SourceLoc = AvatarPawn->GetActorLocation();
    const FQuat SourceRot = AvatarPawn->GetActorQuat();

    FVector TargetingSourceLocation = SourceLoc;

    return TargetingSourceLocation;
}

FTransform UAfterGA__RangedWeapon::GetTargetingTransform(APawn* SourcePawn, EAfterAbilityTargetingSource Source) const
{
    check(SourcePawn);
    AController* SourcePawnController = SourcePawn->GetController();
    UAfterWeaponStateComponent* WeaponStateComponent =
        (SourcePawnController != nullptr) ? SourcePawnController->FindComponentByClass<UAfterWeaponStateComponent>() : nullptr;

    check(Source != EAfterAbilityTargetingSource::Custom);

    const FVector ActorLoc = SourcePawn->GetActorLocation();
    FQuat AimQuat = SourcePawn->GetActorQuat();
    AController* Controller = SourcePawn->Controller;
    FVector SourceLoc;

    double FocalDistance = 1024.0f;
    FVector FocalLoc;

    FVector CamLoc;
    FRotator CamRot;
    bool bFoundFocus = false;

    if ((Controller != nullptr) &&
        ((Source == EAfterAbilityTargetingSource::CameraTowardsFocus) || (Source == EAfterAbilityTargetingSource::PawnTowardsFocus) ||
            (Source == EAfterAbilityTargetingSource::WeaponTowardsFocus)))
    {
        bFoundFocus = true;

        APlayerController* PC = Cast<APlayerController>(Controller);
        if (PC != nullptr)
        {
            PC->GetPlayerViewPoint(/*out*/ CamLoc, /*out*/ CamRot);
        }
        else
        {
            SourceLoc = GetWeaponTargetingSourceLocation();
            CamLoc = SourceLoc;
            CamRot = Controller->GetControlRotation();
        }

        FVector AimDir = CamRot.Vector().GetSafeNormal();
        FocalLoc = CamLoc + (AimDir * FocalDistance);

        if (PC)
        {
            const FVector WeaponLoc = GetWeaponTargetingSourceLocation();
            CamLoc = FocalLoc + (((WeaponLoc - FocalLoc) | AimDir) * AimDir);
            FocalLoc = CamLoc + (AimDir * FocalDistance);
        }

        if (Source == EAfterAbilityTargetingSource::CameraTowardsFocus)
        {
            return FTransform(CamRot, CamLoc);
        }
    }

    if ((Source == EAfterAbilityTargetingSource::WeaponForward) || (Source == EAfterAbilityTargetingSource::WeaponTowardsFocus))
    {
        SourceLoc = GetWeaponTargetingSourceLocation();
    }
    else
    {
        SourceLoc = ActorLoc;
    }

    if (bFoundFocus &&
        ((Source == EAfterAbilityTargetingSource::PawnTowardsFocus) || (Source == EAfterAbilityTargetingSource::WeaponTowardsFocus)))
    {
        return FTransform((FocalLoc - SourceLoc).Rotation(), SourceLoc);
    }

    return FTransform(AimQuat, SourceLoc);
}

void UAfterGA__RangedWeapon::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
    UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    check(MyAbilityComponent);

    if (const FGameplayAbilitySpec* AbilitySpec = MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
    {
        FScopedPredictionWindow ScopedPrediction(MyAbilityComponent);

        FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

        if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
        {
            OnRangedWeaponTargetDataReady(LocalTargetDataHandle);
        }
        else
        {
            UE_LOG(LogAfterAbilitySystem, Warning, TEXT("Weapon ability %s failed to commit."), *GetPathName());
            K2_EndAbility();
        }
    }
}

void UAfterGA__RangedWeapon::StartRangedWeaponTargeting()
{
    check(CurrentActorInfo);

    AActor* AvatarActor = CurrentActorInfo->AvatarActor.Get();
    check(AvatarActor);

    UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
    check(MyAbilityComponent);

    AController* Controller = GetControllerFromActorInfo();
    check(Controller);

    FScopedPredictionWindow ScopedPrediction(MyAbilityComponent, CurrentActivationInfo.GetActivationPredictionKey());

    TArray<FHitResult> FoundHits;
    PerformLocalTargeting(FoundHits);

    FGameplayAbilityTargetDataHandle TargetData;

    if (FoundHits.Num() > 0)
    {
        const int32 CartridgeID = FMath::Rand();

        for (const FHitResult& FoundHit : FoundHits)
        {
            FAfterGATData_SingleTargetHit* NewTargetData = new FAfterGATData_SingleTargetHit();
            NewTargetData->HitResult = FoundHit;
            NewTargetData->CartridgeID = CartridgeID;

            TargetData.Add(NewTargetData);
        }
    }

    OnTargetDataReadyCallback(TargetData, FGameplayTag());
}
