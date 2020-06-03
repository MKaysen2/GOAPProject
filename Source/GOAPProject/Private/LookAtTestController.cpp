#include "..\Public\LookAtTestController.h"

void ALookAtTestController::Tick(float DeltaTime)
{
	AActor::Tick(DeltaTime);
	UpdateControlRotation(DeltaTime, bLockPawnToControlRotation);
}
void ALookAtTestController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		FRotator NewControlRotation = GetControlRotation();

		// Look toward focus
		const FVector FocalPoint = GetFocalPoint();
		if (FAISystem::IsValidLocation(FocalPoint))
		{
			NewControlRotation = (FocalPoint - MyPawn->GetPawnViewLocation()).Rotation();
			if (bClampYaw)
			{
				float NewYaw = FRotator::ClampAxis(NewControlRotation.Yaw);
				float CachedYaw = FRotator::NormalizeAxis(CachedRotation.Yaw);
				float DeltaYaw = FRotator::NormalizeAxis(NewYaw - CachedYaw);

				float ClampedDelta = FMath::Min(FMath::Max(DeltaYaw, -fHalfAngle), fHalfAngle);
				NewControlRotation.Yaw = CachedYaw + ClampedDelta;
				NewControlRotation.Clamp();
			}
		}
		else if (bSetControlRotationFromPawnOrientation)
		{
			NewControlRotation = MyPawn->GetActorRotation();
		}

		// Don't pitch view unless looking at another pawn
		if (NewControlRotation.Pitch != 0 && Cast<APawn>(GetFocusActor()) == nullptr)
		{
			NewControlRotation.Pitch = 0.f;
		}

		SetControlRotation(NewControlRotation);

		if (bUpdatePawn)
		{
			const FRotator CurrentPawnRotation = MyPawn->GetActorRotation();

			if (CurrentPawnRotation.Equals(NewControlRotation, 1e-3f) == false)
			{
				MyPawn->FaceRotation(NewControlRotation, DeltaTime);
			}
		}
	}
}

void ALookAtTestController::LookAtActor(AActor* FocusTarget, bool bClampRotationToHalfAngle=true)
{
	CachedRotation = GetControlRotation();

	SetFocus(FocusTarget);
	bClampYaw = bClampRotationToHalfAngle;
}

void ALookAtTestController::StopLookAt()
{
	ClearFocus(EAIFocusPriority::Gameplay);
	SetControlRotation(CachedRotation);
}
