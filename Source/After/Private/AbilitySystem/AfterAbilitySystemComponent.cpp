#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AfterGameplayAbility.h"
#include "AbilitySystem/AfterAbilityTagRelationshipMapping.h"

#include "Animation/AfterAnimInstance.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "AbilitySystem/AfterGlobalAbilitySystem.h"
#include "AfterLogChannels.h"
#include "System/AfterAssetManager.h"
#include "System/AfterGameData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAbilitySystemComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_AbilityInputBlocked, "Gameplay.AbilityInputBlocked")

UAfterAbilitySystemComponent::UAfterAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
    InputHeldSpecHandles.Reset();

    FMemory::Memset(ActivationGroupCounts, 0, sizeof(ActivationGroupCounts));
}

void UAfterAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void UAfterAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
    FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
    check(ActorInfo);
    check(InOwnerActor);

    const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);

    Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

    if (bHasNewPawnAvatar)
    {
        for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
        {
            UAfterGameplayAbility* AfterAbilityCDO = CastChecked<UAfterGameplayAbility>(AbilitySpec.Ability);

            if (AfterAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
            {
                TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
                for (UGameplayAbility* AbilityInstance : Instances)
                {
                    UAfterGameplayAbility* AfterAbilityInstance = Cast<UAfterGameplayAbility>(AbilityInstance);
                    if (AfterAbilityInstance)
                    {
                        AfterAbilityInstance->OnPawnAvatarSet();
                    }
                }
            }
            else
            {
                AfterAbilityCDO->OnPawnAvatarSet();
            }
        }

        if (UAfterAnimInstance* AfterAnimInst = Cast<UAfterAnimInstance>(ActorInfo->GetAnimInstance()))
        {
            AfterAnimInst->InitializeWithAbilitySystem(this);
        }

        TryActivateAbilitiesOnSpawn();
    }
}

void UAfterAbilitySystemComponent::CancelAbilitiesByFunc(TShouldCancelAbilityFunc ShouldCancelFunc, bool bReplicateCancelAbility)
{
    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
    {
        if (!AbilitySpec.IsActive())
        {
            continue;
        }

        UAfterGameplayAbility* AfterAbilityCDO = CastChecked<UAfterGameplayAbility>(AbilitySpec.Ability);

        if (AfterAbilityCDO->GetInstancingPolicy() != EGameplayAbilityInstancingPolicy::NonInstanced)
        {
            TArray<UGameplayAbility*> Instances = AbilitySpec.GetAbilityInstances();
            for (UGameplayAbility* AbilityInstance : Instances)
            {
                UAfterGameplayAbility* AfterAbilityInstance = CastChecked<UAfterGameplayAbility>(AbilityInstance);

                if (ShouldCancelFunc(AfterAbilityInstance, AbilitySpec.Handle))
                {
                    if (AfterAbilityInstance->CanBeCanceled())
                    {
                        AfterAbilityInstance->CancelAbility(AbilitySpec.Handle, AbilityActorInfo.Get(),
                            AfterAbilityInstance->GetCurrentActivationInfo(), bReplicateCancelAbility);
                    }
                    else
                    {
                        UE_LOG(LogAfterAbilitySystem, Error,
                            TEXT("CancelAbilitiesByFunc: Can't cancel ability [%s] because CanBeCanceled is false."),
                            *AfterAbilityInstance->GetName());
                    }
                }
            }
        }
        else
        {
            if (ShouldCancelFunc(AfterAbilityCDO, AbilitySpec.Handle))
            {
                check(AfterAbilityCDO->CanBeCanceled());
                AfterAbilityCDO->CancelAbility(
                    AbilitySpec.Handle, AbilityActorInfo.Get(), FGameplayAbilityActivationInfo(), bReplicateCancelAbility);
            }
        }
    }
}

void UAfterAbilitySystemComponent::CancelInputActivatedAbilities(bool bReplicateCancelAbility) {}

void UAfterAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
    if (InputTag.IsValid())
    {
        for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
        {
            if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
            {
                InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
                InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
            }
        }
    }
}

void UAfterAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
    if (InputTag.IsValid())
    {
        for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
        {
            if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
            {
                InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
                InputHeldSpecHandles.Remove(AbilitySpec.Handle);
            }
        }
    }
}

void UAfterAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
    if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
    {
        ClearAbilityInput();
        return;
    }

    static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
    AbilitiesToActivate.Reset();

    for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
    {
        if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability && !AbilitySpec->IsActive())
            {
                const UAfterGameplayAbility* AfterAbilityCDO = CastChecked<UAfterGameplayAbility>(AbilitySpec->Ability);

                if (AfterAbilityCDO->GetActivationPolicy() == EAfterAbilityActivationPolicy::WhileInputActive)
                {
                    AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
                }
            }
        }
    }

    for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
    {
        if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability)
            {
                AbilitySpec->InputPressed = true;

                if (AbilitySpec->IsActive())
                {
                    AbilitySpecInputPressed(*AbilitySpec);
                }
                else
                {
                    const UAfterGameplayAbility* AfterAbilityCDO = CastChecked<UAfterGameplayAbility>(AbilitySpec->Ability);

                    if (AfterAbilityCDO->GetActivationPolicy() == EAfterAbilityActivationPolicy::OnInputTriggered)
                    {
                        AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
                    }
                }
            }
        }
    }

    for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
    {
        TryActivateAbility(AbilitySpecHandle);
    }

    for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
    {
        if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
        {
            if (AbilitySpec->Ability)
            {
                AbilitySpec->InputPressed = false;

                if (AbilitySpec->IsActive())
                {
                    AbilitySpecInputReleased(*AbilitySpec);
                }
            }
        }
    }

    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
}

void UAfterAbilitySystemComponent::ClearAbilityInput()
{
    InputPressedSpecHandles.Reset();
    InputReleasedSpecHandles.Reset();
    InputHeldSpecHandles.Reset();
}

bool UAfterAbilitySystemComponent::IsActivationGroupBlocked(EAfterAbilityActivationGroup Group) const
{
    bool bBlocked = false;

    switch (Group)
    {
        case EAfterAbilityActivationGroup::Independent:
            bBlocked = false;
            break;

        case EAfterAbilityActivationGroup::Exclusive_Replaceable:
        case EAfterAbilityActivationGroup::Exclusive_Blocking:
            bBlocked = (ActivationGroupCounts[(uint8)EAfterAbilityActivationGroup::Exclusive_Blocking] > 0);
            break;

        default:
            checkf(false, TEXT("IsActivationGroupBlocked: Invalid ActivationGroup [%d]\n"), (uint8)Group);
            break;
    }

    return bBlocked;
}

void UAfterAbilitySystemComponent::AddAbilityToActivationGroup(EAfterAbilityActivationGroup Group, UAfterGameplayAbility* AfterAbility)
{
    check(AfterAbility);
    check(ActivationGroupCounts[(uint8)Group] < INT32_MAX);

    ActivationGroupCounts[(uint8)Group]++;

    const bool bReplicateCancelAbility = false;

    switch (Group)
    {
        case EAfterAbilityActivationGroup::Independent:
            break;

        case EAfterAbilityActivationGroup::Exclusive_Replaceable:
        case EAfterAbilityActivationGroup::Exclusive_Blocking:
            CancelActivationGroupAbilities(EAfterAbilityActivationGroup::Exclusive_Replaceable, AfterAbility, bReplicateCancelAbility);
            break;

        default:
            checkf(false, TEXT("AddAbilityToActivationGroup: Invalid ActivationGroup [%d]\n"), (uint8)Group);
            break;
    }

    const int32 ExclusiveCount = ActivationGroupCounts[(uint8)EAfterAbilityActivationGroup::Exclusive_Replaceable] +
                                 ActivationGroupCounts[(uint8)EAfterAbilityActivationGroup::Exclusive_Blocking];
    if (!ensure(ExclusiveCount <= 1))
    {
        UE_LOG(LogAfterAbilitySystem, Error, TEXT("AddAbilityToActivationGroup: Multiple exclusive abilities are running."));
    }
}

void UAfterAbilitySystemComponent::RemoveAbilityFromActivationGroup(EAfterAbilityActivationGroup Group, UAfterGameplayAbility* AfterAbility)
{
    check(AfterAbility);
    check(ActivationGroupCounts[(uint8)Group] > 0);

    ActivationGroupCounts[(uint8)Group]--;
}

void UAfterAbilitySystemComponent::CancelActivationGroupAbilities(
    EAfterAbilityActivationGroup Group, UAfterGameplayAbility* IgnoreAfterAbility, bool bReplicateCancelAbility)
{
    auto ShouldCancelFunc = [this, Group, IgnoreAfterAbility](const UAfterGameplayAbility* LyraAbility, FGameplayAbilitySpecHandle Handle)
    { return ((LyraAbility->GetActivationGroup() == Group) && (LyraAbility != IgnoreAfterAbility)); };

    CancelAbilitiesByFunc(ShouldCancelFunc, bReplicateCancelAbility);
}

