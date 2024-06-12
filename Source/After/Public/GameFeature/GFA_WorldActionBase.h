#pragma once

#include "GameFeatureAction.h"
#include "GameFeaturesSubsystem.h"

#include "GFA_WorldActionBase.generated.h"

class FDelegateHandle;
class UGameInstance;
class UObject;

struct FGameFeatureActivatingContext;
struct FGameFeatureDeactivatingContext;
struct FGameFeatureStateChangeContext;
struct FWorldContext;

/**
 *
 */
UCLASS(Abstract)
class AFTER_API UGFA_WorldActionBase : public UGameFeatureAction
{
    GENERATED_BODY()

public:
    //~ Begin UGameFeatureAction interface
    virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
    virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
    //~ End UGameFeatureAction interface

private:
    void HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext);

    virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
        PURE_VIRTUAL(UGameFeatureAction_WorldActionBase::AddToWorld, );

private:
    TMap<FGameFeatureStateChangeContext, FDelegateHandle> GameInstanceStartHandles;
};
