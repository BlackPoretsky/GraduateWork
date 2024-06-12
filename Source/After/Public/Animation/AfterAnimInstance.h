// Project After. Creator Egor Gorochkin.

#pragma once

#include "Animation/AnimInstance.h"
#include "GameplayEffectTypes.h"
#include "AfterAnimInstance.generated.h"

class AAfterCharacter;
class UAbilitySystemComponent;

UCLASS(Config = Game)
class UAfterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    UAfterAnimInstance(const FObjectInitializer& ObjectInitializer);

    virtual void InitializeWithAbilitySystem(UAbilitySystemComponent* ASC);

    // OLD implementattion
    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float Deltatime);
    // ~OLD implementattion

protected:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "GameplayTags")
    FGameplayTagBlueprintPropertyMap GameplayTagPropertyMap;

    UPROPERTY(BlueprintReadOnly, Category = "Character State Data")
    float GroundDistance = -1.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    AAfterCharacter* Owner;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    bool bIsInAir;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement)
    bool bIsAcceleratig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float MovementOffsetYaw;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement)
    float LastMovementOffsetYaw;
};
