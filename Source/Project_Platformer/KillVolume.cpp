#include "KillVolume.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"

AKillVolume::AKillVolume()
{
	PrimaryActorTick.bCanEverTick = true;

	KillBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KillBox"));
	SetRootComponent(KillBox);

	KillBox->SetBoxExtent(FVector(100.0f, 100.0f, 100.0f));
	KillBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillBox->SetCollisionObjectType(ECC_WorldDynamic);
	KillBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	KillBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	KillBox->SetGenerateOverlapEvents(true);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(KillBox);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKillVolume::BeginPlay()
{
	Super::BeginPlay();

	KillBox->OnComponentBeginOverlap.AddDynamic(this, &AKillVolume::OnKillBoxOverlap);
}

void AKillVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bShouldRotate)
	{
		return;
	}

	const FRotator RotationThisFrame = RotationSpeed * DeltaTime;

	if (bUseLocalRotation)
	{
		AddActorLocalRotation(RotationThisFrame);
	}
	else
	{
		AddActorWorldRotation(RotationThisFrame);
	}
}

void AKillVolume::OnKillBoxOverlap(
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

	PlayerCharacter->KillPlayer();
}