#include "../Public/AIAct_ThrowGrenade.h"

UAIAct_ThrowGrenade::UAIAct_ThrowGrenade()
{
}

bool UAIAct_ThrowGrenade::VerifyContext()
{
	return false;
}

EActionStatus UAIAct_ThrowGrenade::StartAction()
{
	return EActionStatus::kFailed;
}

void UAIAct_ThrowGrenade::StopAction()
{
}

void UAIAct_ThrowGrenade::AbortAction()
{
}
