#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "NativeGameplayTags.h"

#include "AfterCharacterMovementComponent.generated.h"

AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_MovementStopped);

USTRUCT(BlueprintType)
struct FAfterCharacterGroundInfo
{
    GENERATED_BODY();

    FAfterCharacterGroundInfo()
        : LastUpdateFrame(0),
          GroundDistance(0.0f)
    {
    }

    uint64 LastUpdateFrame;

    UPROPERTY(BlueprintReadOnly)
    FHitResult GroundHitResult;

    UPROPERTY(BlueprintReadOnly)
    float GroundDistance;
};

UCLASS()
class AFTER_API UAfterCharacterMovementComponent : public UCharacterMovementComponent
{
    GENERATED_BODY()

public:
    UAfterCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

    virtual bool CanAttemptJump() const override;

    UFUNCTION(BlueprintCallable, Category = "After|CharacterMovement")
    const FAfterCharacterGroundInfo& GetGroundInfo();

    //~UMovementComponent interface
    virtual FRotator GetDeltaRotation(float DeltaTime) const override;
    virtual float GetMaxSpeed() const override;
    //~End of UMovementComponent interface

protected:
    virtual void InitializeComponent() override;

protected:
    FAfterCharacterGroundInfo CachedGroundInfo;
};
