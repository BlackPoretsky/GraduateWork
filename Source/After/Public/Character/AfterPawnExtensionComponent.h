#pragma once

#include "Components/GameFrameworkInitStateInterface.h"
#include "Components/PawnComponent.h"

#include "AfterPawnExtensionComponent.generated.h"

class UGameFrameworkComponentManager;
class UAfterAbilitySystemComponent;
class UAfterPawnData;
class UObject;

struct FActorInitStateChangedParams;
struct FGameplayTag;

UCLASS()
class AFTER_API UAfterPawnExtensionComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
    GENERATED_BODY()

public:
    UAfterPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);

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

    UFUNCTION(BlueprintPure, Category = "Afte|Pawn")
    static UAfterPawnExtensionComponent* FindPawnExtensionComponent(const AActor* Actor)
    {
        return (Actor ? Actor->FindComponentByClass<UAfterPawnExtensionComponent>() : nullptr);
    }

    template <class T>
    const T* GetPawnData() const
    {
        return Cast<T>(PawnData);
    }

    void SetPawnData(const UAfterPawnData* InPawnData);

    UFUNCTION(BlueprintPure, Category = "After|Pawn")
    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponent() const { return AbilitySystemComponent; }

    void InitializeAbilitySystem(UAfterAbilitySystemComponent* InASC, AActor* InOwnerActor);

    void UninitializeAbilitySystem();

    void HandleControllerChanged();

    void HandlePlayerStateReplicated();

    void SetupPlayerInputComponent();

    void OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate);
    void OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate);

protected:
    virtual void OnRegister() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    FSimpleMulticastDelegate OnAbilitySystemInitialized;
    FSimpleMulticastDelegate OnAbilitySystemUninitialized;

protected:
    UPROPERTY(EditAnywhere, Category = "After|Pawn")
    TObjectPtr<const UAfterPawnData> PawnData;

    UPROPERTY()
    TObjectPtr<UAfterAbilitySystemComponent> AbilitySystemComponent;
};
