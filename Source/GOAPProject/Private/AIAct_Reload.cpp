#include "..\Public\AIAct_Reload.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"

UAIAct_Reload::UAIAct_Reload() :
	Super(
		{ FWorldProperty(EWorldKey::kHasWeapon, true) },
		{ FWorldProperty(EWorldKey::kWeaponLoaded, true) },
		1
	)
{
}

bool UAIAct_Reload::VerifyContext()
{
	if (!AIOwner)
	{
		return false;
	}
	APawn* Pawn = AIOwner->GetPawn();

	if (!Pawn)
	{
		return false;
	}
	return true;
}

EActionStatus UAIAct_Reload::StartAction()
{
	Super::StartAction();
	return EActionStatus::kFailed;
}