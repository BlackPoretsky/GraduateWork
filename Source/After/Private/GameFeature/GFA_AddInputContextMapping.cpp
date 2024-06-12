#include "GameFeature/GFA_AddInputContextMapping.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "GameFeature/GFA_WorldActionBase.h"
#include "InputMappingContext.h"
#include "Character/AfterHeroComponent.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "System/AfterAssetManager.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFA_AddInputContextMapping)

#define LOCTEXT_NAMESPACE "GameFeature"

void UGFA_AddInputContextMapping::OnGameFeatureRegistering()
{
    Super::OnGameFeatureRegistering();

    RegisterInputMappingContexts();
}

void UGFA_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
    if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) || !ensure(ActiveData.ControllersAddedTo.IsEmpty()))
    {
        Reset(ActiveData);
    }

    Super::OnGameFeatureActivating(Context);
}

void UGFA_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    Super::OnGameFeatureDeactivating(Context);

    FPerContextData* ActiveData = ContextData.Find(Context);
    if (ensure(ActiveData))
    {
        Reset(*ActiveData);
    }
}

void UGFA_AddInputContextMapping::OnGameFeatureUnregistering()
{
    Super::OnGameFeatureUnregistering();

    UnregisterInputMappingContexts();
}

#if WITH_EDITOR
EDataValidationResult UGFA_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

    int32 Index = 0;

    for (const FInputMappingContextAndPriority& Entry : InputMappings)
    {
        if (Entry.InputMapping.IsNull())
        {
            Result = EDataValidationResult::Invalid;
            Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
        }
        ++Index;
    }

    return Result;
}
#endif

void UGFA_AddInputContextMapping::RegisterInputMappingContexts()
{
    RegisterInputContextMappingsForGameInstanceHandle =
        FWorldDelegates::OnStartGameInstance.AddUObject(this, &UGFA_AddInputContextMapping::RegisterInputContextMappingsForGameInstance);

    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator;
         ++WorldContextIterator)
    {
        RegisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
    }
}

void UGFA_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
    if (GameInstance != nullptr && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
    {
        GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGFA_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer);
        GameInstance->OnLocalPlayerRemovedEvent.AddUObject(
            this, &UGFA_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer);

        for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator(); LocalPlayerIterator;
             ++LocalPlayerIterator)
        {
            RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
        }
    }
}

void UGFA_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
    if (ensure(LocalPlayer))
    {
        UAfterAssetManager& AssetManager = UAfterAssetManager::Get();

        if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
            {
                for (const FInputMappingContextAndPriority& Entry : InputMappings)
                {
                    if (!Entry.bRegisterWithSettings)
                    {
                        continue;
                    }

                    if (UInputMappingContext* IMC = AssetManager.GetAsset(Entry.InputMapping))
                    {
                        Settings->RegisterInputMappingContext(IMC);
                    }
                }
            }
        }
    }
}

void UGFA_AddInputContextMapping::UnregisterInputMappingContexts()
{
    FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
    RegisterInputContextMappingsForGameInstanceHandle.Reset();

    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator;
         ++WorldContextIterator)
    {
        UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
    }
}

void UGFA_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
    if (GameInstance != nullptr)
    {
        GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
        GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

        for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator(); LocalPlayerIterator;
             ++LocalPlayerIterator)
        {
            UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
        }
    }
}

void UGFA_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
    if (ensure(LocalPlayer))
    {
        if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
        {
            if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
            {
                for (const FInputMappingContextAndPriority& Entry : InputMappings)
                {
                    if (!Entry.bRegisterWithSettings)
                    {
                        continue;
                    }

                    if (UInputMappingContext* IMC = Entry.InputMapping.Get())
                    {
                        Settings->UnregisterInputMappingContext(IMC);
                    }
                }
            }
        }
    }
}

void UGFA_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
    UWorld* World = WorldContext.World();
    UGameInstance* GameInstance = WorldContext.OwningGameInstance;
    FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

    if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
    {
        if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
        {
            UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
                UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
                    this, &ThisClass::HandleControllerExtension, ChangeContext);
            TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
                ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddAbilitiesDelegate);

            ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
        }
    }
}

void UGFA_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
    ActiveData.ExtensionRequestHandles.Empty();

    while (!ActiveData.ControllersAddedTo.IsEmpty())
    {
        TWeakObjectPtr<APlayerController> ControllerPtr = ActiveData.ControllersAddedTo.Top();
        if (ControllerPtr.IsValid())
        {
            RemoveInputMapping(ControllerPtr.Get(), ActiveData);
        }
        else
        {
            ActiveData.ControllersAddedTo.Pop();
        }
    }
}

void UGFA_AddInputContextMapping::HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
    APlayerController* AsController = CastChecked<APlayerController>(Actor);
    FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

    if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) ||
        (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
    {
        RemoveInputMapping(AsController, ActiveData);
    }
    else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UAfterHeroComponent::NAME_BindInputsNow))
    {
        AddInputMappingForPlayer(AsController->GetLocalPlayer(), ActiveData);
    }
}

void UGFA_AddInputContextMapping::AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData)
{
    if (ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player))
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            for (const FInputMappingContextAndPriority& Entry : InputMappings)
            {
                if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
                {
                    InputSystem->AddMappingContext(IMC, Entry.Priority);
                }
            }
        }
        else
        {
            UE_LOG(LogGameFeatures, Error,
                TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure "
                     "you're set to use the EnhancedInput system via config file."));
        }
    }
}

void UGFA_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData)
{
    if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            for (const FInputMappingContextAndPriority& Entry : InputMappings)
            {
                if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
                {
                    InputSystem->RemoveMappingContext(IMC);
                }
            }
        }
    }

    ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE