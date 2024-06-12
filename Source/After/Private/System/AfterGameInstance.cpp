#include "System/AfterGameInstance.h"

#include "Components/GameFrameworkComponentManager.h"
#include "AfterGameplayTags.h"

UAfterGameInstance::UAfterGameInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void UAfterGameInstance::Init()
{
    Super::Init();

    UGameFrameworkComponentManager* ComponentManager = GetSubsystem<UGameFrameworkComponentManager>();

    if (ensure(ComponentManager))
    {
        ComponentManager->RegisterInitState(AfterGameplayTags::InitState_Spawned, false, FGameplayTag());
        ComponentManager->RegisterInitState(AfterGameplayTags::InitState_DataAvailable, false, AfterGameplayTags::InitState_Spawned);
        ComponentManager->RegisterInitState(
            AfterGameplayTags::InitState_DataInitialized, false, AfterGameplayTags::InitState_DataAvailable);
        ComponentManager->RegisterInitState(
            AfterGameplayTags::InitState_GameplayReady, false, AfterGameplayTags::InitState_DataInitialized);
    }
}
