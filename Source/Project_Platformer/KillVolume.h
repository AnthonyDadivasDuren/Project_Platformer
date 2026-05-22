#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KillVolume.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class PROJECT_PLATFORMER_API AKillVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	AKillVolume();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> KillBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> VisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard|Rotation", meta = (AllowPrivateAccess = "true"))
	bool bShouldRotate = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard|Rotation", meta = (AllowPrivateAccess = "true"))
	FRotator RotationSpeed = FRotator(0.0f, 90.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hazard|Rotation", meta = (AllowPrivateAccess = "true"))
	bool bUseLocalRotation = true;

	UFUNCTION()
	void OnKillBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};