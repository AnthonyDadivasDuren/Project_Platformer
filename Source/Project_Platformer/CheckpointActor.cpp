#include "CheckpointActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "PlayerCharacter.h"

ACheckpointActor::ACheckpointActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointBox"));
	SetRootComponent(CheckpointBox);

	CheckpointBox->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	CheckpointBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CheckpointBox->SetCollisionObjectType(ECC_WorldDynamic);
	CheckpointBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CheckpointBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(CheckpointBox);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	RespawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(CheckpointBox);
	RespawnPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
}

void ACheckpointActor::BeginPlay()
{
	Super::BeginPlay();

	CheckpointBox->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointActor::OnCheckpointOverlap);
}

void ACheckpointActor::OnCheckpointOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	PlayerCharacter->SetRespawnTransform(RespawnPoint->GetComponentTransform());
}