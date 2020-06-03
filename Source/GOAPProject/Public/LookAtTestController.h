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

	UPROPERTY()
		bool bLockPawnToControlRotation = false;

	UPROPERTY()
		bool bClampYaw = false;

	UPROPERTY(EditAnywhere)
		float fHalfAngle = 60.0f;
public:
		virtual void Tick(float DeltaSeconds) override;

		virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;

		UFUNCTION(BlueprintCallable)
		void LookAtActor(AActor* FocusTarget, bool bClampRotationToHalfAngle);

		UFUNCTION(BlueprintCallable)
		void StopLookAt();
};