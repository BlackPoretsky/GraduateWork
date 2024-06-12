#pragma once

#include "AbilitySystemInterface.h"
#include "ModularGameplayActors/ModularPlayerState.h"
#include "System/GameplayTagStack.h"
#include "GameFramework/PlayerState.h"
#include "AfterPlayerState.generated.h"

class AController;
class AAfterPlayerController;
class UAbilitySystemComponent;
class UAfterAbilitySystemComponent;
class UAfterPawnData;
class UAfterHealthSet;
class UAfterCombatSet;

struct FGameplayTag;

UENUM()
enum class EAfterPlayerConnectionType : uint8
{
    Player = 0,
    LiveSpectator,
    ReplaySpectator,
    InactivePlayer
};

/**
 * AAfterPlayerState
 *
 */
UCLASS(Config = Game)
class AFTER_API AAfterPlayerState : public AModularPlayerState
{
    GENERATED_BODY()

public:
    AAfterPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After|PlayerState")
    AAfterPlayerController* GetAfterPlayerController() const;

    UFUNCTION(BlueprintCallable, Category = "After|PlayerState")
    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponent() const { return AbilitySystemComponent; }
    UAbilitySystemComponent* GetAbilitySystemComponent() const;

    template <class T>
    const T* GetPawnData() const
    {
        return Cast<T>(PawnData);
    }

    void SetPawnData(const UAfterPawnData* InPawnData);

    //~AActor interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    //~End of AActor interface

    //~APlayerState interface
    virtual void Reset() override;
    virtual void ClientInitialize(AController* C) override;
    virtual void CopyProperties(APlayerState* PlayerState) override;
    //~End of APlayerState interface

    static const FName NAME_AfterAbilityReady;

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
    void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

    UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Teams)
    void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

    UFUNCTION(BlueprintCallable, Category = Teams)
    int32 GetStatTagStackCount(FGameplayTag Tag) const;

    UFUNCTION(BlueprintCallable, Category = Teams)
    bool HasStatTag(FGameplayTag Tag) const;

protected:
    UPROPERTY()
    TObjectPtr<const UAfterPawnData> PawnData;

private:
    UPROPERTY(VisibleAnywhere, Category = "After|PlayerState")
    TObjectPtr<UAfterAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY()
    TObjectPtr<const UAfterHealthSet> HealthSet;

    UPROPERTY()
    TObjectPtr<const UAfterCombatSet> CombatSet;

    UPROPERTY()
    FGameplayTagStackContainer StatTags;
};
