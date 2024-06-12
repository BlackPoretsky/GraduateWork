#include "Weapons/AfterWeaponStateComponent.h"

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Equipment/AfterEquipmentManagerComponent.h"
#include "GameFramework/Pawn.h"
#include "GameplayEffectTypes.h"
#include "Kismet/GameplayStatics.h"
#include "NativeGameplayTags.h"
#include "Weapons/AfterRangedWeaponInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterWeaponStateComponent)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Gameplay_Zone, "Gamepaly.Zone")

UAfterWeaponStateComponent::UAfterWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.bCanEverTick = true;
}

void UAfterWeaponStateComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (APawn* Pawn = GetPawn<APawn>())
    {
        if (UAfterEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UAfterEquipmentManagerComponent>())
        {
            if (UAfterRangedWeaponInstance* CurrentWeapon =
                    Cast<UAfterRangedWeaponInstance>(EquipmentManager->GetFirstInstanceOfType(UAfterRangedWeaponInstance::StaticClass())))
            {
                CurrentWeapon->Tick(DeltaTime);
            }
        }
    }
}

void UAfterWeaponStateComponent::UpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext)
{
    if (ShouldUpdateDamageInstigatedTime(EffectContext))
    {
        ActuallyUpdateDamageInstigatedTime();
    }
}

double UAfterWeaponStateComponent::GetTimeSinceLastHitNotification() const
{
    UWorld* World = GetWorld();
    return World->TimeSince(LastWeaponDamageInstigatedTime);
}

bool UAfterWeaponStateComponent::ShouldUpdateDamageInstigatedTime(const FGameplayEffectContextHandle& EffectContext) const
{
    return EffectContext.GetEffectCauser() != nullptr;
}

void UAfterWeaponStateComponent::ActuallyUpdateDamageInstigatedTime()
{
    UWorld* World = GetWorld();
    if (World->GetTimeSeconds() - LastWeaponDamageInstigatedTime > 0.1)
    {
        LastWeaponDamageScreenLocations.Reset();
    }

    LastWeaponDamageInstigatedTime = World->GetTimeSeconds();
}
