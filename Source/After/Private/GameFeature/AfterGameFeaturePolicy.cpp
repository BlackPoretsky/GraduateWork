#include "GameFeature/AfterGameFeaturePolicy.h"

#include "AbilitySystem/AfterGameplayCueManager.h"
#include "GameFeatureData.h"
#include "GameplayCueSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameFeaturePolicy)

/////////////////////////////
// UAfterGameFeaturePolicy //
/////////////////////////////

UAfterGameFeaturePolicy& UAfterGameFeaturePolicy::Get()
{
    return UGameFeaturesSubsystem::Get().GetPolicy<UAfterGameFeaturePolicy>();
}

UAfterGameFeaturePolicy::UAfterGameFeaturePolicy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterGameFeaturePolicy::InitGameFeatureManager()
{
    Observers.Add(NewObject<UAfterGameFeature_HotfixManager>());
    Observers.Add(NewObject<UAfterGameFeature_AddGameplayCuePaths>());

    UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
    for (UObject* Observer : Observers)
    {
        Subsystem.AddObserver(Observer);
    }

    Super::InitGameFeatureManager();
}

void UAfterGameFeaturePolicy::ShutdownGameFeatureManager()
{
    Super::ShutdownGameFeatureManager();

    UGameFeaturesSubsystem& Subsystem = UGameFeaturesSubsystem::Get();
    for (UObject* Observer : Observers)
    {
        Subsystem.RemoveObserver(Observer);
    }
    Observers.Empty();
}

TArray<FPrimaryAssetId> UAfterGameFeaturePolicy::GetPreloadAssetListForGameFeature(
    const UGameFeatureData* GameFeatureToLoad, bool bIncludeLoadedAssets) const
{
    return Super::GetPreloadAssetListForGameFeature(GameFeatureToLoad, bIncludeLoadedAssets);
}

bool UAfterGameFeaturePolicy::IsPluginAllowed(const FString& PluginURL) const
{
    return Super::IsPluginAllowed(PluginURL);
}

const TArray<FName> UAfterGameFeaturePolicy::GetPreloadBundleStateForGameFeature() const
{
    return Super::GetPreloadBundleStateForGameFeature();
}

void UAfterGameFeaturePolicy::GetGameFeatureLoadingMode(bool& bLoadClientData, bool& bLoadServerData) const
{
    bLoadClientData = !IsRunningDedicatedServer();
    bLoadServerData = !IsRunningClientOnly();
}

/////////////////////////////////////
// UAfterGameFeature_HotfixManager //
/////////////////////////////////////

void UAfterGameFeature_HotfixManager::OnGameFeatureLoading(const UGameFeatureData* GameFeatureData, const FString& PluginURL) {}

///////////////////////////////////////////
// UAfterGameFeature_AddGameplayCuePaths //
///////////////////////////////////////////

#include "AbilitySystemGlobals.h"
#include "GameFeature/GFA_AddGameplayCuePath.h"

void UAfterGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering(
    const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(UAfterGameFeature_AddGameplayCuePaths::OnGameFeatureRegistering);

    const FString PluginRootPath = TEXT("/") + PluginName;
    for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
    {
        if (const UGFA_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGFA_AddGameplayCuePath>(Action))
        {
            const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->GetDirectoryPathsToAdd();

            if (UAfterGameplayCueManager* GCM = UAfterGameplayCueManager::Get())
            {
                UGameplayCueSet* RuntimeGameplayCueSet = GCM->GetRuntimeCueSet();
                const int32 PreInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;

                for (const FDirectoryPath& Directory : DirsToAdd)
                {
                    FString MutablePath = Directory.Path;
                    UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
                    GCM->AddGameplayCueNotifyPath(MutablePath, /** bShouldRescanCueAssets = */ false);
                }

                if (!DirsToAdd.IsEmpty())
                {
                    GCM->InitializeRuntimeObjectLibrary();
                }

                const int32 PostInitializeNumCues = RuntimeGameplayCueSet ? RuntimeGameplayCueSet->GameplayCueData.Num() : 0;
                if (PreInitializeNumCues != PostInitializeNumCues)
                {
                    GCM->RefreshGameplayCuePrimaryAsset();
                }
            }
        }
    }
}

void UAfterGameFeature_AddGameplayCuePaths::OnGameFeatureUnregistering(
    const UGameFeatureData* GameFeatureData, const FString& PluginName, const FString& PluginURL)
{
    const FString PluginRootPath = TEXT("/") + PluginName;
    for (const UGameFeatureAction* Action : GameFeatureData->GetActions())
    {
        if (const UGFA_AddGameplayCuePath* AddGameplayCueGFA = Cast<UGFA_AddGameplayCuePath>(GameFeatureData))
        {
            const TArray<FDirectoryPath>& DirsToAdd = AddGameplayCueGFA->GetDirectoryPathsToAdd();

            if (UGameplayCueManager* GCM = UAbilitySystemGlobals::Get().GetGameplayCueManager())
            {
                int32 NumRemoved = 0;
                for (const FDirectoryPath& Directory : DirsToAdd)
                {
                    FString MutablePath = Directory.Path;
                    UGameFeaturesSubsystem::FixPluginPackagePath(MutablePath, PluginRootPath, false);
                    NumRemoved += GCM->RemoveGameplayCueNotifyPath(MutablePath, false);
                }

                ensure(NumRemoved == DirsToAdd.Num());

                if (NumRemoved > 0)
                {
                    GCM->InitializeRuntimeObjectLibrary();
                }
            }
        }
    }
}
