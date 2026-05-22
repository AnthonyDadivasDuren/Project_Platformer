#include "FallingPlatform.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"

AFallingPlatform::AFallingPlatform()
{
	PrimaryActorTick.bCanEverTick = false;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlatformMesh"));
	SetRootComponent(PlatformMesh);

	PlatformMesh->SetMobility(EComponentMobility::Movable);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PlatformMesh->SetCollisionObjectType(ECC_WorldStatic);
	PlatformMesh->SetCollisionResponseToAllChannels(ECR_Block);
	PlatformMesh->SetSimulatePhysics(false);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetupAttachment(PlatformMesh);

	TriggerBox->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f));

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	TriggerBox->SetGenerateOverlapEvents(true);
}

void AFallingPlatform::BeginPlay()
{
	Super::BeginPlay();
	
	InitialTransform = GetActorTransform();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFallingPlatform::OnTriggerBoxOverlap);
}

void AFallingPlatform::OnTriggerBoxOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bHasTriggered)
	{
		return;
	}

	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	bHasTriggered = true;

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (FallDelay <= 0.0f)
	{
		StartFalling();
		return;
	}

	GetWorldTimerManager().SetTimer(
		FallTimerHandle,
		this,
		&AFallingPlatform::StartFalling,
		FallDelay,
		false
	);
}

void AFallingPlatform::StartFalling()
{
	PlatformMesh->SetCollisionObjectType(ECC_PhysicsBody);
	PlatformMesh->SetSimulatePhysics(true);
	PlatformMesh->WakeAllRigidBodies();

	if (!InitialImpulse.IsNearlyZero())
	{
		PlatformMesh->AddImpulse(InitialImpulse, NAME_None, true);
	}

	if (bDisableAfterFalling)
	{
		GetWorldTimerManager().SetTimer(
			DestroyTimerHandle,
			this,
			&AFallingPlatform::DisablePlatform,
			DestroyDelay,
			false
		);
	}
}

void AFallingPlatform::DisablePlatform()
{
	PlatformMesh->SetSimulatePhysics(false);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorHiddenInGame(true);
}

void AFallingPlatform::ResetLevelObject_Implementation()
{
	GetWorldTimerManager().ClearTimer(FallTimerHandle);
	GetWorldTimerManager().ClearTimer(DestroyTimerHandle);

	bHasTriggered = false;

	SetActorHiddenInGame(false);
	SetActorTransform(InitialTransform, false, nullptr, ETeleportType::TeleportPhysics);

	PlatformMesh->SetSimulatePhysics(false);
	PlatformMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
	PlatformMesh->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	PlatformMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetGenerateOverlapEvents(true);
}