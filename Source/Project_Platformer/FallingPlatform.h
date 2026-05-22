#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ResettableLevelObject.h"
#include "FallingPlatform.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_PLATFORMER_API AFallingPlatform : public AActor, public IResettableLevelObject
{
	GENERATED_BODY()

public:
	AFallingPlatform();
	
	virtual void ResetLevelObject_Implementation() override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> PlatformMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Platform", meta = (AllowPrivateAccess = "true"))
	float FallDelay = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Platform", meta = (AllowPrivateAccess = "true"))
	float DestroyDelay = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Platform", meta = (AllowPrivateAccess = "true"))
	bool bDisableAfterFalling = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Falling Platform", meta = (AllowPrivateAccess = "true"))
	FVector InitialImpulse = FVector::ZeroVector;

	bool bHasTriggered = false;

	FTimerHandle FallTimerHandle;
	FTimerHandle DestroyTimerHandle;

	UFUNCTION()
	void OnTriggerBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartFalling();
	void DisablePlatform();
	
	FTransform InitialTransform;
};