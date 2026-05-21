#include "PlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(FName("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	
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

	MovementComponent->MaxWalkSpeed = GroundMoveSpeed;
	MovementComponent->MaxAcceleration = GroundAcceleration;
	MovementComponent->BrakingDecelerationWalking = GroundBrakingDeceleration;
	MovementComponent->GroundFriction = GroundFriction;
	MovementComponent->AirControl = AirControlStrength;
	MovementComponent->JumpZVelocity = JumpVelocity;
	MovementComponent->GravityScale = GravityScale;
	
	MovementComponent->bOrientRotationToMovement = true;
	
	MovementComponent->RotationRate = FRotator(0.0f, MovementRotationRate, 0.0f);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
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
	Jump();
}

void APlayerCharacter::JumpStopped()
{
	StopJumping();
}

