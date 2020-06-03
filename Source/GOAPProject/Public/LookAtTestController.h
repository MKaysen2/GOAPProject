#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LookAtTestController.generated.h"



UCLASS(BlueprintType, Blueprintable)
class GOAPPROJECT_API ALookAtTestController : public AAIController
{

	GENERATED_BODY()
private:

	UPROPERTY()
		FRotator CachedRotation;
public:
		virtual void Tick(float DeltaSeconds) override;
};