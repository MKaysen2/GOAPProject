#include "..\Public\AIAct_Death.h"

UAIAct_Death::UAIAct_Death() : Super(
	{ },
	{ FWorldProperty(EWorldKey::kDead, true) },
	5)
{
	
}

bool UAIAct_Death::VerifyContext()
{
	return false;
}

EActionStatus UAIAct_Death::StartAction()
{
	return EActionStatus::kFailed;
}
