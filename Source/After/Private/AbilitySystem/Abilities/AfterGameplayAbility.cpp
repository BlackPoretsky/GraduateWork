#include "AbilitySystem/Abilities/AfterGameplayAbility.h"

#include "AfterLogChannels.h"
#include "AfterGameplayTags.h"

#include "Player/AfterPlayerController.h"
#include "Character/AfterCharacter.h"
#include "Character/AfterHeroComponent.h"
#include "Camera/AfterCameraMode.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "AbilitySystem/AfterAbilitySystemGlobals.h"
#include "AbilitySystem/Abilities/AfterAbilityCost.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AfterAbilitySourceInterface.h"
#include "AbilitySystem/AfterGameplayEffectContext.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "AbilitySystemLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameplayAbility)

#define ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(FunctionName, ReturnValue)                                                             \
    {                                                                                                                                   \
        if (!ensure(IsInstantiated()))                                                                                                  \
        {                                                                                                                               \
            ABILITY_LOG(Error, TEXT("%s: " #FunctionName " cannot be called on a non-instanced ability. Check the instancing policy."), \
                *GetPathName());                                                                                                        \
            return ReturnValue;                                                                                                         \
        }                                                                                                                               \
    }

UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_SIMPLE_FAILURE_MESSAGE, "Ability.UserFacingSimpleActivateFail.Message");
UE_DEFINE_GAMEPLAY_TAG(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, "Ability.PlayMontageOnActivateFail.Message");

UAfterGameplayAbility::UAfterGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;

    ActivationPolicy = EAfterAbilityActivationPolicy::OnInputTriggered;
    ActivationGroup = EAfterAbilityActivationGroup::Independent;

    bLogCancelation = false;

    ActiveCameraMode = nullptr;
}

UAfterAbilitySystemComponent* UAfterGameplayAbility::GetAfterAbilitySystemComponentFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<UAfterAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get()) : nullptr);
}

AAfterPlayerController* UAfterGameplayAbility::GetAfterPlayerControllerFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AAfterPlayerController>(CurrentActorInfo->PlayerController.Get()) : nullptr);
}

AController* UAfterGameplayAbility::GetControllerFromActorInfo() const
{
    if (CurrentActorInfo)
    {
        if (AController* PC = CurrentActorInfo->PlayerController.Get())
        {
            return PC;
        }

        AActor* TestActor = CurrentActorInfo->OwnerActor.Get();
        while (TestActor)
        {
            if (AController* C = Cast<AController>(TestActor))
            {
                return C;
            }

            if (APawn* Pawn = Cast<APawn>(TestActor))
            {
                return Pawn->GetController();
            }

            TestActor = TestActor->GetOwner();
        }
    }

    return nullptr;
}

AAfterCharacter* UAfterGameplayAbility::GetAfterCharacterFromActorInfo() const
{
    return (CurrentActorInfo ? Cast<AAfterCharacter>(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

UAfterHeroComponent* UAfterGameplayAbility::GetHeroComponentFromActorInfo() const
{
    return (CurrentActorInfo ? UAfterHeroComponent::FindHeroComponent(CurrentActorInfo->AvatarActor.Get()) : nullptr);
}

void UAfterGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
    const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);

    if (ActorInfo && !Spec.IsActive() && !bIsPredicting && (ActivationPolicy == EAfterAbilityActivationPolicy::OnSpawn))
    {
        UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
        const AActor* AvatarActor = ActorInfo->AvatarActor.Get();

        if (ASC && AvatarActor && !AvatarActor->GetTearOff() && (AvatarActor->GetLifeSpan() <= 0.0f))
        {
            const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) ||
                                           (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
            const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) ||
                                            (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

            const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
            const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

            if (bClientShouldActivate || bServerShouldActivate)
            {
                ASC->TryActivateAbility(Spec.Handle);
            }
        }
    }
}

bool UAfterGameplayAbility::CanChangeActivationGroup(EAfterAbilityActivationGroup NewGroup) const
{
    if (!IsInstantiated() || !IsActive())
    {
        return false;
    }

    if (ActivationGroup == NewGroup)
    {
        return true;
    }

    UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponentFromActorInfo();
    check(AfterASC);

    if ((ActivationGroup != EAfterAbilityActivationGroup::Exclusive_Blocking) && AfterASC->IsActivationGroupBlocked(NewGroup))
    {
        return false;
    }

    if ((NewGroup == EAfterAbilityActivationGroup::Exclusive_Replaceable) && !CanBeCanceled())
    {
        return false;
    }

    return true;
}

bool UAfterGameplayAbility::ChangeActivationGroup(EAfterAbilityActivationGroup NewGroup)
{
    ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ChangeActivationGroup, false);

    if (!CanChangeActivationGroup(NewGroup))
    {
        return false;
    }

    if (ActivationGroup != NewGroup)
    {
        UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponentFromActorInfo();
        check(AfterASC);

        AfterASC->RemoveAbilityFromActivationGroup(ActivationGroup, this);
        AfterASC->AddAbilityToActivationGroup(NewGroup, this);

        ActivationGroup = NewGroup;
    }

    return true;
}

