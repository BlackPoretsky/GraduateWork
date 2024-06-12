#include "GameFeature/GFA_WorldActionBase.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFA_WorldActionBase)

void UGFA_WorldActionBase::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
    GameInstanceStartHandles.FindOrAdd(Context) = FWorldDelegates::OnStartGameInstance.AddUObject(
        this, &UGFA_WorldActionBase::HandleGameInstanceStart, FGameFeatureStateChangeContext(Context));

    for (const FWorldContext& WorldContext : GEngine->GetWorldContexts())
    {
        if (Context.ShouldApplyToWorldContext(WorldContext))
        {
            AddToWorld(WorldContext, Context);
        }
    }
}

void UGFA_WorldActionBase::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
    FDelegateHandle* FoundHandle = GameInstanceStartHandles.Find(Context);
    if (ensure(FoundHandle))
    {
        FWorldDelegates::OnStartGameInstance.Remove(*FoundHandle);
    }
}

void UGFA_WorldActionBase::HandleGameInstanceStart(UGameInstance* GameInstance, FGameFeatureStateChangeContext ChangeContext)
{
    if (FWorldContext* WorldContext = GameInstance->GetWorldContext())
    {
        if (ChangeContext.ShouldApplyToWorldContext(*WorldContext))
        {
            AddToWorld(*WorldContext, ChangeContext);
        }
    }
}
