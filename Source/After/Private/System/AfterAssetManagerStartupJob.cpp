#include "System/AfterAssetManagerStartupJob.h"

#include "AfterLogChannels.h"

TSharedPtr<FStreamableHandle> FAfterAssetManagerStartupJob::DoJob() const
{
    const double JobStartTime = FPlatformTime::Seconds();

    TSharedPtr<FStreamableHandle> Handle;
    UE_LOG(LogAfter, Display, TEXT("Startup job \"%s\" starting"), *JobName);
    JobFunc(*this, Handle);

    if (Handle.IsValid())
    {
        Handle->BindUpdateDelegate(
            FStreamableUpdateDelegate::CreateRaw(this, &FAfterAssetManagerStartupJob::UpdateSubstepProgressFromStreamable));
        Handle->WaitUntilComplete(0.0f, false);
        Handle->BindUpdateDelegate(FStreamableUpdateDelegate());
    }

    UE_LOG(LogAfter, Display, TEXT("Startup job \"%s\" took %.2f seconds to complete"), *JobName, FPlatformTime::Seconds() - JobStartTime);

    return Handle;
}
