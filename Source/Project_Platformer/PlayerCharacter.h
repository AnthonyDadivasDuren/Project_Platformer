#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;


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
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> FollowCamera;
	
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
	float GroundMoveSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundAcceleration = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundBrakingDeceleration = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Ground")
	float GroundFriction = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air")
	float AirControlStrength = 0.85f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Jump")
	float JumpVelocity = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Gravity")
	float GravityScale = 2.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Rotation")
	float MovementRotationRate = 720.0f;

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ApplyMovementSettings();
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	
private:
	void Move(const FInputActionValue& Value);
	void JumpStarted();
	void JumpStopped();
};
