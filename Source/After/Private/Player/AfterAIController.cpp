#include "Player/AfterAIController.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/World.h"
#include "GameFramework/PlayerState.h"
#include "AfterLogChannels.h"
#include "Character/AfterPawnExtensionComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AfterAIController)

AAfterAIController::AAfterAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bWantsPlayerState = true;
    bStopAILogicOnUnposses = false;
    bPawnExtinsionComponentInit = false;
}

void AAfterAIController::OnUnPossess()
{
    if (APawn* PawnBeingUnpossessed = GetPawn())
    {
        if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerState))
        {
            if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
            {
                ASC->SetAvatarActor(nullptr);
            }
        }
    }

    Super::OnUnPossess();
}

void AAfterAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bPawnExtinsionComponentInit)
    {
        if (GetPawn() != nullptr)
        {
            if (UAfterPawnExtensionComponent* PawnExtComponent = GetPawn()->FindComponentByClass<UAfterPawnExtensionComponent>())
            {
                PawnExtComponent->CheckDefaultInitialization();
                bPawnExtinsionComponentInit = true;
            }
        }
    }
}
