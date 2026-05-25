

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelCameraSettingsActor.generated.h"

UCLASS()
class PROJECT_PLATFORMER_API ALevelCameraSettingsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ALevelCameraSettingsActor();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Camera", meta = (AllowPrivateAccess = true))
	float CameraDistance = 3500.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Camera", meta = (AllowPrivateAccess = true))
	FRotator CameraRotation = FRotator(-40.0f, -45.0f, 0.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Camera", meta = (AllowPrivateAccess = true))
	FVector CameraTargetOffset = FVector(0.0f, 0.0f, 300.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Camera", meta = (AllowPrivateAccess = true))
	bool bApplyOnBeginPlay = true;

};
