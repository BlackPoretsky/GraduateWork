#pragma once

#include "Logging/LogMacros.h"

#include "AfterCoreTypes.generated.h"

#define PRIMARY_WEAPON_INDEX 0
#define SECONDARY_WEAPON_INDEX 1
#define MELEE_WEAPON_INDEX 2

// weapon
class AWeapon;
class AMeleeWeapon;
class ARangedWeapon;

DECLARE_MULTICAST_DELEGATE(FOnClipEmptySignature)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayAnimMontage, UAnimMontage*)

    UENUM(BlueprintType) enum class ERangedWeaponCategory : uint8
{
    ERWC_Primary UMETA(DisplayName = "Primary"),
    ERWC_Secondary UMETA(DisplayName = "Secondary")
};

USTRUCT(BlueprintType)
struct FAmmoData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    int32 Bullets = 15;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!Infinite"))
    int32 Clips = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (EditCondition = "!Infinite"))
    int32 MaxClips = 10;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    bool Infinite = false;
};

USTRUCT(BlueprintType)
struct FWeaponAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* AttackAnimMotage;
};

USTRUCT(BlueprintType)
struct FMeleeWeaponAnimations : public FWeaponAnimations
{
    GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FRangedWeaponAnimations : public FWeaponAnimations
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    UAnimMontage* ReloadAnimMotage;
};

USTRUCT(BlueprintType)
struct FArsenalData
{
    GENERATED_BODY()

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeapon> MainWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeapon> SecondaryWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
    TSubclassOf<AWeapon> MeleeWeapon;

public:
    TArray<TSubclassOf<AWeapon>> GetDefaultArsenal() { return {MainWeapon, SecondaryWeapon, MeleeWeapon}; }
};

// health

DECLARE_MULTICAST_DELEGATE(FOnDeath)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnChangedHealth, float)

    UENUM(BlueprintType) enum class EAfterAbilitiInputID : uint8
{
    None,
    Confirm,
    Cancel,
    Attack
};