#include "AbilitySystem/AfterGameplayCueManager.h"
#include "Engine/AssetManager.h"
#include "AfterLogChannels.h"
#include "GameplayCueSet.h"
#include "AbilitySystem/AfterAbilitySystemGlobals.h"
#include "GameplayTagsManager.h"
#include "UObject/UObjectThreadContext.h"
#include "Async/Async.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameplayCueManager)

/////////////////////////////////////////////////////////////////

enum class EAfterEditorLoadMode
{
    LoadUpfront,
    PreloadAsCuesAreReferenced_GameOnly,
    PreloadAsCuesAreReferenced
};

namespace AfterGameplayCueManagerCvars
{
    static FAutoConsoleCommand CVarDumpGameplayCues(TEXT("After.DumpGameplayCues"),
        TEXT("Shows all assets that were loaded via AfterGameplayCueManager and are currently in memory."),
        FConsoleCommandWithArgsDelegate::CreateStatic(UAfterGameplayCueManager::DumGameplayCues));

    static EAfterEditorLoadMode LoadMode = EAfterEditorLoadMode::LoadUpfront;
}  // namespace AfterGameplayCueManagerCvars

/////////////////////////////////////////////////////////////////

struct FGameplayCueTagThreadSynchronizeGraphTask : public FAsyncGraphTaskBase
{
    TFunction<void()> TheTask;
    FGameplayCueTagThreadSynchronizeGraphTask(TFunction<void()>&& Task)
        : TheTask(MoveTemp(Task))
    {
    }
    void DoTask(ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) { TheTask(); }
    ENamedThreads::Type GetDesiredThread() { return ENamedThreads::GameThread; }
};

/////////////////////////////////////////////////////////////////

