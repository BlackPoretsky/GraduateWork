#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"
#include "GameFeature/GFA_AddInputContextMapping.h"
#include "GameplayAbilitySpecHandle.h"
#include "AfterHeroComponent.generated.h"

struct FLoadedMappableConfigPair;
struct FMappableConfigPair;

class UGameFrameworkComponentManager;
class UInputComponent;
class UAfterCameraMode;
class UAfterInputConfig;
class UObject;
struct FActorInitStateChangedParams;
struct FGameplayTag;
struct FInputActionValue;

UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class AFTER_API UAfterHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
    GENERATED_BODY()

public:
    UAfterHeroComponent(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintPure, Category = "After|Hero")
    static UAfterHeroComponent* FindHeroComponent(const AActor* Actor)
    {
        return (Actor ? Actor->FindComponentByClass<UAfterHeroComponent>() : nullptr);
    }

    void SetAbilityCameraMode(TSubclassOf<UAfterCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle);

    void ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle);

    void AddAdditionalInputConfig(const UAfterInputConfig* InputConfig);

    void RemoveAdditionalInputConfig(const UAfterInputConfig* InputConfig);

    bool IsReadyToBindInputs() const;

    static const FName NAME_BindInputsNow;

    static const FName NAME_ActorFeatureName;

    //~ Begin IGameFrameworkInitStateInterface interface
    virtual FName GetFeatureName() const override { return NAME_ActorFeatureName; }
    virtual bool CanChangeInitState(
        UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;
    virtual void HandleChangeInitState(
        UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
    virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;
    virtual void CheckDefaultInitialization() override;
    //~ End IGameFrameworkInitStateInterface interface

protected:
    virtual void OnRegister() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

    void Input_AbilityInputTagPressed(FGameplayTag InputTag);
    void Input_AbilityInputTagReleased(FGameplayTag InputTag);

    void Input_Move(const FInputActionValue& InputActionValue);
    void Input_LookMouse(const FInputActionValue& InputActionValue);
    void Input_LookStick(const FInputActionValue& InputActionValue);
    void Input_Crouch(const FInputActionValue& InputActionValue);

    TSubclassOf<UAfterCameraMode> DetermineCameraMode() const;

protected:
    UPROPERTY(EditAnywhere)
    TArray<FInputMappingContextAndPriority> DefaultInputMappings;

    UPROPERTY()
    TSubclassOf<UAfterCameraMode> AbilityCameraMode;

    FGameplayAbilitySpecHandle AbilityCameraModeOwningSpecHandle;

    bool bReadyToBindInputs;
};
