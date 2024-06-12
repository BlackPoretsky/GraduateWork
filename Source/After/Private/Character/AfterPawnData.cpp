#include "Character/AfterPawnData.h"

UAfterPawnData::UAfterPawnData(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    PawnClass = nullptr;
    InputConfig = nullptr;
    DefaultCameraMode = nullptr;
}
