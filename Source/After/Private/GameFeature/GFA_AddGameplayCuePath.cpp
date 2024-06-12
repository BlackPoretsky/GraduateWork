#include "GameFeature/GFA_AddGameplayCuePath.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GFA_AddGameplayCuePath)

#define LOCTEXT_NAMESPACE "GameFeatures"

UGFA_AddGameplayCuePath::UGFA_AddGameplayCuePath()
{
    DirectoryPathsToAdd.Add(FDirectoryPath{TEXT("/GameplayCues")});
}

#if WITH_EDITOR
EDataValidationResult UGFA_AddGameplayCuePath::IsDataValid(FDataValidationContext& Context) const
{
    EDataValidationResult Result = Super::IsDataValid(Context);

    for (const FDirectoryPath& Directory : DirectoryPathsToAdd)
    {
        if (Directory.Path.IsEmpty())
        {
            const FText InvalidCuePathError =
                FText::Format(LOCTEXT("InvalidCuePathError", "'{0}' is not a valid path!"), FText::FromString(Directory.Path));
            Context.AddError(InvalidCuePathError);
            Result = CombineDataValidationResults(Result, EDataValidationResult::Invalid);
        }
    }

    return CombineDataValidationResults(Result, EDataValidationResult::Valid);
}
#endif

#undef LOCTEXT_NAMESPACE
