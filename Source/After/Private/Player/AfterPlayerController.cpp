#include "Player/AfterPlayerController.h"
#include "CommonGame/CommonLocalPlayer.h"
#include "Components/PrimitiveComponent.h"
#include "Player/AfterPlayerState.h"
#include "Camera/AfterPlayerCameraManager.h"
#include "Interface/AfterHUD.h"
#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "GameFramework/Pawn.h"
#include "Engine/GameInstance.h"
#include "AbilitySystemGlobals.h"
#include "Inventory/AfterInventoryManagerComponent.h"
#include "Equipment/AfterQuickBarComponent.h"
#include "Blueprint/UserWidget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterPlayerController)

AAfterPlayerController::AAfterPlayerController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PlayerCameraManagerClass = AAfterPlayerCameraManager::StaticClass();

    InventoryComponent = CreateDefaultSubobject<UAfterInventoryManagerComponent>("InventoryComponent");
    check(InventoryComponent);

    QuickBarComponent = CreateDefaultSubobject<UAfterQuickBarComponent>("QuickBarComponent");
    check(QuickBarComponent);
}

void AAfterPlayerController::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AAfterPlayerController::BeginPlay()
{
    Super::BeginPlay();
    SetActorHiddenInGame(false);

    if (HUDOverlayClass)
    {
        HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);
        if (HUDOverlay)
        {
            HUDOverlay->AddToViewport();
            HUDOverlay->SetVisibility(ESlateVisibility::Visible);
        }
    }
}

void AAfterPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

AAfterPlayerState* AAfterPlayerController::GetAfterPlayerState() const
{
    return CastChecked<AAfterPlayerState>(PlayerState, ECastCheckedType::NullAllowed);
}

UAfterAbilitySystemComponent* AAfterPlayerController::GetAfterAbilitySystemComponent() const
{
    const AAfterPlayerState* AfterPlayerState = GetAfterPlayerState();
    return AfterPlayerState ? AfterPlayerState->GetAfterAbilitySystemComponent() : nullptr;
}

AAfterHUD* AAfterPlayerController::GetAfterHUD() const
{
    return CastChecked<AAfterHUD>(GetHUD(), ECastCheckedType::NullAllowed);
}

void AAfterPlayerController::ReceivedPlayer()
{
    Super::ReceivedPlayer();
}

void AAfterPlayerController::OnUnPossess()
{
    if (APawn* PawnBeingUnpossessed = GetPawn())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
        {
            if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
            {
                ASC->SetAvatarActor(nullptr);
            }
        }
    }

    Super::OnUnPossess();
}

void AAfterPlayerController::UpdateHiddenComponents(const FVector& ViewLocation, TSet<FPrimitiveComponentId>& OutHiddenComponents)
{
    Super::UpdateHiddenComponents(ViewLocation, OutHiddenComponents);

    if (bHideViewTargetPawnNextFrame)
    {
        AActor* const ViewTargetPawn = PlayerCameraManager ? Cast<AActor>(PlayerCameraManager->GetViewTarget()) : nullptr;
        if (ViewTargetPawn)
        {

            auto AddToHiddenComponents = [&OutHiddenComponents](const TInlineComponentArray<UPrimitiveComponent*>& InComponents)
            {
                for (UPrimitiveComponent* Comp : InComponents)
                {
                    if (Comp->IsRegistered())
                    {
                        OutHiddenComponents.Add(Comp->ComponentId);

                        for (USceneComponent* AttachedChild : Comp->GetAttachChildren())
                        {
                            static FName NAME_NoParentAutoHide(TEXT("NoParentAutoHide"));
                            UPrimitiveComponent* AttachChildPC = Cast<UPrimitiveComponent>(AttachedChild);
                            if (AttachChildPC && AttachChildPC->IsRegistered() &&
                                !AttachChildPC->ComponentTags.Contains(NAME_NoParentAutoHide))
                            {
                                OutHiddenComponents.Add(AttachChildPC->ComponentId);
                            }
                        }
                    }
                }
            };

            TInlineComponentArray<UPrimitiveComponent*> PawnComponents;
            ViewTargetPawn->GetComponents(PawnComponents);
            AddToHiddenComponents(PawnComponents);
        }

        bHideViewTargetPawnNextFrame = false;
    }
}

void AAfterPlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
    Super::PreProcessInput(DeltaTime, bGamePaused);
}

void AAfterPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    if (UAfterAbilitySystemComponent* AfterASC = GetAfterAbilitySystemComponent())
    {
        AfterASC->ProcessAbilityInput(DeltaTime, bGamePaused);
    }

    Super::PostProcessInput(DeltaTime, bGamePaused);
}

void AAfterPlayerController::OnCameraPenetratingTarget()
{
    bHideViewTargetPawnNextFrame = true;
}
