#include "System/GameplayTagStack.h"

#include "UObject/Stack.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayTagStack)

////////////////////////////////
// FGameplayTagStackContainer //
////////////////////////////////
void FGameplayTagStackContainer::AddStack(FGameplayTag Tag, int32 StackCount)
{
    if (!Tag.IsValid() || StackCount < 1) return;

    if (TagToCountMap.Contains(Tag))
    {
        const int32 NewCount = TagToCountMap[Tag] + StackCount;
        TagToCountMap[Tag] = NewCount;
        return;
    }

    TagToCountMap.Add(Tag, StackCount);
}

void FGameplayTagStackContainer::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
    if (!Tag.IsValid() || StackCount < 1) return;

    if (TagToCountMap.Contains(Tag))
    {
        if (TagToCountMap[Tag] <= StackCount)
        {
            TagToCountMap.Remove(Tag);
        }
        else
        {
            const int32 NewCount = TagToCountMap[Tag] - StackCount;
            TagToCountMap[Tag] = NewCount;
        }
        return;
    }
}