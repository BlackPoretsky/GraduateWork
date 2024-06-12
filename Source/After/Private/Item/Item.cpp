#include "Item/Item.h"
#include "Character/AfterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AItem::AItem()
    : DisplayName("None"),
      ItemState(EItemState::EIS_Pickup)
{

    PrimaryActorTick.bCanEverTick = true;

    DisplayMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
    SetRootComponent(DisplayMesh.Get());

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
    PickupWidget->SetupAttachment(GetRootComponent());

    AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
    AreaSphere->SetupAttachment(GetRootComponent());

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
    CollisionBox->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
    Super::BeginPlay();

    PickupWidget->SetVisibility(false);

    AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

    SetItemProperties(ItemState);
}

void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AItem::SetItemProperties(EItemState State)
{
    check(CollisionBox);
    switch (State)
    {
        case EItemState::EIS_Pickup:
            DisplayMesh->SetSimulatePhysics(false);
            DisplayMesh->SetVisibility(true);
            DisplayMesh->SetEnableGravity(false);
            DisplayMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

            CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            break;

        case EItemState::EIS_Equipped:
            PickupWidget->SetVisibility(false);

            DisplayMesh->SetSimulatePhysics(false);
            DisplayMesh->SetVisibility(true);
            DisplayMesh->SetEnableGravity(false);
            DisplayMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            DisplayMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            break;

        case EItemState::EIS_Falling:
            DisplayMesh->SetSimulatePhysics(true);
            DisplayMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            DisplayMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            DisplayMesh->SetEnableGravity(true);
            DisplayMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            CollisionBox.Get()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox.Get()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            break;
    }
}

void AItem::SetItemState(EItemState State)
{
    ItemState = State;
    SetItemProperties(ItemState);
}

void AItem::SetIngnoreAllCollisions() const
{
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AAfterCharacter* AfterCharacter = Cast<AAfterCharacter>(OtherActor);
        if (AfterCharacter)
        {
            AfterCharacter->IncrementOverlappedItemCount(1);
        }
    }
}

void AItem::OnSphereEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AAfterCharacter* AfterCharacter = Cast<AAfterCharacter>(OtherActor);
        if (AfterCharacter)
        {
            AfterCharacter->IncrementOverlappedItemCount(-1);
        }
    }
}