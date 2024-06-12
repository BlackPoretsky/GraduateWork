// Fill out your copyright notice in the Description page of Project Settings.

#include "Animation/AfterAnimNotify.h"

void UAfterAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    OnNotified.Broadcast(MeshComp);
    Super::Notify(MeshComp, Animation);
}
