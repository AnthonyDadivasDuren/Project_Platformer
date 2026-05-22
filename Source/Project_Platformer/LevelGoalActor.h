#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGoalActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_PLATFORMER_API ALevelGoalActor : public AActor
{
	GENERATED_BODY()

public:
	ALevelGoalActor();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Level Goal")
	void OnLevelCompleted();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> GoalTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Goal", meta = (AllowPrivateAccess = "true"))
	FName NextLevelName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Goal", meta = (AllowPrivateAccess = "true"))
	float LevelCompleteDelay = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Goal", meta = (AllowPrivateAccess = "true"))
	bool bLoadNextLevelOnComplete = true;

	UPROPERTY(BlueprintReadOnly, Category = "Level Goal", meta = (AllowPrivateAccess = "true"))
	bool bHasBeenCompleted = false;

	FTimerHandle LevelCompleteTimerHandle;

	UFUNCTION()
	void OnGoalTriggerOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void CompleteLevel();
	void LoadNextLevel();

	
};