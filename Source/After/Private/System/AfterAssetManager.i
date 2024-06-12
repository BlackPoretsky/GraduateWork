#line 1 ".\\AfterAssetManager.cpp"

















const FName FAfterBundles::Equipped("Equipped");



static FAutoConsoleCommand CVarDumpLoadedAssets(TEXT("After.DumpLoadedAssets"),
    TEXT("Shows all assets that were loaded via the asset manager and are currently in memory."),
    FConsoleCommandDelegate::CreateStatic(UAfterAssetManager::DumpLoadedAssets));













UAfterAssetManager::UAfterAssetManager()
{
    DefaultPawnData = nullptr;
}

UAfterAssetManager& UAfterAssetManager::Get()
{
    check(GEngine);

    if (UAfterAssetManager* Singleton = Cast<UAfterAssetManager>(GEngine->AssetManager))
    {
        return *Singleton;
    }

    UE_LOG(LogAfter, Fatal, TEXT("Invalid AssetManagerClassName in DefaultEngine.ini.  It must be set to AfterAssetManager!"));

    return *NewObject<UAfterAssetManager>();
}

void UAfterAssetManager::DumpLoadedAssets()
{
    UE_LOG(LogAfter, Log, TEXT("========== Start Dumping Loaded Assets =========="));

    for (const UObject* LoadedAsset : Get().LoadedAssets)
    {
        UE_LOG(LogAfter, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
    }

    UE_LOG(LogAfter, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
    UE_LOG(LogAfter, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

const UAfterGameData& UAfterAssetManager::GetGameData()
{
    return GetOrLoadTypedGameData<UAfterGameData>(AfterGameDataPath);
}

const UAfterPawnData* UAfterAssetManager::GetDefaultPawnData() const
{
    return GetAsset<UAfterPawnData>(DefaultPawnData);
}

UObject* UAfterAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
    if (AssetPath.IsValid())
    {
        TUniquePtr<FScopeLogTime> LogTimePtr;

        if (ShouldLogAssetLoads())
        {
            LogTimePtr = MakeUnique<FScopeLogTime>(
                *FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
        }

        if (UAssetManager::IsInitialized())
        {
            return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
        }

        return AssetPath.TryLoad();
    }

    return nullptr;
}

bool UAfterAssetManager::ShouldLogAssetLoads()
{
    static bool bLogAssetLoads = FParse::Param(FCommandLine::Get(), TEXT("LogAssetLoads"));
    return bLogAssetLoads;
}

void UAfterAssetManager::AddLoadedAsset(const UObject* Asset)
{
    if (ensureAlways(Asset))
    {
        FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
        LoadedAssets.Add(Asset);
    }
}

void UAfterAssetManager::StartInitialLoading()
{
    SCOPED_BOOT_TIMING("UAfterAssetManager::StartInitialLoading");

    Super::StartInitialLoading();

    StartupJobs.Add(FAfterAssetManagerStartupJob( "InitializeGameplayCueManager()", [this](const FAfterAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle) { InitializeGameplayCueManager(); }, 1.f));

    {
        StartupJobs.Add(FAfterAssetManagerStartupJob( "GetGameData()", [this](const FAfterAssetManagerStartupJob& StartupJob, TSharedPtr<FStreamableHandle>& LoadHandle) { GetGameData(); }, 25.f));
    }

    DoAllStartupJobs();
}

















#line 150 ".\\AfterAssetManager.cpp"

UPrimaryDataAsset* UAfterAssetManager::LoadGameDataOfClass(
    TSubclassOf<UPrimaryDataAsset> DataClass, const TSoftObjectPtr<UPrimaryDataAsset>& DataClassPath, FPrimaryAssetType PrimaryAssetType)
{
    UPrimaryDataAsset* Asset = nullptr;

    DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Loading GameData Object"), STAT_GameData, STATGROUP_LoadTime);
    if (!DataClassPath.IsNull())
    {






#line 166 ".\\AfterAssetManager.cpp"
        UE_LOG(LogAfter, Log, TEXT("Loading GameData: %s ..."), *DataClassPath.ToString());
        SCOPE_LOG_TIME_IN_SECONDS(TEXT("    ... GameData loaded!"), nullptr);

        if (GIsEditor)
        {
            Asset = DataClassPath.LoadSynchronous();
            LoadPrimaryAssetsWithType(PrimaryAssetType);
        }
        else
        {
            TSharedPtr<FStreamableHandle> Handle = LoadPrimaryAssetsWithType(PrimaryAssetType);
            if (Handle.IsValid())
            {
                Handle->WaitUntilComplete(0.0f, false);

                
                Asset = Cast<UPrimaryDataAsset>(Handle->GetLoadedAsset());
            }
        }
    }

    if (Asset)
    {
        GameDataMap.Add(DataClass, Asset);
    }
    else
    {
        UE_LOG(LogAfter, Fatal,
            TEXT("Failed to load GameData asset at %s. Type %s. This is not recoverable and likely means you do not have the correct data "
                 "to run %s."),
            *DataClassPath.ToString(), *PrimaryAssetType.ToString(), FApp::GetProjectName());
    }

    return Asset;
}

void UAfterAssetManager::DoAllStartupJobs()
{
    SCOPED_BOOT_TIMING("ULyraAssetManager::DoAllStartupJobs");
    const double AllStartupJobsStartTime = FPlatformTime::Seconds();

    if (IsRunningDedicatedServer())
    {
        for (const FAfterAssetManagerStartupJob& StartupJob : StartupJobs)
        {
            StartupJob.DoJob();
        }
    }
    else
    {
        if (StartupJobs.Num() > 0)
        {
            float TotalJobValue = 0.0f;
            for (const FAfterAssetManagerStartupJob& StartupJob : StartupJobs)
            {
                TotalJobValue += StartupJob.JobWeight;
            }

            float AccumulatedJobValue = 0.0f;
            for (FAfterAssetManagerStartupJob& StartupJob : StartupJobs)
            {
                const float JobValue = StartupJob.JobWeight;
                StartupJob.SubstepProgressDelegate.BindLambda(
                    [This = this, AccumulatedJobValue, JobValue, TotalJobValue](float NewProgress)
                    {
                        const float SubstepAdjustment = FMath::Clamp(NewProgress, 0.0f, 1.0f) * JobValue;
                        const float OverallPercentWithSubstep = (AccumulatedJobValue + SubstepAdjustment) / TotalJobValue;

                        This->UpdateInitialGameContentLoadPercent(OverallPercentWithSubstep);
                    });

                StartupJob.DoJob();

                StartupJob.SubstepProgressDelegate.Unbind();

                AccumulatedJobValue += JobValue;

                UpdateInitialGameContentLoadPercent(AccumulatedJobValue / TotalJobValue);
            }
        }
        else
        {
            UpdateInitialGameContentLoadPercent(1.0f);
        }
    }

    StartupJobs.Empty();

    UE_LOG(LogAfter, Display, TEXT("All startup jobs took %.2f seconds to complete"), FPlatformTime::Seconds() - AllStartupJobsStartTime);
}

void UAfterAssetManager::InitializeGameplayCueManager()
{
    SCOPED_BOOT_TIMING("UAfterAssetManager::InitializeGameplayCueManager");

    UAfterGameplayCueManager* GCM = UAfterGameplayCueManager::Get();
    check(GCM);
    GCM->LoadAlwaysLoadedCues();
}

void UAfterAssetManager::UpdateInitialGameContentLoadPercent(float GameContentPercent) {}
