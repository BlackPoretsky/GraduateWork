#include "System/AfterGameData.h"
#include "System/AfterAssetManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterGameData)

UAfterGameData::UAfterGameData() {}

const UAfterGameData& UAfterGameData::Get()
{
    return UAfterAssetManager::Get().GetGameData();
}
