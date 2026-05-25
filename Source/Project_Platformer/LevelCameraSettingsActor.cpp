#include "LevelCameraSettingsActor.h"

#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

ALevelCameraSettingsActor::ALevelCameraSettingsActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALevelCameraSettingsActor::BeginPlay()
{
	Super::BeginPlay();

	if (!bApplyOnBeginPlay)
	{
		return;
	}

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(
		UGameplayStatics::GetPlayerCharacter(this, 0)
	);

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelCameraSettingsActor could not find APlayerCharacter."));
		return;
	}

	PlayerCharacter->SetFixedCameraSettings(
		CameraDistance,
		CameraRotation,
		CameraTargetOffset
	);
}