void UAfterAbilitySystemComponent::AddDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
    const TSubclassOf<UGameplayEffect> DynamicTagGE = UAfterAssetManager::GetSubclass(UAfterGameData::Get().DynamicTagGameplayEffect);
    if (!DynamicTagGE)
    {
        UE_LOG(LogAfterAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to find DynamicTagGameplayEffect [%s]."),
            *UAfterGameData::Get().DynamicTagGameplayEffect.GetAssetName());
        return;
    }

    const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingSpec(DynamicTagGE, 1.0f, MakeEffectContext());
    FGameplayEffectSpec* Spec = SpecHandle.Data.Get();

    if (!Spec)
    {
        UE_LOG(LogAfterAbilitySystem, Warning, TEXT("AddDynamicTagGameplayEffect: Unable to make outgoing spec for [%s]."),
            *GetNameSafe(DynamicTagGE));
        return;
    }

    Spec->DynamicGrantedTags.AddTag(Tag);

    ApplyGameplayEffectSpecToSelf(*Spec);
}

void UAfterAbilitySystemComponent::RemoveDynamicTagGameplayEffect(const FGameplayTag& Tag)
{
    const TSubclassOf<UGameplayEffect> DynamicTagGE = UAfterAssetManager::GetSubclass(UAfterGameData::Get().DynamicTagGameplayEffect);
    if (!DynamicTagGE)
    {
        UE_LOG(LogAfterAbilitySystem, Warning, TEXT("RemoveDynamicTagGameplayEffect: Unable to find gameplay effect [%s]."),
            *UAfterGameData::Get().DynamicTagGameplayEffect.GetAssetName());
        return;
    }

    FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));
    Query.EffectDefinition = DynamicTagGE;

    RemoveActiveEffects(Query);
}

void UAfterAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle,
    FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
    TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData =
        AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
    if (ReplicatedData.IsValid())
    {
        OutTargetDataHandle = ReplicatedData->TargetData;
    }
}

void UAfterAbilitySystemComponent::SetTagRelationshipMapping(UAfterAbilityTagRelationshipMapping* NewMapping)
{
    TagRelationshipMapping = NewMapping;
}

void UAfterAbilitySystemComponent::GetAdditionalActivationTagRequirements(const FGameplayTagContainer& AbilityTags,
    FGameplayTagContainer& OutActivationRequired, FGameplayTagContainer& OutActivationBlocked) const
{
    if (TagRelationshipMapping)
    {
        TagRelationshipMapping->GetRequiredAndBlockedActivationTags(AbilityTags, &OutActivationRequired, &OutActivationBlocked);
    }
}

void UAfterAbilitySystemComponent::TryActivateAbilitiesOnSpawn()
{
    ABILITYLIST_SCOPE_LOCK();
    for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
    {
        const UAfterGameplayAbility* AfterAbilityCDO = CastChecked<UAfterGameplayAbility>(AbilitySpec.Ability);
        AfterAbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
    }
}

void UAfterAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
    Super::AbilitySpecInputPressed(Spec);

    if (Spec.IsActive())
    {
        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
    }
}

void UAfterAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
    Super::AbilitySpecInputReleased(Spec);

    if (Spec.IsActive())
    {
        InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
    }
}

void UAfterAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability)
{
    Super::NotifyAbilityActivated(Handle, Ability);

    UAfterGameplayAbility* AfterAbility = CastChecked<UAfterGameplayAbility>(Ability);

    AddAbilityToActivationGroup(AfterAbility->GetActivationGroup(), AfterAbility);
}

void UAfterAbilitySystemComponent::NotifyAbilityFailed(
    const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
    Super::NotifyAbilityFailed(Handle, Ability, FailureReason);

    HandleAbilityFailed(Ability, FailureReason);
}

void UAfterAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
    Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

    UAfterGameplayAbility* AfterAbility = CastChecked<UAfterGameplayAbility>(Ability);

    RemoveAbilityFromActivationGroup(AfterAbility->GetActivationGroup(), AfterAbility);
}

void UAfterAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
    UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags,
    const FGameplayTagContainer& CancelTags)
{
    FGameplayTagContainer ModifiedBlockTags = BlockTags;
    FGameplayTagContainer ModifiedCancelTags = CancelTags;

    if (TagRelationshipMapping)
    {
        TagRelationshipMapping->GetAbilityTagsToBlockAndCancel(AbilityTags, &ModifiedBlockTags, &ModifiedCancelTags);
    }

    Super::ApplyAbilityBlockAndCancelTags(
        AbilityTags, RequestingAbility, bEnableBlockTags, ModifiedBlockTags, bExecuteCancelTags, ModifiedCancelTags);
}

void UAfterAbilitySystemComponent::HandleAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureReason)
{
    if (const UAfterGameplayAbility* AfterAbility = Cast<const UAfterGameplayAbility>(Ability))
    {
        AfterAbility->OnAbilityFailedToActivate(FailureReason);
    }
}
