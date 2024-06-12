#pragma once

#include "AbilitySystem/Abilities/AfterAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "AfterAbilityCost_InventoryItem.generated.h"

struct FGameplayAvilityActivationInfo;
struct FGamaplayAbilitySpecHandle;

class UAfterGameplayAbility;
class UAfterInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Представляет количествл затрат запрашивание для предмета.
 */
UCLASS(meta = (DisplayName = "Inventory Item"))
class UAfterAbilityCost_InventoryItem : public UAfterAbilityCost
{
    GENERATED_BODY()

public:
    UAfterAbilityCost_InventoryItem();

    //~UAfterAbilityCost interface
    virtual bool CheckCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
    virtual void ApplyCost(const UAfterGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle,
        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
    //~End of UAfterAbilityCost interface

protected:
    /** Требуемые затраты. */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityCost")
    FScalableFloat Quantity;

    /** К какому предммету прмиенять */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AbilityCost")
    TSubclassOf<UAfterInventoryItemDefinition> ItemDefinition;
};
