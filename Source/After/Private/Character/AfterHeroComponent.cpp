#include "Character/AfterHeroComponent.h"

#include "Components/GameFrameworkComponentDelegates.h"
#include "Components/GameFrameworkComponentManager.h"

#include "Logging/MessageLog.h"
#include "AfterLogChannels.h"
#include "AfterGameplayTags.h"

#include "Player/AfterPlayerState.h"
#include "Player/AfterPlayerController.h"
#include "Engine/LocalPlayer.h"

#include "Character/AfterPawnExtensionComponent.h"
#include "Character/AfterPawnData.h"
#include "Character/AfterCharacter.h"

#include "AbilitySystem/AfterAbilitySystemComponent.h"
#include "Input/AfterInputConfig.h"
#include "Input/AfterEnhancedInputComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "PlayerMappableInputConfig.h"

#include "Camera/AfterCameraComponent.h"
#include "Camera/AfterCameraMode.h"

#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterHeroComponent)

namespace AfterHero
{
    static const float LookYawRate = 300.0f;
    static const float LookPitchRate = 165.0f;
};  // namespace AfterHero

const FName UAfterHeroComponent::NAME_BindInputsNow("BindInputsNow");
const FName UAfterHeroComponent::NAME_ActorFeatureName("Hero");

UAfterHeroComponent::UAfterHeroComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    AbilityCameraMode = nullptr;
    bReadyToBindInputs = false;
}

void UAfterHeroComponent::OnRegister()
{
    Super::OnRegister();

    if (!GetPawn<APawn>())
    {
        UE_LOG(LogAfter, Error,
            TEXT("[UAfterHeroComponent::OnRegister] This component has been added to a blueprint whose base class is not a Pawn. To use "
                 "this component, it MUST be placed on a Pawn Blueprint."));
    }
    else
    {
        RegisterInitStateFeature();
    }
}

bool UAfterHeroComponent::CanChangeInitState(
    UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
    check(Manager);

    APawn* Pawn = GetPawn<APawn>();

    if (!CurrentState.IsValid() && DesiredState == AfterGameplayTags::InitState_Spawned)
    {
        if (Pawn)
        {
            return true;
        }
    }
    else if (CurrentState == AfterGameplayTags::InitState_Spawned && DesiredState == AfterGameplayTags::InitState_DataAvailable)
    {
        if (!GetPlayerState<AAfterPlayerState>())
        {
            return false;
        }

        return true;
    }
    else if (CurrentState == AfterGameplayTags::InitState_DataAvailable && DesiredState == AfterGameplayTags::InitState_DataInitialized)
    {
        AAfterPlayerState* AfterPS = GetPlayerState<AAfterPlayerState>();

        return AfterPS && Manager->HasFeatureReachedInitState(
                              Pawn, UAfterPawnExtensionComponent::NAME_ActorFeatureName, AfterGameplayTags::InitState_DataInitialized);
    }
    else if (CurrentState == AfterGameplayTags::InitState_DataInitialized && DesiredState == AfterGameplayTags::InitState_GameplayReady)
    {
        return true;
    }

    return false;
}

void UAfterHeroComponent::HandleChangeInitState(
    UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
    if (CurrentState == AfterGameplayTags::InitState_DataAvailable && DesiredState == AfterGameplayTags::InitState_DataInitialized)
    {
        APawn* Pawn = GetPawn<APawn>();
        AAfterPlayerState* AfterPS = GetPlayerState<AAfterPlayerState>();
        if (!ensure(Pawn && AfterPS))
        {
            return;
        }

        const UAfterPawnData* PawnData = nullptr;

        if (UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
        {
            PawnData = PawnExtComp->GetPawnData<UAfterPawnData>();

            PawnExtComp->InitializeAbilitySystem(AfterPS->GetAfterAbilitySystemComponent(), AfterPS);
        }

        if (AAfterPlayerController* AfterPC = GetController<AAfterPlayerController>())
        {
            if (Pawn->InputComponent != nullptr)
            {
                InitializePlayerInput(Pawn->InputComponent);
            }
        }

        if (PawnData)
        {
            if (UAfterCameraComponent* CameraComponent = UAfterCameraComponent::FindCameraComponent(Pawn))
            {
                CameraComponent->DetermineCameraModeDelegate.BindUObject(this, &ThisClass::DetermineCameraMode);
            }
        }
    }
}

void UAfterHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
    if (Params.FeatureName == UAfterPawnExtensionComponent::NAME_ActorFeatureName)
    {
        if (Params.FeatureState == AfterGameplayTags::InitState_DataInitialized)
        {
            CheckDefaultInitialization();
        }
    }
}

