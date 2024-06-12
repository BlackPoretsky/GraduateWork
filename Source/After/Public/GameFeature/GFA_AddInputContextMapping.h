#pragma once

#include "GameFeature/GFA_WorldActionBase.h"
#include "GFA_AddInputContextMapping.generated.h"

class AActor;
class UInputMappingContext;
class UPlayer;
class APlayerController;

struct FComponentRequestHandle;

USTRUCT()
struct FInputMappingContextAndPriority
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Input", meta = (AssetBundles = "Client,Server"))
    TSoftObjectPtr<UInputMappingContext> InputMapping;

    UPROPERTY(EditAnywhere, Category = "Input")
    int32 Priority = 0;

    UPROPERTY(EditAnywhere, Category = "Input")
    bool bRegisterWithSettings = true;
};

/**
 *
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Mapping"))
class UGFA_AddInputContextMapping : public UGFA_WorldActionBase
{
    GENERATED_BODY()

public:
    //~UGameFeatureAction interface
    virtual void OnGameFeatureRegistering() override;
    virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
    virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
    virtual void OnGameFeatureUnregistering() override;
    //~End of UGameFeatureAction interface

    //~UObject interface
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
    //~End of UObject interface

    UPROPERTY(EditAnywhere, Category = "Input")
    TArray<FInputMappingContextAndPriority> InputMappings;

private:
    struct FPerContextData
    {
        TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
        TArray<TWeakObjectPtr<APlayerController>> ControllersAddedTo;
    };

    TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

    FDelegateHandle RegisterInputContextMappingsForGameInstanceHandle;

    void RegisterInputMappingContexts();

    void RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

    void RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

    void UnregisterInputMappingContexts();

    void UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance);

    void UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer);

    //~UGameFeatureAction_WorldActionBase interface
    virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
    //~End of UGameFeatureAction_WorldActionBase interface

    void Reset(FPerContextData& ActiveData);
    void HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
    void AddInputMappingForPlayer(UPlayer* Player, FPerContextData& ActiveData);
    void RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData);
};