UAfterGameplayCueManager::UAfterGameplayCueManager(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

UAfterGameplayCueManager* UAfterGameplayCueManager::Get()
{
    return Cast<UAfterGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
}

void UAfterGameplayCueManager::OnCreated()
{
    Super::OnCreated();

    UpdateDelayLoadDelegateListeners();
}

bool UAfterGameplayCueManager::ShouldAsyncLoadRuntimeObjectLibraries() const
{
    switch (AfterGameplayCueManagerCvars::LoadMode)
    {
        case EAfterEditorLoadMode::LoadUpfront:
            return true;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
            if (GIsEditor)
            {
                return false;
            }
#endif
            break;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced:
            break;
    }

    return !ShouldDelayLoadGameplayCues();
}

bool UAfterGameplayCueManager::ShouldSyncLoadMissingGameplayCues() const
{
    return false;
}

bool UAfterGameplayCueManager::ShouldAsyncLoadMissingGameplayCues() const
{
    return true;
}

void UAfterGameplayCueManager::DumGameplayCues(const TArray<FString>& Args)
{
    UAfterGameplayCueManager* GCM = Cast<UAfterGameplayCueManager>(UAbilitySystemGlobals::Get().GetGameplayCueManager());
    if (!GCM)
    {
        UE_LOG(LogAfter, Error, TEXT("DumpGameplayCues failed. No UAfterGameplayCueManager found."));
        return;
    }

    const bool bIncludeRefs = Args.Contains(TEXT("Refs"));

    UE_LOG(LogAfter, Log, TEXT("=========== Dumping Always Loaded Gameplay Cue Notifies ==========="));
    for (UClass* CueClass : GCM->AlwaysLoadedCues)
    {
        UE_LOG(LogAfter, Log, TEXT("  %s"), *GetPathNameSafe(CueClass));
    }

    UE_LOG(LogAfter, Log, TEXT("=========== Dumping Preloaded Gameplay Cue Notifies ==========="));
    for (UClass* CueClass : GCM->PreloadedCues)
    {
        TSet<FObjectKey>* ReferencerSet = GCM->PreloadedCueReferencers.Find(CueClass);
        int32 NumRefs = ReferencerSet ? ReferencerSet->Num() : 0;
        UE_LOG(LogAfter, Log, TEXT("  %s (%d refs)"), *GetPathNameSafe(CueClass), NumRefs);
        if (bIncludeRefs && ReferencerSet)
        {
            for (const FObjectKey& Ref : *ReferencerSet)
            {
                UObject* RefObject = Ref.ResolveObjectPtr();
                UE_LOG(LogAfter, Log, TEXT("    ^- %s"), *GetPathNameSafe(RefObject));
            }
        }
    }

    UE_LOG(LogAfter, Log, TEXT("=========== Dumping Gameplay Cue Notifies loaded on demand ==========="));
    int32 NumMissingCuesLoaded = 0;
    if (GCM->RuntimeGameplayCueObjectLibrary.CueSet)
    {
        for (const FGameplayCueNotifyData& CueData : GCM->RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData)
        {
            if (CueData.LoadedGameplayCueClass && !GCM->AlwaysLoadedCues.Contains(CueData.LoadedGameplayCueClass) &&
                !GCM->PreloadedCues.Contains(CueData.LoadedGameplayCueClass))
            {
                NumMissingCuesLoaded++;
                UE_LOG(LogAfter, Log, TEXT("  %s"), *CueData.LoadedGameplayCueClass->GetPathName());
            }
        }
    }

    UE_LOG(LogAfter, Log, TEXT("=========== Gameplay Cue Notify summary ==========="));
    UE_LOG(LogAfter, Log, TEXT("  ... %d cues in always loaded list"), GCM->AlwaysLoadedCues.Num());
    UE_LOG(LogAfter, Log, TEXT("  ... %d cues in preloaded list"), GCM->PreloadedCues.Num());
    UE_LOG(LogAfter, Log, TEXT("  ... %d cues loaded on demand"), NumMissingCuesLoaded);
    UE_LOG(LogAfter, Log, TEXT("  ... %d cues in total"), GCM->AlwaysLoadedCues.Num() + GCM->PreloadedCues.Num() + NumMissingCuesLoaded);
}

void UAfterGameplayCueManager::LoadAlwaysLoadedCues()
{
    if (ShouldDelayLoadGameplayCues())
    {
        UGameplayTagsManager& TagManager = UGameplayTagsManager::Get();

        TArray<FName> AdditionalAlwaysLoadedCueTags;

        for (const FName& CueTagName : AdditionalAlwaysLoadedCueTags)
        {
            FGameplayTag CueTag = TagManager.RequestGameplayTag(CueTagName, false);
            if (CueTag.IsValid())
            {
                ProcessTagToPreload(CueTag, nullptr);
            }
            else
            {
                UE_LOG(LogAfter, Warning, TEXT("ULyraGameplayCueManager::AdditionalAlwaysLoadedCueTags contains invalid tag %s"),
                    *CueTagName.ToString());
            }
        }
    }
}

const FPrimaryAssetType UFortAssetManager_GameplayCueRefsType = TEXT("GameplayCueRefs");
const FName UFortAssetManager_GameplayCueRefsName = TEXT("GameplayCueReferences");
const FName UFortAssetManager_LoadStateClient = FName(TEXT("Client"));

void UAfterGameplayCueManager::RefreshGameplayCuePrimaryAsset()
{
    TArray<FSoftObjectPath> CuePaths;
    UGameplayCueSet* RuntimeGameplayCueSet = GetRuntimeCueSet();
    if (RuntimeGameplayCueSet)
    {
        RuntimeGameplayCueSet->GetSoftObjectPaths(CuePaths);
    }

    FAssetBundleData BundleData;
    BundleData.AddBundleAssetsTruncated(UFortAssetManager_LoadStateClient, CuePaths);

    FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(UFortAssetManager_GameplayCueRefsType, UFortAssetManager_GameplayCueRefsName);
    UAssetManager::Get().AddDynamicAsset(PrimaryAssetId, FSoftObjectPath(), BundleData);
}

void UAfterGameplayCueManager::OnGameplayTagLoaded(const FGameplayTag& Tag)
{
    FScopeLock ScopeLock(&LoadedGameplayTagsToProcessCS);
    bool bStartTask = LoadedGameplayTagsToProcess.Num() == 0;
    FUObjectSerializeContext* LoadContext = FUObjectThreadContext::Get().GetSerializeContext();
    UObject* OwningObject = LoadContext ? LoadContext->SerializedObject : nullptr;
    LoadedGameplayTagsToProcess.Emplace(Tag, OwningObject);
    if (bStartTask)
    {
        TGraphTask<FGameplayCueTagThreadSynchronizeGraphTask>::CreateTask().ConstructAndDispatchWhenReady(
            []()
            {
                if (GIsRunning)
                {
                    if (UAfterGameplayCueManager* StrongThis = Get())
                    {
                        // If we are garbage collecting we cannot call StaticFindObject (or a few other static uobject functions), so we'll
                        // just wait until the GC is over and process the tags then
                        if (IsGarbageCollecting())
                        {
                            StrongThis->bProcessLoadedTagsAfterGC = true;
                        }
                        else
                        {
                            StrongThis->ProcessLoadedTags();
                        }
                    }
                }
            });
    }
}

void UAfterGameplayCueManager::HandlePostGarbageCollect()
{
    if (bProcessLoadedTagsAfterGC)
    {
        ProcessLoadedTags();
    }
    bProcessLoadedTagsAfterGC = false;
}

void UAfterGameplayCueManager::ProcessLoadedTags()
{
    TArray<FLoadedGameplayTagToProcessData> TaskLoadedGameplayTagsToProcess;
    {
        // Lock LoadedGameplayTagsToProcess just long enough to make a copy and clear
        FScopeLock TaskScopeLock(&LoadedGameplayTagsToProcessCS);
        TaskLoadedGameplayTagsToProcess = LoadedGameplayTagsToProcess;
        LoadedGameplayTagsToProcess.Empty();
    }

    // This might return during shutdown, and we don't want to proceed if that is the case
    if (GIsRunning)
    {
        if (RuntimeGameplayCueObjectLibrary.CueSet)
        {
            for (const FLoadedGameplayTagToProcessData& LoadedTagData : TaskLoadedGameplayTagsToProcess)
            {
                if (RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Contains(LoadedTagData.Tag))
                {
                    if (!LoadedTagData.WeakOwner.IsStale())
                    {
                        ProcessTagToPreload(LoadedTagData.Tag, LoadedTagData.WeakOwner.Get());
                    }
                }
            }
        }
        else
        {
            UE_LOG(LogAfter, Warning,
                TEXT("UAfterGameplayCueManager::OnGameplayTagLoaded processed loaded tag(s) but RuntimeGameplayCueObjectLibrary.CueSet was "
                     "null. Skipping processing."));
        }
    }
}

void UAfterGameplayCueManager::ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject)
{
    switch (AfterGameplayCueManagerCvars::LoadMode)
    {
        case EAfterEditorLoadMode::LoadUpfront:
            return;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
            if (GIsEditor)
            {
                return;
            }
#endif
            break;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced:
            break;
    }

    check(RuntimeGameplayCueObjectLibrary.CueSet);

    int32* DataIdx = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueDataMap.Find(Tag);
    if (DataIdx && RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData.IsValidIndex(*DataIdx))
    {
        const FGameplayCueNotifyData& CueData = RuntimeGameplayCueObjectLibrary.CueSet->GameplayCueData[*DataIdx];

        UClass* LoadedGameplayCueClass = FindObject<UClass>(nullptr, *CueData.GameplayCueNotifyObj.ToString());
        if (LoadedGameplayCueClass)
        {
            RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject);
        }
        else
        {
            bool bAlwaysLoadedCue = OwningObject == nullptr;
            TWeakObjectPtr<UObject> WeakOwner = OwningObject;
            StreamableManager.RequestAsyncLoad(CueData.GameplayCueNotifyObj,
                FStreamableDelegate::CreateUObject(
                    this, &ThisClass::OnPreloadCueComplete, CueData.GameplayCueNotifyObj, WeakOwner, bAlwaysLoadedCue),
                FStreamableManager::DefaultAsyncLoadPriority, false, false, TEXT("GameplayCueManager"));
        }
    }
}

