#pragma once

#include "GameplayCueManager.h"

#include "AfterGameplayCueManager.generated.h"

class UClass;
class UObject;
class UWorld;
struct FObjectKey;
class IAfterAbilitySourceInterface;

/**
 * UAfterGameplayCueManager
 *
 */
UCLASS()
class UAfterGameplayCueManager : public UGameplayCueManager
{
    GENERATED_BODY()

public:
    UAfterGameplayCueManager(const FObjectInitializer& ObjectInitializer);

    static UAfterGameplayCueManager* Get();

    //~UGameplayCueManager interface
    virtual void OnCreated() override;
    virtual bool ShouldAsyncLoadRuntimeObjectLibraries() const override;
    virtual bool ShouldSyncLoadMissingGameplayCues() const override;
    virtual bool ShouldAsyncLoadMissingGameplayCues() const override;
    //~End of UGameplayCueManager interface

    static void DumGameplayCues(const TArray<FString>& Args);

    void LoadAlwaysLoadedCues();

    void RefreshGameplayCuePrimaryAsset();

private:
    void OnGameplayTagLoaded(const FGameplayTag& Tag);
    void HandlePostGarbageCollect();
    void ProcessLoadedTags();
    void ProcessTagToPreload(const FGameplayTag& Tag, UObject* OwningObject);
    void OnPreloadCueComplete(FSoftObjectPath Path, TWeakObjectPtr<UObject> OwningObject, bool bAlwaysLoadedCue);
    void RegisterPreloadedCue(UClass* LoadedGameplayCueClass, UObject* OwningObject);
    void HandlePostLoadMap(UWorld* NewWorld);
    void UpdateDelayLoadDelegateListeners();
    bool ShouldDelayLoadGameplayCues() const;

private:
    struct FLoadedGameplayTagToProcessData
    {
        FGameplayTag Tag;
        TWeakObjectPtr<UObject> WeakOwner;

        FLoadedGameplayTagToProcessData() {}
        FLoadedGameplayTagToProcessData(const FGameplayTag& InTag, const TWeakObjectPtr<UObject>& InWeakOwner)
            : Tag(InTag),
              WeakOwner(InWeakOwner)
        {
        }
    };

private:
    UPROPERTY(Transient)
    TSet<TObjectPtr<UClass>> PreloadedCues;
    TMap<FObjectKey, TSet<FObjectKey>> PreloadedCueReferencers;

    UPROPERTY(Transient)
    TSet<TObjectPtr<UClass>> AlwaysLoadedCues;

    TArray<FLoadedGameplayTagToProcessData> LoadedGameplayTagsToProcess;
    FCriticalSection LoadedGameplayTagsToProcessCS;
    bool bProcessLoadedTagsAfterGC = false;
};
