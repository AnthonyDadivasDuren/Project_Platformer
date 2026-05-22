
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerAirDashComponent.generated.h"

class ACharacter;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLATFORMER_API UPlayerAirDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerAirDashComponent();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Air Dash")
	void TryStartDash(const FVector& RequestDirection);
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Air Dash")
	void ResetDash();
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Air Dash")
	bool IsDashing() const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement|Air Dash")
	bool HasDashAvilable() const;
	
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	float DashSpeed = 3200.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	float DashDuration = 0.14f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	float DashCooldown = 0.05f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	float DashGravityScale = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	bool bResetDashOnGround = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement|Air Dash")
	bool bStopMovementBeforeDash = true;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air Dash")
	bool bIsDashing = false;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Air Dash")
	bool bDashAvailable = true;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Air Dash")
	void OnDashStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Movement|Air Dash")
	void OnDashEnded();

private:
	void EndDash();

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	FTimerHandle DashTimerHandle;
	FTimerHandle DashCooldownTimerHandle;

	float OriginalGravityScale = 1.0f;
};