void UAfterGameplayCueManager::OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue)
{
    if (bAlwaysLoadedCue || OwningObject.IsValid())
    {
        if (UClass* LoadedGameplayCueClass = Cast<UClass>(Path.ResolveObject()))
        {
            RegisterPreloadedCue(LoadedGameplayCueClass, OwningObject.Get());
        }
    }
}

void UAfterGameplayCueManager::RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject)
{
    check(LoadedGameplayCueClass);

    const bool bAlwaysLoadedCue = OwningObject == nullptr;
    if (bAlwaysLoadedCue)
    {
        AlwaysLoadedCues.Add(LoadedGameplayCueClass);
        PreloadedCues.Remove(LoadedGameplayCueClass);
        PreloadedCueReferencers.Remove(LoadedGameplayCueClass);
    }
    else if ((OwningObject != LoadedGameplayCueClass) && (OwningObject != LoadedGameplayCueClass->GetDefaultObject()) &&
             !AlwaysLoadedCues.Contains(LoadedGameplayCueClass))
    {
        PreloadedCues.Add(LoadedGameplayCueClass);
        TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencers.FindOrAdd(LoadedGameplayCueClass);
        ReferencerSet.Add(OwningObject);
    }
}

void UAfterGameplayCueManager::HandlePostLoadMap(UWorld* NewWorld)
{
    if (RuntimeGameplayCueObjectLibrary.CueSet)
    {
        for (UClass* CueClass : AlwaysLoadedCues)
        {
            RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
        }

        for (UClass* CueClass : PreloadedCues)
        {
            RuntimeGameplayCueObjectLibrary.CueSet->RemoveLoadedClass(CueClass);
        }
    }

    for (auto CueIt = PreloadedCues.CreateIterator(); CueIt; ++CueIt)
    {
        TSet<FObjectKey>& ReferencerSet = PreloadedCueReferencers.FindChecked(*CueIt);
        for (auto RefIt = ReferencerSet.CreateIterator(); RefIt; ++RefIt)
        {
            if (!RefIt->ResolveObjectPtr())
            {
                RefIt.RemoveCurrent();
            }
        }
        if (ReferencerSet.Num() == 0)
        {
            PreloadedCueReferencers.Remove(*CueIt);
            CueIt.RemoveCurrent();
        }
    }
}

void UAfterGameplayCueManager::UpdateDelayLoadDelegateListeners()
{
    UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.RemoveAll(this);
    FCoreUObjectDelegates::GetPostGarbageCollect().RemoveAll(this);
    FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);

    switch (AfterGameplayCueManagerCvars::LoadMode)
    {
        case EAfterEditorLoadMode::LoadUpfront:
            return;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced_GameOnly:
#if WITH_EDITOR
            if (GIsEditor)
            {
                return;
            }
#endif
            break;
        case EAfterEditorLoadMode::PreloadAsCuesAreReferenced:
            break;
    }

    UGameplayTagsManager::Get().OnGameplayTagLoadedDelegate.AddUObject(this, &ThisClass::OnGameplayTagLoaded);
    FCoreUObjectDelegates::GetPostGarbageCollect().AddUObject(this, &ThisClass::HandlePostGarbageCollect);
    FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
}

bool UAfterGameplayCueManager::ShouldDelayLoadGameplayCues() const
{
    const bool bClientDelayLoadGameplayCues = true;
    return !IsRunningDedicatedServer() && bClientDelayLoadGameplayCues;
}