void UAfterGameplayAbility::SetCameraMode(TSubclassOf<UAfterCameraMode> CameraMode)
{
    ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(SetCameraMode, );

    if (UAfterHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
    {
        HeroComponent->SetAbilityCameraMode(CameraMode, CurrentSpecHandle);
        ActiveCameraMode = CameraMode;
    }
}

void UAfterGameplayAbility::ClearCameraMode()
{
    ENSURE_ABILITY_IS_INSTANTIATED_OR_RETURN(ClearCameraMode, );

    if (ActiveCameraMode)
    {
        if (UAfterHeroComponent* HeroComponent = GetHeroComponentFromActorInfo())
        {
            HeroComponent->ClearAbilityCameraMode(CurrentSpecHandle);
        }

        ActiveCameraMode = nullptr;
    }
}

void UAfterGameplayAbility::NativeOnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
{
    bool bSimpleFailureFound = false;
    for (FGameplayTag Reason : FailedReason)
    {
        if (UAnimMontage* pMontage = FailureTagToAnimMontage.FindRef(Reason))
        {
            FAfterAbilityMontageFailureMessage Message;
            Message.PlayerController = GetActorInfo().PlayerController.Get();
            Message.FailureTags = FailedReason;
            Message.FailureMontage = pMontage;

            UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetWorld());
            MessageSystem.BroadcastMessage(TAG_ABILITY_PLAY_MONTAGE_FAILURE_MESSAGE, Message);
        }
    }
}

bool UAfterGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
    {
        return false;
    }

    if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
    {
        return false;
    }

    UAfterAbilitySystemComponent* AfterASC = CastChecked<UAfterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
    if (AfterASC->IsActivationGroupBlocked(ActivationGroup))
    {
        if (OptionalRelevantTags)
        {
            OptionalRelevantTags->AddTag(AfterGameplayTags::Ability_ActivateFail_ActivationGroup);
        }
        return false;
    }

    return true;
}

void UAfterGameplayAbility::SetCanBeCanceled(bool bCanBeCanceled)
{
    if (!bCanBeCanceled && (ActivationGroup == EAfterAbilityActivationGroup::Exclusive_Replaceable))
    {
        UE_LOG(LogAfterAbilitySystem, Error,
            TEXT("SetCanBeCanceled: Ability [%s] can not block canceling because its activation group is replaceable."), *GetName());
        return;
    }

    Super::SetCanBeCanceled(bCanBeCanceled);
}

void UAfterGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    K2_OnAbilityAdded();

    TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void UAfterGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    K2_OnAbilityRemoved();

    Super::OnRemoveAbility(ActorInfo, Spec);
}

void UAfterGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAfterGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    ClearCameraMode();

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAfterGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
    {
        return false;
    }

    for (TObjectPtr<UAfterAbilityCost> AdditionalCost : AdditionalCosts)
    {
        if (AdditionalCost != nullptr)
        {
            if (!AdditionalCost->CheckCost(this, Handle, ActorInfo, OptionalRelevantTags))
            {
                return false;
            }
        }
    }

    return true;
}

void UAfterGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo) const
{
    Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

    check(ActorInfo);

    auto DetermineIfAbilityHitTarget = [&]()
    {
        if (ActorInfo->IsNetAuthority())
        {
            if (UAfterAbilitySystemComponent* ASC = Cast<UAfterAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get()))
            {
                FGameplayAbilityTargetDataHandle TargetData;
                ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
                for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
                {
                    if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    };

    bool bAbilityHitTarget = false;
    bool bHasDeterminedIfAbilityHitTarget = false;
    for (TObjectPtr<UAfterAbilityCost> AdditionalCost : AdditionalCosts)
    {
        if (AdditionalCost != nullptr)
        {
            if (AdditionalCost->ShouldOnlyApplyCostOnHit())
            {
                if (!bHasDeterminedIfAbilityHitTarget)
                {
                    bAbilityHitTarget = DetermineIfAbilityHitTarget();
                    bHasDeterminedIfAbilityHitTarget = true;
                }

                if (!bAbilityHitTarget)
                {
                    continue;
                }
            }

            AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
        }
    }
}

FGameplayEffectContextHandle UAfterGameplayAbility::MakeEffectContext(
    const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const
{
    FGameplayEffectContextHandle ContextHandle = Super::MakeEffectContext(Handle, ActorInfo);

    FAfterGameplayEffectContext* EffectContext = FAfterGameplayEffectContext::ExtractEffectContext(ContextHandle);
    check(EffectContext);

    check(ActorInfo);

    AActor* EffectCauser = nullptr;
    const IAfterAbilitySourceInterface* AbilitySource = nullptr;
    float SourceLevel = 0.0f;
    GetAbilitySource(Handle, ActorInfo, SourceLevel, AbilitySource, EffectCauser);

    UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

    AActor* Instigator = ActorInfo ? ActorInfo->OwnerActor.Get() : nullptr;

    EffectContext->SetAbilitySource(AbilitySource, SourceLevel);
    EffectContext->AddInstigator(Instigator, EffectCauser);
    EffectContext->AddSourceObject(SourceObject);

    return ContextHandle;
}

void UAfterGameplayAbility::ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const
{
    Super::ApplyAbilityTagsToGameplayEffectSpec(Spec, AbilitySpec);
}

bool UAfterGameplayAbility::DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bBlocked = false;
    bool bMissing = false;

    UAbilitySystemGlobals& AbilitySystemGlobals = UAbilitySystemGlobals::Get();
    const FGameplayTag& BlockedTag = AbilitySystemGlobals.ActivateFailTagsBlockedTag;
    const FGameplayTag& MissingTag = AbilitySystemGlobals.ActivateFailTagsMissingTag;

    // Check if any of this ability's tags are currently blocked
    if (AbilitySystemComponent.AreAbilityTagsBlocked(AbilityTags))
    {
        bBlocked = true;
    }

    const UAfterAbilitySystemComponent* AfterASC = Cast<UAfterAbilitySystemComponent>(&AbilitySystemComponent);
    static FGameplayTagContainer AllRequiredTags;
    static FGameplayTagContainer AllBlockedTags;

    AllRequiredTags = ActivationRequiredTags;
    AllBlockedTags = ActivationBlockedTags;

    if (AfterASC)
    {
        AfterASC->GetAdditionalActivationTagRequirements(AbilityTags, AllRequiredTags, AllBlockedTags);
    }

    if (AllBlockedTags.Num() || AllRequiredTags.Num())
    {
        static FGameplayTagContainer AbilitySystemComponentTags;

        AbilitySystemComponentTags.Reset();
        AbilitySystemComponent.GetOwnedGameplayTags(AbilitySystemComponentTags);

        if (AbilitySystemComponentTags.HasAny(AllBlockedTags))
        {
            if (OptionalRelevantTags && AbilitySystemComponentTags.HasTag(AfterGameplayTags::Status_Death))
            {
                OptionalRelevantTags->AddTag(AfterGameplayTags::Ability_ActivateFail_IsDead);
            }

            bBlocked = true;
        }

        if (!AbilitySystemComponentTags.HasAll(AllRequiredTags))
        {
            bMissing = true;
        }
    }

    if (SourceTags != nullptr)
    {
        if (SourceBlockedTags.Num() || SourceRequiredTags.Num())
        {
            if (SourceTags->HasAny(SourceBlockedTags))
            {
                bBlocked = true;
            }

            if (!SourceTags->HasAll(SourceRequiredTags))
            {
                bMissing = true;
            }
        }
    }

    if (TargetTags != nullptr)
    {
        if (TargetBlockedTags.Num() || TargetRequiredTags.Num())
        {
            if (TargetTags->HasAny(TargetBlockedTags))
            {
                bBlocked = true;
            }

            if (!TargetTags->HasAll(TargetRequiredTags))
            {
                bMissing = true;
            }
        }
    }

    if (bBlocked)
    {
        if (OptionalRelevantTags && BlockedTag.IsValid())
        {
            OptionalRelevantTags->AddTag(BlockedTag);
        }
        return false;
    }
    if (bMissing)
    {
        if (OptionalRelevantTags && MissingTag.IsValid())
        {
            OptionalRelevantTags->AddTag(MissingTag);
        }
        return false;
    }

    return true;
}

void UAfterGameplayAbility::OnPawnAvatarSet()
{
    K2_OnPawnAvatarSet();
}

void UAfterGameplayAbility::GetAbilitySource(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    float& OutSourceLevel, const IAfterAbilitySourceInterface*& OutAbilitySource, AActor*& OutEffectCauser) const
{
    OutSourceLevel = 0.0f;
    OutAbilitySource = nullptr;
    OutEffectCauser = nullptr;

    OutEffectCauser = ActorInfo->AvatarActor.Get();

    UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

    OutAbilitySource = Cast<IAfterAbilitySourceInterface>(SourceObject);
}