void UAfterHeroComponent::CheckDefaultInitialization()
{
    static const TArray<FGameplayTag> StateChain = {AfterGameplayTags::InitState_Spawned, AfterGameplayTags::InitState_DataAvailable,
        AfterGameplayTags::InitState_DataInitialized, AfterGameplayTags::InitState_GameplayReady};

    ContinueInitStateChain(StateChain);
}

void UAfterHeroComponent::BeginPlay()
{
    Super::BeginPlay();

    BindOnActorInitStateChanged(UAfterPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);

    ensure(TryToChangeInitState(AfterGameplayTags::InitState_Spawned));
    CheckDefaultInitialization();
}

void UAfterHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnregisterInitStateFeature();

    Super::EndPlay(EndPlayReason);
}

void UAfterHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    const APawn* Pawn = GetPawn<APawn>();
    if (!Pawn)
    {
        return;
    }

    const APlayerController* PC = GetController<APlayerController>();
    check(PC);

    const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
    check(LP);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(Subsystem);

    Subsystem->ClearAllMappings();

    if (const UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
    {
        if (const UAfterPawnData* PawnData = PawnExtComp->GetPawnData<UAfterPawnData>())
        {
            if (const UAfterInputConfig* InputConfig = PawnData->InputConfig)
            {
                for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
                {
                    if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
                    {
                        if (Mapping.bRegisterWithSettings)
                        {
                            if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
                            {
                                Settings->RegisterInputMappingContext(IMC);
                            }

                            FModifyContextOptions Options = {};
                            Options.bIgnoreAllPressedKeysUntilRelease = false;
                            Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
                        }
                    }
                }

                UAfterEnhancedInputComponent* AfterIC = Cast<UAfterEnhancedInputComponent>(PlayerInputComponent);
                if (ensureMsgf(AfterIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. "
                                             "Change the input component to UAfterEnhancedInputComponent or a subclass of it.")))
                {
                    AfterIC->AddInputMappings(InputConfig, Subsystem);

                    TArray<uint32> BindHandles;
                    AfterIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed,
                        &ThisClass::Input_AbilityInputTagReleased, BindHandles);

                    AfterIC->BindNativeAction(
                        InputConfig, AfterGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, false);
                    AfterIC->BindNativeAction(InputConfig, AfterGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this,
                        &ThisClass::Input_LookMouse, false);
                    AfterIC->BindNativeAction(InputConfig, AfterGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this,
                        &ThisClass::Input_LookStick, false);
                    AfterIC->BindNativeAction(
                        InputConfig, AfterGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, false);

                    if (AAfterCharacter* AfterCharacter = GetPawn<AAfterCharacter>())
                    {
                        AfterIC->BindNativeAction(InputConfig, AfterGameplayTags::InputTag_Select, ETriggerEvent::Started, AfterCharacter,
                            &AAfterCharacter::SelectButtonPressed, false);
                    }
                }
            }
        }
    }

    if (ensure(!bReadyToBindInputs))
    {
        bReadyToBindInputs = true;
    }

    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
    UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UAfterHeroComponent::AddAdditionalInputConfig(const UAfterInputConfig* InputConfig)
{
    TArray<uint32> BindHandles;

    const APawn* Pawn = GetPawn<APawn>();
    if (!Pawn)
    {
        return;
    }

    const APlayerController* PC = GetController<APlayerController>();
    check(PC);

    const ULocalPlayer* LP = PC->GetLocalPlayer();
    check(LP);

    UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
    check(Subsystem);

    if (const UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
    {
        UAfterEnhancedInputComponent* AfterIC = Pawn->FindComponentByClass<UAfterEnhancedInputComponent>();
        if (ensureMsgf(AfterIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change "
                                     "the input component to ULyraInputComponent or a subclass of it.")))
        {
            AfterIC->BindAbilityActions(
                InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, BindHandles);
        }
    }
}

