#pragma once

#include "Equipment/AfterGA_FromEquipment.h"

#include "AfterGA__RangedWeapon.generated.h"

class APawn;
class UAfterRangedWeaponInstance;
struct FCollisionQueryParams;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;
struct FGameplayTag;
struct FGameplayTagContainer;

UENUM(BlueprintType)
enum class EAfterAbilityTargetingSource : uint8
{
    CameraTowardsFocus,
    PawnForward,
    PawnTowardsFocus,
    WeaponForward,
    WeaponTowardsFocus,
    Custom
};

UCLASS()
class UAfterGA__RangedWeapon : public UAfterGA_FromEquipment
{
    GENERATED_BODY()

public:
    UAfterGA__RangedWeapon(const FObjectInitializer& ObjectIntializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After|Ability")
    UAfterRangedWeaponInstance* GetWeaponInstance() const;

    //~UGameplayAbility interface
    virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr,
        OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
    //~End of UGameplayAbility interface

protected:
    struct FRangedWeaponFiringInput
    {
        FVector StartTrace;

        FVector EndAim;

        FVector AimDir;

        UAfterRangedWeaponInstance* WeaponData = nullptr;

        bool bCanPlayBulletFX = false;

        FRangedWeaponFiringInput()
            : StartTrace(ForceInitToZero),
              EndAim(ForceInitToZero),
              AimDir(ForceInitToZero)
        {
        }
    };

protected:
    static int32 FindFirstPawnHitResult(const TArray<FHitResult>& HitResult);

    FHitResult WeaponTrace(const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated,
        OUT TArray<FHitResult>& OutHitResults) const;

    FHitResult DoSingleBulletTrace(
        const FVector& StartTrace, const FVector& EndTrace, float SweepRadius, bool bIsSimulated, OUT TArray<FHitResult>& OutHits) const;

    void TraceBulletsInCartridge(const FRangedWeaponFiringInput& InputData, OUT TArray<FHitResult>& OutHits);

    virtual void AddAdditionalTraceIgnoreActors(FCollisionQueryParams& TraceParams) const;

    virtual ECollisionChannel DetermineTraceChannel(FCollisionQueryParams& TraceParams, bool bIsSimulated) const;

    void PerformLocalTargeting(OUT TArray<FHitResult>& OutHits);

    FVector GetWeaponTargetingSourceLocation() const;
    FTransform GetTargetingTransform(APawn* SourcePawn, EAfterAbilityTargetingSource Source) const;

    void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

    UFUNCTION(BlueprintCallable)
    void StartRangedWeaponTargeting();

    UFUNCTION(BlueprintImplementableEvent)
    void OnRangedWeaponTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

private:
    FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
