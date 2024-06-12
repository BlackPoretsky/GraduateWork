#pragma once

#include "Engine/AssetManager.h"
#include "AfterAssetManagerStartupJob.h"
#include "AfterAssetManager.generated.h"

class UPrimaryDataAsset;
class UAfterGameData;
class UAfterPawnData;

struct FAfterBundles
{
    static const FName Equipped;
};

UCLASS(Config = Game)
class UAfterAssetManager : public UAssetManager
{
    GENERATED_BODY()

public:
    UAfterAssetManager();

    static UAfterAssetManager& Get();

    template <typename AssetType>
    static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

    template <typename AssetType>
    static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

    static void DumpLoadedAssets();

    const UAfterGameData& GetGameData();
    const UAfterPawnData* GetDefaultPawnData() const;

protected:
    template <typename GameDataClass>
    const GameDataClass& GetOrLoadTypedGameData(const TSoftObjectPtr<GameDataClass>& DataPath)
    {
        if (const TObjectPtr<UPrimaryDataAsset>* Result = GameDataMap.Find(GameDataClass::StaticClass()))
        {
            return *CastChecked<GameDataClass>(*Result);
        }

        return *CastChecked<const GameDataClass>(
            LoadGameDataOfClass(GameDataClass::StaticClass(), DataPath, GameDataClass::StaticClass()->GetFName()));
    }

    static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);
    static bool ShouldLogAssetLoads();

    void AddLoadedAsset(const UObject* Asset);

    //~UAssetManager interface
    virtual void StartInitialLoading() override;
#if WITH_EDITOR
    virtual void PreBeginPIE(bool bStartSimulate) override;
#endif
    //~End ofUAssetManager interface

    UPrimaryDataAsset* LoadGameDataOfClass(TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath,
        FPrimaryAssetType PrimaryAssetType);

private:
    void DoAllStartupJobs();

    void InitializeGameplayCueManager();

    void UpdateInitialGameContentLoadPercent(float GameContentPercent);

    TArray<FAfterAssetManagerStartupJob> StartupJobs;

protected:
    UPROPERTY(Config)
    TSoftObjectPtr<UAfterGameData> AfterGameDataPath;

    UPROPERTY(Transient)
    TMap<TObjectPtr<UClass>, TObjectPtr<UPrimaryDataAsset>> GameDataMap;

    UPROPERTY(Config)
    TSoftObjectPtr<UAfterPawnData> DefaultPawnData;

private:
    UPROPERTY()
    TSet<TObjectPtr<const UObject>> LoadedAssets;

    FCriticalSection LoadedAssetsCritical;
};

template <typename AssetType>
AssetType* UAfterAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
    AssetType* LoadedAsset = nullptr;

    const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

    if (AssetPath.IsValid())
    {
        LoadedAsset = AssetPointer.Get();
        if (!LoadedAsset)
        {
            LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
            ensureAlwaysMsgf(LoadedAsset, TEXT("Failed to load asset [%s]"), *AssetPointer.ToString());
        }

        if (LoadedAsset && bKeepInMemory)
        {
            Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
        }
    }

    return LoadedAsset;
}

template <typename AssetType>
TSubclassOf<AssetType> UAfterAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
    TSubclassOf<AssetType> LoadedSubclass;

    const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();

    if (AssetPath.IsValid())
    {
        LoadedSubclass = AssetPointer.Get();
        if (!LoadedSubclass)
        {
            LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
            ensureAlwaysMsgf(LoadedSubclass, TEXT("Failed to load asset class [%s]"), *AssetPointer.ToString());
        }

        if (LoadedSubclass && bKeepInMemory)
        {
            Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
        }
    }

    return LoadedSubclass;
}