void UAfterHeroComponent::RemoveAdditionalInputConfig(const UAfterInputConfig* InputConfig) {}

bool UAfterHeroComponent::IsReadyToBindInputs() const
{
    return bReadyToBindInputs;
}

void UAfterHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    if (const APawn* Pawn = GetPawn<APawn>())
    {
        if (const UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
        {
            if (UAfterAbilitySystemComponent* AfterASC = PawnExtComp->GetAfterAbilitySystemComponent())
            {
                AfterASC->AbilityInputTagPressed(InputTag);
            }
        }
    }
}

void UAfterHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
    if (const APawn* Pawn = GetPawn<APawn>())
    {
        if (const UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
        {
            if (UAfterAbilitySystemComponent* AfterASC = PawnExtComp->GetAfterAbilitySystemComponent())
            {
                AfterASC->AbilityInputTagReleased(InputTag);
            }
        }
    }
}

void UAfterHeroComponent::Input_Move(const FInputActionValue& InputActionValue)
{
    APawn* Pawn = GetPawn<APawn>();
    AController* Controller = Pawn ? Pawn->GetController() : nullptr;

    if (!Controller) return;

    const FVector2D Value = InputActionValue.Get<FVector2D>();
    const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

    if (Value.X != 0.0f)
    {
        const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
        Pawn->AddMovementInput(MovementDirection, Value.X);
    }

    if (Value.Y != 0.0f)
    {
        const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
        Pawn->AddMovementInput(MovementDirection, Value.Y);
    }
}

void UAfterHeroComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
    APawn* Pawn = GetPawn<APawn>();

    if (!Pawn) return;

    const FVector2D Value = InputActionValue.Get<FVector2D>();

    if (Value.X != 0.0f)
    {
        Pawn->AddControllerYawInput(Value.X);
    }

    if (Value.Y != 0.0f)
    {
        Pawn->AddControllerPitchInput(Value.Y);
    }
}

void UAfterHeroComponent::Input_LookStick(const FInputActionValue& InputActionValue)
{
    APawn* Pawn = GetPawn<APawn>();

    if (!Pawn) return;

    const FVector2D Value = InputActionValue.Get<FVector2D>();

    const UWorld* World = GetWorld();
    check(World);

    if (Value.X != 0.0f)
    {
        Pawn->AddControllerYawInput(Value.X * AfterHero::LookYawRate * World->GetDeltaSeconds());
    }

    if (Value.Y != 0.0f)
    {
        Pawn->AddControllerPitchInput(Value.Y * AfterHero::LookPitchRate * World->GetDeltaSeconds());
    }
}

void UAfterHeroComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
    if (AAfterCharacter* Character = GetPawn<AAfterCharacter>())
    {
        Character->ToggleCrouch();
    }
}

TSubclassOf<UAfterCameraMode> UAfterHeroComponent::DetermineCameraMode() const
{
    if (AbilityCameraMode)
    {
        return AbilityCameraMode;
    }

    const APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return nullptr;

    if (UAfterPawnExtensionComponent* PawnExtComp = UAfterPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
    {
        if (const UAfterPawnData* PawnData = PawnExtComp->GetPawnData<UAfterPawnData>())
        {
            return PawnData->DefaultCameraMode;
        }
    }

    return nullptr;
}

void UAfterHeroComponent::SetAbilityCameraMode(TSubclassOf<UAfterCameraMode> CameraMode, const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
    if (CameraMode)
    {
        AbilityCameraMode = CameraMode;
        AbilityCameraModeOwningSpecHandle = OwningSpecHandle;
    }
}

void UAfterHeroComponent::ClearAbilityCameraMode(const FGameplayAbilitySpecHandle& OwningSpecHandle)
{
    if (AbilityCameraModeOwningSpecHandle == OwningSpecHandle)
    {
        AbilityCameraMode = nullptr;
        AbilityCameraModeOwningSpecHandle = FGameplayAbilitySpecHandle();
    }
}
