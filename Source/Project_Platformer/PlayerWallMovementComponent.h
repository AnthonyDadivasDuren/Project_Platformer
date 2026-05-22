#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWallMovementComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLATFORMER_API UPlayerWallMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerWallMovementComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Wall")
	bool CanWallJump() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Wall")
	bool TryWallJump();
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Wall")
	bool IsWallSliding() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Wall")
	bool IsTouchingWall() const;
	
	UFUNCTION( BlueprintCallable, Category = "Movement|Wall")
	FVector GetCurrentWallNormal() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Wall")
	bool CutWallJumpShort();
		
	protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Detection" )
	float WallDetectionDistance = 90.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Detection" )
	float WallTraceRadius = 35.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Detection" )
	float ValidWallNormalZLimit = 0.35f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Detection")
	bool bUseBlockedWallTag = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Detection")
	FName BlockedWallTag = TEXT("NoWallMovement");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Slide" )
	float WallSlideMaxFallSpeed = 650.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Jump" )
	float WallJumpHorizontalForce = 1600.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Jump" )
	float WallJumpVerticalForce = 1450.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Jump" )
	float WallJumpInputLockTime = 0.12f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Jump")
	float WallJumpReleaseVelocityMultiplier = 0.45f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Debug" )
	bool bDrawDebugTraces = false;
	
	UPROPERTY (BlueprintReadOnly, Category = "Movement|Wall" )
	bool bIsWallSliding = false;
	
	UPROPERTY( BlueprintReadOnly, Category = "Movement|Wall" )
	bool bIsTouchingWall = false;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Wall")
	void OnWallSlideStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Wall")
	void OnWallSlideEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Wall")
	void OnWallJumped();

	private:
	void UpdateWallState();
	void ApplyWallSlide();
	bool FindWall(FHitResult& OutHit) const;
	bool IsValidWallHit(const FHitResult& Hit) const;
	void ClearWallState();
	bool bPerformedWallJump = false;
	bool HasBlockedWallTag(const FHitResult& Hit) const;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	FVector CurrentWallNormal = FVector::ZeroVector;
	bool bWasWallSlidingLastFrame = false;
	bool bWallJumpInputLocked = false;

	FTimerHandle WallJumpInputLockTimerHandle;
};

