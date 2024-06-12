#pragma once

#include "NativeGameplayTags.h"

namespace AfterGameplayTags
{
    AFTER_API FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

    // Declare all of the custom native tags that After will use
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_AutoRun);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Select);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Drop);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Reload);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Weapon_Fire);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Heal);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_AutoRunning);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

    AFTER_API extern const TMap<uint8, FGameplayTag> MovementModeTagMap;
    AFTER_API extern const TMap<uint8, FGameplayTag> CustomMovementModeTagMap;

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_NavWalking);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);

    AFTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Custom);
};  // namespace AfterGameplayTags