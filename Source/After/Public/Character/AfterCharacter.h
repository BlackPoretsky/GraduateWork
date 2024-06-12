// Project After. Creator Egor Gorochkin.

#pragma once

#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ModularGameplayActors/ModularCharacter.h"

#include "AfterCharacter.generated.h"

class AActor;
class AController;
class AAfterPlayerController;
class AAfterPlayerState;
class AItem;
class UAbilitySystemComponent;
class UInputComponent;
class UAfterAbilitySystemComponent;
class UAfterCameraComponent;
class UAfterHealthComponent;
class UAfterPawnExtensionComponent;
class UAfterEquipmentManagerComponent;
class UAfterPickupDefinition;

struct FGameplayTag;
struct FGameplayTagContainer;

UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class AAfterCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
    GENERATED_BODY()

public:
    AAfterCharacter(const FObjectInitializer& ObjInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = "GiveWeapon"))
    bool GiveWeapon(TSubclassOf<UAfterInventoryItemDefinition> WeaponItemClass, APawn* ReceivingPawn);

    UFUNCTION(BlueprintCallable, Category = "After|Character")
    AAfterPlayerController* GetAfterPlayerController() const;

    UFUNCTION(BlueprintCallable, Category = "After|Characater")
    AAfterPlayerState* GetAfterPlayerState() const;

    // IAbilitySystemInterface interface
    UFUNCTION(BlueprintCallable, Category = "After|Character")
    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponent() const;
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~End of IAbilitySystemInterface interface

    // IGameplayTagAssetInterface interface
    virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
    virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
    virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
    //~End of IGameplayTagAssetInterface interface

    void ToggleCrouch();

    // AActor interface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Reset() override;
    //~End of AActor interface

    UFUNCTION(BlueprintCallable, Category = "After|Character")
    void SelectButtonPressed(const FInputActionValue& InputActionValue);

    void IncrementOverlappedItemCount(int8 Amount);

protected:
    virtual void OnAbilitySystemInitialized();
    virtual void OnAbilitySystemUninitialized();

    virtual void PossessedBy(AController* NewController) override;
    virtual void UnPossessed() override;

    virtual void OnRep_Controller() override;
    virtual void OnRep_PlayerState() override;

    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

    void InitializeGameplayTags();

    UFUNCTION()
    virtual void OnDeathStarted(AActor* OwningActor);

    UFUNCTION()
    virtual void OnDeathFinished(AActor* OwningActor);

    void DisableMovementAndCollision();
    void DestroyDueToDeath();
    void UninitAndDestroy();

    UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDeathFinished"))
    void K2_OnDeathFinished();

    virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
    void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

    virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
    virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

    virtual bool CanJumpInternal_Implementation() const;

private:
    FVector GetTracePointStart(FVector& ViewLocation, FRotator& ViewRotation) const;

    void TraceForItems();
    bool TraceResult(FHitResult& OutHitResult, FVector& OutHitLocation);

    //! TODO: implement in Blueprints.
    UFUNCTION()
    void OnGraundLanded(const FHitResult& Hit);

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Character", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterPawnExtensionComponent> PawnExtComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Character", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterHealthComponent> HealthComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Character", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterCameraComponent> CameraComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Inventory", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterEquipmentManagerComponent> EquipmentManagerComponent;

public:
    UPROPERTY(EditDefaultsOnly, Category = "After|Animation")
    UAnimMontage* DeathAnimMontage;

    UPROPERTY(EditDefaultsOnly, Category = "After|Movement")
    FVector2D LandedVelocityDamage = FVector2D(900.0f, 1200.0f);

    UPROPERTY(EditDefaultsOnly, Category = "After|Movement")
    FVector2D LandedDamage = FVector2D(10.0f, 100.0f);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Trace Item")
    AItem* TraceHitItemLastFrame;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "After|Trace Item")
    AItem* TraceHitItem;

private:
    bool bShouldTraceForItems = false;
    int8 OverlappedItemCount = 0;
};
