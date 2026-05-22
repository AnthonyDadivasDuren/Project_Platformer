#include "PlayerWallMovementComponent.h"

#include "DrawDebugHelpers.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"

UPlayerWallMovementComponent::UPlayerWallMovementComponent()
{
	
	PrimaryComponentTick.bCanEverTick = true;


}

void UPlayerWallMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerWallMovementComponent Owner is not an ACharacter"));
		return;
	}
	
	MovementComponent = OwnerCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerWallMovementComponent could not find CharacterMovementComponent."));
		return;
	}
}


void UPlayerWallMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!OwnerCharacter || !MovementComponent)
	{
		return;
	}
	
	UpdateWallState();
	ApplyWallSlide();
}

bool UPlayerWallMovementComponent::CanWallJump() const
{
	if (!MovementComponent)
	{
		return false;
	}
	
	return bIsTouchingWall && MovementComponent->IsFalling() && !bWallJumpInputLocked;
}

bool UPlayerWallMovementComponent::TryWallJump()
{
	if (!CanWallJump())
	{
		return false;
	}
	
	FVector WallJumpVelocity = CurrentWallNormal * WallJumpHorizontalForce;
	WallJumpVelocity.Z = WallJumpVerticalForce;
	
	MovementComponent->StopMovementImmediately();
	MovementComponent->Velocity = WallJumpVelocity;
	MovementComponent->SetMovementMode(MOVE_Falling);
	
	bPerformedWallJump = true;
	bIsWallSliding = false;
	bIsTouchingWall = false;
	bWallJumpInputLocked = true;
	
	OnWallJumped();
	
	if (UWorld* World = GetWorld())
	{
	World->GetTimerManager().SetTimer(
			WallJumpInputLockTimerHandle,
			[this]()
			{
				bWallJumpInputLocked = false;
			},
			WallJumpInputLockTime,
			false
		);
	}

	return true;
}

bool UPlayerWallMovementComponent::IsWallSliding() const
{
	return bIsWallSliding;
}

bool UPlayerWallMovementComponent::IsTouchingWall() const
{
	return bIsTouchingWall;
}

FVector UPlayerWallMovementComponent::GetCurrentWallNormal() const
{
	return CurrentWallNormal;
}

void UPlayerWallMovementComponent::UpdateWallState()
{
	bWasWallSlidingLastFrame = bIsWallSliding;

	if (!MovementComponent->IsFalling())
	{
		bPerformedWallJump = false;
		ClearWallState();
		return;
	}

	FHitResult WallHit;
	const bool bFoundWall = FindWall(WallHit);

	if (!bFoundWall)
	{
		ClearWallState();
		return;
	}

	bIsTouchingWall = true;
	CurrentWallNormal = WallHit.ImpactNormal;

	const bool bIsFallingDown = MovementComponent->Velocity.Z < 0.0f;
	bIsWallSliding = bIsFallingDown;

	if (bIsWallSliding && !bWasWallSlidingLastFrame)
	{
		OnWallSlideStarted();
	}
	else if (!bIsWallSliding && bWasWallSlidingLastFrame)
	{
		OnWallSlideEnded();
	}
}

void UPlayerWallMovementComponent::ApplyWallSlide()
{
	if (!bIsWallSliding || !MovementComponent)
	{
		return;
	}

	if (MovementComponent->Velocity.Z < -WallSlideMaxFallSpeed)
	{
		MovementComponent->Velocity.Z = -WallSlideMaxFallSpeed;
	}
}

bool UPlayerWallMovementComponent::FindWall(FHitResult& OutHit) const
{
	if (!OwnerCharacter)
	{
		return false;
	}

	const FVector Start = OwnerCharacter->GetActorLocation();

	const FVector Forward = OwnerCharacter->GetActorForwardVector();
	const FVector Right = OwnerCharacter->GetActorRightVector();

	const FVector TraceDirections[] =
	{
		Forward,
		-Forward,
		Right,
		-Right
	};

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	for (const FVector& Direction : TraceDirections)
	{
		const FVector End = Start + Direction * WallDetectionDistance;

		FHitResult Hit;
		const bool bHit = GetWorld()->SweepSingleByChannel(
			Hit,
			Start,
			End,
			FQuat::Identity,
			ECC_Visibility,
			FCollisionShape::MakeSphere(WallTraceRadius),
			QueryParams
		);

		if (bDrawDebugTraces)
		{
			const FColor DebugColor = bHit ? FColor::Green : FColor::Red;
			DrawDebugSphere(GetWorld(), End, WallTraceRadius, 12, DebugColor, false, 0.0f);
			DrawDebugLine(GetWorld(), Start, End, DebugColor, false, 0.0f, 0, 2.0f);
		}

		if (bHit && IsValidWallHit(Hit))
		{
			OutHit = Hit;
			return true;
		}
	}

	return false;
}

bool UPlayerWallMovementComponent::IsValidWallHit(const FHitResult& Hit) const
{
	if (!Hit.bBlockingHit)
	{
		return false;
	}

	const FVector Normal = Hit.ImpactNormal;
	const bool bSurfaceIsWallLike = FMath::Abs(Normal.Z) <= ValidWallNormalZLimit;

	if (!bSurfaceIsWallLike)
	{
		return false;
	}

	if (bUseBlockedWallTag && HasBlockedWallTag(Hit))
	{
		return false;
	}

	return true;
}

bool UPlayerWallMovementComponent::HasBlockedWallTag(const FHitResult& Hit) const
{
	if (BlockedWallTag.IsNone())
	{
		return false;
	}

	const UPrimitiveComponent* HitComponent = Hit.GetComponent();
	if (HitComponent && HitComponent->ComponentHasTag(BlockedWallTag))
	{
		return true;
	}

	const AActor* HitActor = Hit.GetActor();
	if (HitActor && HitActor->ActorHasTag(BlockedWallTag))
	{
		return true;
	}

	return false;
}

void UPlayerWallMovementComponent::ClearWallState()
{
	if (bIsWallSliding)
	{
		OnWallSlideEnded();
	}

	bIsWallSliding = false;
	bIsTouchingWall = false;
	CurrentWallNormal = FVector::ZeroVector;
}
bool UPlayerWallMovementComponent::CutWallJumpShort()
{
	if (!bPerformedWallJump || !MovementComponent)
	{
		return false;
	}

	if (MovementComponent->IsFalling() && MovementComponent->Velocity.Z > 0.0f)
	{
		MovementComponent->Velocity.Z *= WallJumpReleaseVelocityMultiplier;
		bPerformedWallJump = false;
		return true;
	}

	return false;
}