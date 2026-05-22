
#include "PlayerAirDashComponent.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UPlayerAirDashComponent::UPlayerAirDashComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
	
}


void UPlayerAirDashComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerAirDashComponent owner is not an ACharacter."));
		return;
	}
	
	MovementComponent = OwnerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerAirDashComponent could not find CharacterMovementComponent."));
		return;
	}
	
	OriginalGravityScale = MovementComponent->GravityScale;



}


void UPlayerAirDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!MovementComponent)
	{
		return;
	}

	if (bResetDashOnGround && MovementComponent->IsMovingOnGround() && !bIsDashing)
	{
		ResetDash();
	}
}
void UPlayerAirDashComponent::TryStartDash(const FVector& RequestedDirection)
{
	if (!OwnerCharacter || !MovementComponent)
	{
		return;
	}
	
	if (bIsDashing || !bDashAvailable)
	{
		return;
	}
	
	if (!MovementComponent->IsFalling())
	{
		return;
	}
	
	FVector DashDirection = RequestedDirection;
	
	DashDirection.Z = 0.0f;
	
	if (!DashDirection.Normalize())
	{
		DashDirection = OwnerCharacter->GetActorForwardVector();
		DashDirection.Z = 0.0f;
		DashDirection.Normalize();
	}
	
	bIsDashing = true;
	bDashAvailable = false;
	
	OriginalGravityScale = MovementComponent->GravityScale;
	MovementComponent->GravityScale = DashGravityScale;
	if (bStopMovementBeforeDash)
	{
		MovementComponent->StopMovementImmediately();
	}
	
	MovementComponent->Velocity = DashDirection * DashSpeed;
	
	OnDashStarted();
	
	GetWorld()->GetTimerManager().SetTimer(
		DashTimerHandle,
		this,
		&UPlayerAirDashComponent::EndDash,
		DashDuration,
		false);
}

void UPlayerAirDashComponent::EndDash()
{
	if (!MovementComponent)
	{
		return;
	}
	
	MovementComponent->GravityScale = OriginalGravityScale;
	bIsDashing = false;
	
	OnDashEnded();
	
	GetWorld()->GetTimerManager().SetTimer(
		DashCooldownTimerHandle,
		this,
		&UPlayerAirDashComponent::ResetDash,
		DashCooldown,
		false);
}

void UPlayerAirDashComponent::ResetDash()
{
	if (bIsDashing)
	{
		return;
	}
	
	if (MovementComponent->IsFalling())
	{
		return;
	}
	
	bDashAvailable = true;
}

bool UPlayerAirDashComponent::IsDashing() const 
{
	return bIsDashing;
}

bool UPlayerAirDashComponent::HasDashAvilable() const
{
	return bDashAvailable;
}

