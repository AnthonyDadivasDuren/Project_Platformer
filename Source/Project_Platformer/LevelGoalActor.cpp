#include "LevelGoalActor.h"

#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "TimerManager.h"

ALevelGoalActor::ALevelGoalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GoalTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("GoalTrigger"));
	SetRootComponent(GoalTrigger);

	GoalTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 150.0f));
	GoalTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GoalTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	GoalTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	GoalTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	GoalTrigger->SetGenerateOverlapEvents(true);

	VisualMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualMesh"));
	VisualMesh->SetupAttachment(GoalTrigger);
	VisualMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ALevelGoalActor::BeginPlay()
{
	Super::BeginPlay();

	GoalTrigger->OnComponentBeginOverlap.AddDynamic(this, &ALevelGoalActor::OnGoalTriggerOverlap);
}

void ALevelGoalActor::OnGoalTriggerOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (bHasBeenCompleted)
	{
		return;
	}

	const APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerCharacter)
	{
		return;
	}

	CompleteLevel();
}

void ALevelGoalActor::CompleteLevel()
{
	if (bHasBeenCompleted)
	{
		return;
	}

	bHasBeenCompleted = true;

	GoalTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OnLevelCompleted();

	if (!bLoadNextLevelOnComplete)
	{
		return;
	}

	if (LevelCompleteDelay <= 0.0f)
	{
		LoadNextLevel();
		return;
	}

	GetWorldTimerManager().SetTimer(
		LevelCompleteTimerHandle,
		this,
		&ALevelGoalActor::LoadNextLevel,
		LevelCompleteDelay,
		false
	);
}

void ALevelGoalActor::LoadNextLevel()
{
	if (NextLevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelGoalActor has no NextLevelName set."));
		return;
	}

	UGameplayStatics::OpenLevel(this, NextLevelName);
}