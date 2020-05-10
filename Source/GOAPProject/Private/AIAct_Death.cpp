#include "..\Public\AIAct_Death.h"

UAIAct_Death::UAIAct_Death() : Super(
	{ },
	{ FWorldProperty(EWorldKey::kDead, true) },
	5)
{
	
}

bool UAIAct_Death::VerifyContext(AAIController* controller)
{
	return false;
}

void UAIAct_Death::StartAction(AAIController* Controller)
{
}
