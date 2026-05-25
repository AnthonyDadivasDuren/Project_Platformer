#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "TimerManager.h"
#include "PlayerAirDashComponent.h"
#include "PlayerWallMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ResettableLevelObject.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
	AirDashComponent = CreateDefaultSubobject<UPlayerAirDashComponent>(TEXT("AirDashComponent"));
	WallMovementComponent = CreateDefaultSubobject<UPlayerWallMovementComponent>(TEXT("WallMovementComponent"));
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	ApplyFixedCameraSettings();
	ApplyMovementSettings();
}


void APlayerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	ApplyFixedCameraSettings();
	ApplyMovementSettings();
}
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	RespawnTransform = GetActorTransform();
	
	const APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}
	
	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return;
	}
	
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
		LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	if (!InputSubsystem)
	{
		return;
	}
	
	if (PlayerMappingContext)
	{
		InputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsDead)
	{
		return;
	}
	
	UpdateJumpForgiveness(DeltaTime);
	TryConsumeBufferedJump();

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}
	
	if (MoveAction)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
	
	if (JumpAction)
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerCharacter::JumpStarted);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this , &APlayerCharacter::JumpStopped);
	}
	if (SprintAction)
	{
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APlayerCharacter::SprintStarted);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::SprintStopped);
	}
	if (DashAction)
	{
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &APlayerCharacter::DashStarted);
	}
}

void APlayerCharacter::SetFixedCameraSettings(float NewDistance, FRotator NewRotation, FVector NewTargetOffset)
{
	FixedCameraDistance = NewDistance;
	FixedCameraRotation = NewRotation;
	FixedCameraTargetOffset = NewTargetOffset;
	
	ApplyFixedCameraSettings();
}

void APlayerCharacter::KillPlayer()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	
	bIsJumpInputHeld = false;
	JumpBufferCounter = 0.0f;
	CoyoteTimeCounter = 0.0f;
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}
	
	OnPlayerDied();
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	
	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&APlayerCharacter::RespawnPlayer,
		RespawnDelay,
		false
		);

}


void APlayerCharacter::RespawnPlayer()
{
	ResetLevelObjects();
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	
	SetActorTransform(RespawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->SetMovementMode(MOVE_Walking);
	}
	
	bIsDead = false;
	
	OnPlayerRespawned();
	
}

void APlayerCharacter::SetRespawnTransform(const FTransform& NewRespawnTransform)
{
	RespawnTransform = NewRespawnTransform;
}
void APlayerCharacter::ApplyFixedCameraSettings()
{
	if (!CameraBoom)
	{
		return;
	}

	CameraBoom->TargetArmLength = FixedCameraDistance;
	CameraBoom->SetRelativeRotation(FixedCameraRotation);
	CameraBoom->TargetOffset = FixedCameraTargetOffset;
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bDoCollisionTest = false;

	if (FollowCamera)
	{
		FollowCamera->bUsePawnControlRotation = false;
	}
}

void APlayerCharacter::ApplyMovementSettings()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
	MovementComponent->MaxAcceleration = GroundAcceleration;
	MovementComponent->BrakingDecelerationWalking = GroundBrakingDeceleration;
	MovementComponent->GroundFriction = GroundFriction;
	MovementComponent->AirControl = AirControlStrength;
	MovementComponent->JumpZVelocity = JumpVelocity;
	MovementComponent->GravityScale = GravityStrength;

	MovementComponent->bOrientRotationToMovement = true;
	MovementComponent->RotationRate = FRotator(0.0f, MovementRotationRate, 0.0f);

	JumpMaxHoldTime = JumpHoldTime;
	JumpMaxCount = 1;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (bIsDead)
	{
		return;
	}
	
	const FVector2D MoveInput = Value.Get<FVector2D>();
	
	if (MoveInput.IsNearlyZero())
	{
		return;
	}
	
	if (!FollowCamera)
	{
		return;
	}
	
	FVector ForwardDirection = FollowCamera->GetForwardVector();
	ForwardDirection.Z = 0.0f;
	ForwardDirection.Normalize();
	
	FVector RightDirection = FollowCamera->GetRightVector();
	RightDirection.Z = 0.0f;
	RightDirection.Normalize();
	
	AddMovementInput(ForwardDirection, MoveInput.Y);
	AddMovementInput(RightDirection, MoveInput.X);
}

