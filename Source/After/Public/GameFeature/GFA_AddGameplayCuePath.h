#pragma once

#include "GameFeatureAction.h"
#include "GFA_AddGameplayCuePath.generated.h"

/**
 * .
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Gameplay Cue Path"))
class UGFA_AddGameplayCuePath : public UGameFeatureAction
{
    GENERATED_BODY()

public:
    UGFA_AddGameplayCuePath();

    //~UObject interface
#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
    //~End of UObject interface

    const TArray<FDirectoryPath>& GetDirectoryPathsToAdd() const { return DirectoryPathsToAdd; }

private:
    UPROPERTY(EditAnywhere, Category = "Game Feature | Gamepaly Cues", meta = (RelativeToGameContentDir, LongPackageName))
    TArray<FDirectoryPath> DirectoryPathsToAdd;
};
