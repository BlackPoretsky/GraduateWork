#pragma once

#include "GameFeatureStateChangeObserver.h"
#include "GameFeaturesProjectPolicies.h"

#include "AfterGameFeaturePolicy.generated.h"

class UGameFeatureData;
struct FPrimaryAssetId;

/**
 *
 */
UCLASS(MinimalAPI, Config = Game)
class UAfterGameFeaturePolicy : public UDefaultGameFeaturesProjectPolicies
{
    GENERATED_BODY()

public:
    AFTER_API static UAfterGameFeaturePolicy& Get();

    UAfterGameFeaturePolicy(const FObjectInitializer& ObjectInitializer);

    //~UGameFeaturesProjectPolicies interface
    virtual void InitGameFeatureManager() override;
    virtual void ShutdownGameFeatureManager() override;
    virtual TArray<FPrimaryAssetId> GetPreloadAssetListForGameFeature(
        const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets = false) const override;
    virtual bool IsPluginAllowed(const FString& PluginURL) const override;
    virtual const TArray<FName> GetPreloadBundleStateForGameFeature() const override;
    virtual void GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const override;
    //~End of UGameFeaturesProjectPolicies interface

private:
    UPROPERTY(Transient)
    TArray<TObjectPtr<UObject>> Observers;
};

UCLASS()
class UAfterGameFeature_HotfixManager : public UObject, public IGameFeatureStateChangeObserver
{
    GENERATED_BODY()

public:
    virtual void OnGameFeatureLoading(const UGameFeatureData* GameFeatureData, const FString& PluginURL) override;
};

UCLASS()
class UAfterGameFeature_AddGameplayCuePaths : public UObject, public IGameFeatureStateChangeObserver
{
    GENERATED_BODY()

public:
    virtual void OnGameFeatureRegistering(
        const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
    virtual void OnGameFeatureUnregistering(
        const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL) override;
};