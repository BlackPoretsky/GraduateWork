#pragma once

#include "GameplayTagContainer.h"

#include "GameplayTagStack.generated.h"

USTRUCT(BlueprintType)
struct FGameplayTagStackContainer
{
    GENERATED_BODY();

    FGameplayTagStackContainer() {}

    void AddStack(FGameplayTag Tag, int32 StackCount);

    void RemoveStack(FGameplayTag Tag, int32 StackCount);

    int32 GetStackCount(FGameplayTag Tag) const { return TagToCountMap.FindRef(Tag); }

    bool ContainsTag(FGameplayTag Tag) const { return TagToCountMap.Contains(Tag); }

private:
    TMap<FGameplayTag, int32> TagToCountMap;
};