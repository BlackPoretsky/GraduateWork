#pragma once

#include "Engine/StreamableManager.h"

DECLARE_DELEGATE_OneParam(FAfterAssetManagerStartupJobSubstepProgress, float);

struct FAfterAssetManagerStartupJob
{
    FAfterAssetManagerStartupJobSubstepProgress SubstepProgressDelegate;
    TFunction<void(const FAfterAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)> JobFunc;
    FString JobName;
    float JobWeight;
    mutable double LastUpdate = 0;

    FAfterAssetManagerStartupJob(const FString& InJobName,
        const TFunction<void(const FAfterAssetManagerStartupJob&, TSharedPtr<FStreamableHandle>&)>& InJobFunc, float InJobWeight)
        : JobFunc(InJobFunc),
          JobName(InJobName),
          JobWeight(InJobWeight)
    {
    }

    TSharedPtr<FStreamableHandle> DoJob() const;

    void UpdateSubstepProgress(float NewProgress) const { SubstepProgressDelegate.ExecuteIfBound(NewProgress); }

    void UpdateSubstepProgressFromStreamable(TSharedRef<FStreamableHandle> StreamableHandle) const
    {
        if (SubstepProgressDelegate.IsBound())
        {
            double Now = FPlatformTime::Seconds();
            if (LastUpdate - Now > 1.0 / 60)
            {
                SubstepProgressDelegate.Execute(StreamableHandle->GetProgress());
                LastUpdate = Now;
            }
        }
    }
};
