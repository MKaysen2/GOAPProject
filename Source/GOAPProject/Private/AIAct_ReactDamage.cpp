#include "../Public/AIAct_ReactDamage.h"

UAIAct_ReactDamage::UAIAct_ReactDamage()
{
}

bool UAIAct_ReactDamage::VerifyContext()
{
	return false;
}

EActionStatus UAIAct_ReactDamage::StartAction()
{
	return EActionStatus::kFailed;
}

void UAIAct_ReactDamage::StopAction()
{
}

void UAIAct_ReactDamage::AbortAction()
{
}
