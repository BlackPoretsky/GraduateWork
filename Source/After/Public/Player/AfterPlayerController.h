#pragma once

#include "Camera/AfterCameraAssistInterface.h"
#include "CommonGame/CommonPlayerController.h"
#include "AfterPlayerController.generated.h"

class AAfterHUD;
class AAfterPlayerState;
class APawn;
class APlayerState;
class UAfterAbilitySystemComponent;
class UAfterInventoryManagerComponent;
class UAfterQuickBarComponent;
class UUserWidget;

UCLASS(meta = (ShortToolTip = "The  base player controller class used by this progect."))
class AFTER_API AAfterPlayerController : public ACommonPlayerController, public IAfterCameraAssistInterface
{
    GENERATED_BODY()

public:
    AAfterPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "After|PlayerController")
    AAfterPlayerState* GetAfterPlayerState() const;

    UFUNCTION(BlueprintCallable, Category = "After|PlayerController")
    UAfterAbilitySystemComponent* GetAfterAbilitySystemComponent() const;

    UFUNCTION(BlueprintCallable, Category = "After|PlayerController")
    AAfterHUD* GetAfterHUD() const;

    UFUNCTION(BlueprintCallable, Category = "After|PlayerController")
    UAfterInventoryManagerComponent* GetInventoryManagerComponent() const { return InventoryComponent; }

    //~AActor inteface
    virtual void PreInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of Actor inteface

    //~AController interface
    virtual void OnUnPossess() override;
    //~End of AController interface

    //~APlayerController interface
    virtual void ReceivedPlayer() override;
    virtual void UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents) override;
    virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
    //~End of APlayerController interface

    //~IAfterCameraAssistInterface interface
    virtual void OnCameraPenetratingTarget() override;
    //~End of IAfterCameraAssistInterface interface

protected:
    bool bHideViewTargetPawnNextFrame = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Interface")
    TSubclassOf<UUserWidget> HUDOverlayClass;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterInventoryManagerComponent> InventoryComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", Meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UAfterQuickBarComponent> QuickBarComponent;

    UPROPERTY()
    UUserWidget* HUDOverlay;
};