void APlayerCharacter::JumpStarted()
{
	if (bIsDead)
	{
		return;
	}
	
	bIsJumpInputHeld = true;
	JumpBufferCounter = JumpBufferTime;
}

void APlayerCharacter::JumpStopped()
{
	bIsJumpInputHeld = false;
	StopJumping();

	if (WallMovementComponent && WallMovementComponent->CutWallJumpShort())
	{
		return;
	}
	
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}
	
	if (MovementComponent->IsFalling() && MovementComponent->Velocity.Z > 0.0f)
	{
		MovementComponent->Velocity.Z *= JumpReleaseVelocityMultiplier;
	}
}

void APlayerCharacter::SprintStarted()
{
	if (bIsDead)
	{
		return;
	}
	
	bIsSprinting = true;
	ApplyCurrentMoveSpeed();
}

void APlayerCharacter::SprintStopped()
{
	bIsSprinting = false;
	ApplyCurrentMoveSpeed();
}

void APlayerCharacter::ApplyCurrentMoveSpeed()
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	MovementComponent->MaxWalkSpeed = bIsSprinting ? SprintSpeed : WalkSpeed;
}

void APlayerCharacter::DashStarted()
{
	if (bIsDead)
	{
		return;
	}
	
	if (!AirDashComponent)
	{
		return;
	}
	
	FVector DashDirection = FVector::ZeroVector;
	
	if (GetCharacterMovement())
	{
		DashDirection = GetCharacterMovement()->GetLastInputVector();
	}

	if (DashDirection.IsNearlyZero() && GetCharacterMovement())
	{
		DashDirection = GetCharacterMovement()->Velocity;
	}

	if (DashDirection.IsNearlyZero())
	{
		DashDirection = GetActorForwardVector();
	}

	AirDashComponent->TryStartDash(DashDirection);
}

void APlayerCharacter::UpdateJumpForgiveness(float DeltaTime)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}
	
	if (MovementComponent->IsMovingOnGround())
	{
		CoyoteTimeCounter = CoyoteTime;
	}
	else
	{
		CoyoteTimeCounter -= DeltaTime;
	}
	
	if (JumpBufferCounter > 0.0f)
	{
		JumpBufferCounter -= DeltaTime;
	}
}

void APlayerCharacter::TryConsumeBufferedJump()
{
	if (JumpBufferCounter <= 0.0f)
	{
		return;
	}

	if (WallMovementComponent && WallMovementComponent->TryWallJump())
	{
		if (bResetAirDashOnWallJump && AirDashComponent)
		{
			AirDashComponent->ResetDash();
		}

		JumpBufferCounter = 0.0f;
		CoyoteTimeCounter = 0.0f;
		return;
	}
	
	if (CoyoteTimeCounter <= 0.0f)
	{
		return;
	}
	
	Jump();
	
	if (!bIsJumpInputHeld)
	{
		StopJumping();
	}
	
	JumpBufferCounter = 0.0f;
	CoyoteTimeCounter = 0.0f;
}

void APlayerCharacter::ResetLevelObjects()
{
	TArray<AActor*> ResettableActors;
	UGameplayStatics::GetAllActorsWithInterface(
		GetWorld(),
		UResettableLevelObject::StaticClass(),
		ResettableActors
	);

	for (AActor* Actor : ResettableActors)
	{
		if (!Actor)
		{
			continue;
		}

		IResettableLevelObject::Execute_ResetLevelObject(Actor);
	}
}