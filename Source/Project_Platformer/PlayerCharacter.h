#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UPlayerAirDashComponent;
class UPlayerWallMovementComponent;


UCLASS()
class PROJECT_PLATFORMER_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Player|Death")
	void KillPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Player|Respawn")
	void RespawnPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Player|Respawn")
	void SetRespawnTransform(const FTransform& NewRespawnTransform);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player|Respawn")
	float RespawnDelay = 0.35f;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Events")
	void OnPlayerDied();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player|Events")
	void OnPlayerRespawned();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerAirDashComponent> AirDashComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPlayerWallMovementComponent> WallMovementComponent;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Fixed Follow")
	float FixedCameraDistance = 1800.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Fixed Follow")
	FRotator FixedCameraRotation = FRotator(-55.0f, -45.0f, 0.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Fixed Follow")
	FVector FixedCameraTargetOffset = FVector(0.0f, 0.0f, 250.0f);
	
	UFUNCTION(BlueprintCallable, Category = "Camera|Fixed Follow")
	void ApplyFixedCameraSettings();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float WalkSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float SprintSpeed = 1450.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundAcceleration = 6500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundBrakingDeceleration = 7000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundFriction = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air")
	float AirControlStrength = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpVelocity = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpHoldTime = 0.18f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpReleaseVelocityMultiplier = 0.45f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float CoyoteTime = 0.10f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpBufferTime = 0.10f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Wall Jump")
	bool bResetAirDashOnWallJump = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Gravity")
	float GravityStrength = 2.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
	float MovementRotationRate = 1440.0f;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyMovementSettings();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    TObjectPtr<UInputAction> SprintAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DashAction;
private:
	void Move(const FInputActionValue& Value);
	void JumpStarted();
	void JumpStopped();
	void SprintStarted();
	void SprintStopped();
	void ApplyCurrentMoveSpeed();
	void DashStarted();
	
	void UpdateJumpForgiveness(float DeltaTime);
	void TryConsumeBufferedJump();

private:
	bool bIsSprinting = false;
	bool bIsJumpInputHeld = false;
	
	float CoyoteTimeCounter = 0.0f;
	float JumpBufferCounter = 0.0f;
	
	FTransform RespawnTransform;
	FTimerHandle RespawnTimerHandle;

	bool bIsDead = false;
	
	
};
