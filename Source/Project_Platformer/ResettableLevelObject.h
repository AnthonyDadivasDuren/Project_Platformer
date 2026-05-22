#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ResettableLevelObject.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UResettableLevelObject : public UInterface
{
	GENERATED_BODY()
};

class PROJECT_PLATFORMER_API IResettableLevelObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Level Reset")
	void ResetLevelObject();
};